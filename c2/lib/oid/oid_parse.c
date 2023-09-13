#include <hash.h>
#include <mesg.h>
#include <oid.h>

#undef ccp
#undef uccp
#define ccp 	const char *
#define uccp 	const unsigned char *

extern int oo_verbose;

static unsigned char *
nextfield(unsigned char *s)
{
  while (*s && '\t' != *s)
    ++s;
  if (*s)
    {
      *s++ = '\0';
      return s;
    }
  else
    return NULL;
}

#define op_error(x) mesg_verr(mesg_mloc(o->file,i),"oid parse error: "x),++op_status

int
oid_parse(Oids *o, enum oid_tab_t t)
{
  size_t i;
  int op_status = 0;

  o->h = hash_create(2048);
  o->o = calloc(o->nlines, sizeof(struct oid));
  o->oo = calloc(o->nlines, sizeof(struct oid *));

  for (i = 0; i < o->nlines; ++i)
    {
      unsigned char *s = o->lines[i];
      o->oo[i] = &o->o[i];
      if (ot_oids == t)
	{
	  o->oo[i]->id = (ccp)s;
	  s = nextfield(s);

	  if (hash_find(o->h, (uccp)o->oo[i]->id))
	    mesg_verr(mesg_mloc(o->file,i), "duplicate OID %s", o->oo[i]->id), ++op_status;
	  else if (t != ot_keys)
	    hash_add(o->h, (uccp)o->oo[i]->id, o->oo[i]);
	}
      if (s)
	{
	  o->oo[i]->domain = (ccp)s;
	  s = nextfield(s);

	  if (!oid_domain(o->oo[i]->domain, strlen(o->oo[i]->domain)))
 	    mesg_verr(mesg_mloc(o->file,i), "unknown DOMAIN %s", o->oo[i]->domain), ++op_status;

	  if (s)
	    {
	      o->oo[i]->key = s;
	      s = nextfield(s);
	      const char *dk = oid_domainify(o->oo[i]->domain, (ccp)o->oo[i]->key);

	      if (hash_find(o->h, (uccp)dk))
		{
		  if ('o' != *o->oo[i]->key && 'x' != *o->oo[i]->key && strcmp((ccp)o->oo[i]->key, "deleted"))
		    mesg_verr(mesg_mloc(o->file,i), "duplicate KEY %s:%s", o->oo[i]->domain, o->oo[i]->key), ++op_status;		  
		}
	      else
		{
		  if (t != ot_keys && 'o' != *o->oo[i]->key && 'x' != *o->oo[i]->key &&strcmp((ccp)o->oo[i]->key, "deleted"))
		    {
		      if (oo_verbose > 1)
			fprintf(stderr, "oid_parse adding %s <=> %s\n", dk, o->oo[i]->id);
			
		      hash_add(o->h, (uccp)strdup(dk), o->oo[i]);
		    }
		}

	      if (s)
		{
		  o->oo[i]->type = (ccp)s;
		  s = nextfield(s);
		  if (!oid_type(o->oo[i]->type, strlen(o->oo[i]->type)))
		    mesg_verr(mesg_mloc(o->file,i), "unknown TYPE %s", o->oo[i]->type), ++op_status;
		    
		  if (s)
		    {
		      o->oo[i]->extension = (ccp)s;
		      s = nextfield(s);
		      /* split list and check they are all legal OIDs */
		      if (s != NULL)
			op_error("unexpected data after EXTENSION");
		    }
		  /* else OK--we allow a missing last field */
		}
	      else
		op_error("expected TYPE");
	    }
	  else
	    op_error("expected KEY");
	}
      else
	op_error("expected DOMAIN");
    }
  return op_status;
}

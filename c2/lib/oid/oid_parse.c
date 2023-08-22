#include <oid.h>

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

void
oid_parse(Oids *o, enum oid_tab_t t)
{
  size_t i;
  
  o->o = calloc(o->nlines, sizeof(struct oid));
  o->oo = calloc(o->nlines, sizeof(struct oid *));

  for (i = 0; i < o->nlines; ++i)
    {
      unsigned char *s = o->lines[i];
      o->oo[i] = &o->o[i];
      if (ot_oids == t)
	{
	  o->o[i]->id = s;
	  s = nextfield(s);

	  if (hash_find(o->h, o->o[i]->id))
	    mesg_verr(mesg_mloc(o->file,i), "duplicate OID %s", o->o[i]->id);
	  else
	    hash_add(o->h, o->o[i]->id, o->oo[i]);
	}
      if (s)
	{
	  o->o[i]->domain = s;
	  s = nextfield(s);

	  if (!oid_known_domain(o->o[i]->domain))
 	    mesg_verr(mesg_mloc(o->file,i), "unknown DOMAIN %s", o->o[i]->domain);

	  if (s)
	    {
	      o->o[i]->key = s;
	      s = nextfield(s);

	      if (hash_find(o->h, o->o[i]->key))
		mesg_verr(mesg_mloc(o->file,i), "duplicate KEY %s", o->o[i]->id);
	      else
		hash_add(o->h, o->o[i]->key, o->oo[i]);

	      if (s)
		{
		  o->o[i]->type = s;
		  s = nextfield(s);
		  if (!oid_known_type(o->o[i]->type))
		    mesg_verr(mesg_mloc(o->file,i), "unknown TYPE %s", o->o[i]->type);
		    
		  if (s)
		    {
		      o->[i]->extension = s;
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
}

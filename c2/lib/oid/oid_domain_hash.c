#include <oraccsys.h>
#include <hash.h>
#include <oid.h>

extern int oo_verbose;

Hash *
oid_domain_hash(Oids *o, const char *otab, const char *domain)
{
  Hash *h = NULL;
  if (!o)
    {
      oid_set_oidtab(otab);
      o = oid_load();
    }
  if (o && o->nlines)
    {
      h = hash_create(2048);
      int i;
      for (i = 0; i < o->nlines; ++i)
	{
	  unsigned char *s = o->lines[i];
	  unsigned char *t = (ucp)strchr((ccp)s, '\t');
	  if (t)
	    {
	      unsigned char *k = (ucp)strchr((ccp)(t+1), '\t');
	      if (k)
		{
		  *t++ = '\0';
		  *k++ = '\0';
		  if (!strcmp((ccp)t, domain))
		    {
		      unsigned char *e = k;
		      while (*e)
			{
			  if ('\t' == *e)
			    *e++ = '\0';
			  else if ('\n' == *e)
			    {
			      *e = '\0';
			      break;
			    }
			  else
			    ++e;
			}
		      if (oo_verbose > 1)
			fprintf(stderr, "oid_domain_hash adding %s <=> %s\n", s, k);
		      hash_add(h, s, k);
		      hash_add(h, k, s);
		      s = e;
		    }
		  else
		    s = k;
		}		      
	    }
	}
    }
  return h;
}

#include <oraccsys.h>
#include <hash.h>

extern int verbose;

const char *
oid_tab_file(void)
{
  const char *obuilds = NULL;
  char *fn = malloc(strlen((obuilds = oracc_builds()))+strlen("/oid/oid.tabX"));
  (void)sprintf(fn, "%s%s", obuilds, "/oid/oid.tab");
  return fn;
}

Hash *
oid_load(const char *domain)
{
  const char *oidfn = NULL;
  unsigned char *oids = slurp("oid_load", (oidfn = oid_tab_file()), NULL);
  Hash *h = NULL;
  if (oids)
    {
      unsigned char *s;
      h = hash_create(2048);
      for (s = oids; *s; ++s)
	{
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
		      if (verbose > 1)
			fprintf(stderr, "oid_load adding %s <=> %s\n", s, k);
		      hash_add(h, s, k);
		      hash_add(h, k, s);
		      s = e;
		    }
		  else
		    s = k;
		}		      
	    }
	  while (*s && '\n' != *s)
	    ++s;
	}      
    }
  free((void*)oidfn);
  return h;
}

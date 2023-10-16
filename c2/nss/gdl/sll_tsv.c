#include <string.h>
#include <ctype128.h>
#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

extern int sll_trace;
extern Hash *sll_sl;
extern const char *oracc;
extern Pool *sllpool;
/*static List *kstrip = NULL;*/
static Hash *h = NULL;

#if 0
static void
sll_kstrip(void *k)
{
  unsigned char *ks = sll_strip_pp((unsigned char *)k);
  if (strcmp((const char *)ks,(const char *)k))
    {
      unsigned char *kh = hash_find(h,ks);
      if (kh)
	; /* fprintf(stderr, "sll_init_h: stripped key %s=>%s duplicates known key with OID %s\n",
	     (const char*)k, (const char*)ks, (const char*)kh);*/
      else
	{
	  kh = hash_find(h,k);
	  if (kh)
	    {
	      if (sll_trace)
		fprintf(stderr, "adding stripped version %s for %s with OID %s\n",
			(const char *)ks, (const char *)k, (const char *)kh);
	      hash_add(h,pool_copy(ks, sllpool),kh);
	    }
	  else
	    fprintf(stderr, "sll: internal error: no OID for key %s stripped to %s\n", (const char *)k, ks);
	}
    }
}
#endif

Hash *
sll_init_t(const char *project, const char *name)
{
  char *tsv_file;
  unsigned char *tsv_data = NULL, *p;
  ssize_t fsiz;

  sll_init();
  sll_init_si();

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  if (!name)
    name = "ogsl";

  oracc = oracc_home();
  tsv_file = (char *)pool_alloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + strlen("/sl.tsv") + 1, sllpool);
  sprintf(tsv_file, "%s/pub/%s/sl/sl.tsv", oracc, project);

  tsv_data = slurp("sll", tsv_file, &fsiz);
  if (tsv_data)
    {
      sllpool = pool_init();
  
      if (sll_trace)
	fprintf(stderr, "sll: slurped %s\n", tsv_file);
      h = hash_create(1024);
#if 0
      kstrip = list_create(LIST_SINGLE);
#endif
      for (p = tsv_data; *p; )
	{
	  unsigned char *k = p, *v = NULL;

	  /* The #letters appendix in sl.tsv is not used in the
	     internalized signlist only in the dbi */
	  if ('#' == *p && !strncmp((ccp)p, "#letters", strlen("#letters")))
	    break;

	  while (*p && '\t' != *p)
	    ++p;
	  if (*p)
	    {
	      *p++ = '\0';
	      v = p;
	      while (*p && '\n' != *p)
		++p;
	      if ('\n' == *p)
		*p++ = '\0';
	      if (v)
		{
		  if (sll_trace)
		    fprintf(stderr, "sll: adding %s = %s\n", k, v);
		  hash_add(h, k, v);
#if 0
		  if ('o' == *v && isdigit(v[1]))
		    {
		      if (!strchr((ccp)k,';') && sll_has_sign_indicator(k))
			{
			  unsigned char *k2 = hash_find(h,v);
			  if (k2)
			    fprintf(stderr, "sll_init_t: duplicate key/val %s = %s\n", k, v);
			  else
			    {
			      if (sll_trace)
				fprintf(stderr, "sll: adding %s = %s\n", v, k);
			      hash_add(h,v,k);
			    }
			  if (strpbrk((const char *)k,"()+"))
			    list_add(kstrip, k);
			}
		    }
#endif
		}
	    }
	}
#if 0
      list_exec(kstrip, (list_exec_func*)sll_kstrip);
      list_free(kstrip, NULL);
#endif
    }
  else
    {
      fprintf(stderr, "sll_init_t: failed to load %s\n", tsv_file);
    }

  return sll_sl = h;
}

void
sll_term_t(Hash *h)
{
  hash_free(h, NULL);
  sll_term();
}

unsigned const char *
sll_lookup_t(unsigned const char *key)
{
  return key ? hash_find(sll_sl, (const unsigned char *)key) : NULL;
}

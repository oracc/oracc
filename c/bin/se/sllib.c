#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include "sllib.h"

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

static const char *oracc = NULL;

static int sllib_verbose = 0;

static int signindicator[256];

/* Every sign name has at least one of these uppercase letters--this
   is validated by sl-xml */
void
sl_init_si(void)
{
  signindicator['A'] = 
    signindicator['E'] = 
    signindicator['I'] = 
    signindicator['U'] = 
    signindicator['F'] = 
    signindicator['N'] = 
    signindicator['O'] = 
    signindicator['P'] = 
    signindicator['S'] = 
    signindicator['X'] = 1;
}

int
has_sign_indicator(unsigned const char *g)
{
  while (*g)
    {
      if (signindicator[*g])
	return 1;
      ++g;
    }
  return 0;
}

unsigned const char *
sl_lookup_d(Dbi_index *dbi, unsigned const char *key)
{
  if (dbi && key)
    {
      dbi_find(dbi,key);
      if (dbi->data)
	return dbi->data;
    }
  return NULL;
}

Dbi_index *
sl_init_d(const char *project, const char *name)
{
  Dbi_index *dbi = NULL;
  char *db;

  sl_init_si();
    
  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  if (!name)
    name = "ogsl";

  oracc = oracc_home();
  db = malloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + 1);
  sprintf(db, "%s/pub/%s/sl", oracc, project);

  if (!(dbi = dbi_open(name, db)))
    fprintf(stderr, "sllib: failed to open %s (project=%s; name=%s)\n", db, project, name);

  return dbi;
}

void
sl_term_d(Dbi_index *dbi)
{
  if (dbi)
    dbi_close(dbi);
}

Hash_table *
sl_init_h(const char *project, const char *name)
{
  Hash_table *h = NULL;
  char *tsv_file;
  unsigned char *tsv_data = NULL, *p;
  ssize_t fsiz;

  sl_init_si();

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  if (!name)
    name = "ogsl";

  oracc = oracc_home();
  tsv_file = malloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + strlen("/sl.tsv") + 1);
  sprintf(tsv_file, "%s/pub/%s/sl/sl.tsv", oracc, project);

  tsv_data = slurp("sllib", tsv_file, &fsiz);
  if (tsv_data)
    {
      h = hash_create(1024);
      for (p = tsv_data; *p; )
	{
	  unsigned char *k = p, *v = NULL;
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
		  hash_add(h, k, v);
		  if ('o' == *v && isdigit(v[1]))
		    {
		      if (!strchr((ccp)k,';') && has_sign_indicator(k))
			{
			  if (hash_find(h,v))
			    fprintf(stderr, "sl_init_h: duplicate key/val %s = %s\n", k, v);
			  else
			    hash_add(h,v,k);
			}
		    }
		  if (sllib_verbose)
		    fprintf(stderr, "sl_init_h: adding k %s = v %s\n", k, v);
		}
	    }
	}
    }
  else
    {
      fprintf(stderr, "sl_init_h: failed to load %s\n", tsv_file);
    }

  return h;
}

void
sl_term_h(Hash_table *h)
{
}

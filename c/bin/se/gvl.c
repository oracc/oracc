/* Grapheme Validation Tool Kit */

#include <stdlib.h>
#include <stdio.h>

#include "gvl.h"
#include "npool.h"
#include "hash.h"

static gvl_i *sl = NULL; /* sl is always the head of the list of signlist datasets, not necessarily the current one */
static gvl_i *curr_sl = NULL; /* the sl that should be used for look up */
static int tsv = 0;

const char *(*gvl_lookup)(const char *key);

static gvl_i *gvl_i_init_d(const char *name, Dbi_index *dbi);
static gvl_i *gvl_i_init_h(const char *name, Hash_table *h);
static void   gvl_i_term(const char *name);

static const char *
gvl_lookup_d(const char *key)
{
  return sl_lookup_d(curr_sl->u.d,key);
}

static const char *
gvl_lookup_h(const char *key)
{
  return hash_find(curr_sl->u.h, (const unsigned char *)key);
}

gvl_i*
gvl_setup(const char *project, const char *name, int arg_tsv)
{
  gvl_i *ret = NULL;
  tsv = arg_tsv;
  if (tsv)
    {
      Hash_table *h = NULL;
      
      gvl_lookup = gvl_lookup_h;

      if ((h = sl_init_h(project, name)))
	ret = gvl_i_init_h(name, h);
      else
	fprintf(stderr, "sl: failed to open TSV %s/%s\n", (char *)project, (char*)name);
    }
  else
    {
      Dbi_index *dbi = NULL;

      gvl_lookup = gvl_lookup_d;

      if ((dbi = sl_init_d(project, name)))
	ret = gvl_i_init_d(name, dbi);
      else
	fprintf(stderr, "sl: failed to open DBI %s/%s\n", (char *)project, (char*)name);
    }

  ret->tsv = arg_tsv;
  return ret;
}
      
void
gvl_wrapup(const char *name)
{
  gvl_i_term(name);
}

/* Don't error here; caller must decide if not finding the gvl is an error or not */
gvl_i *
gvl_i_find(const char *name)
{
  gvl_i *tmp = sl;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	return tmp;
      else
	tmp = tmp->next;
    }
  return NULL;
}

gvl_i *
gvl_i_init_(const char *name)
{
  gvl_i *p = NULL;

  if (!name)
    return NULL;
  
  if (!(p = malloc(sizeof(struct gvl_i))))
    return NULL;
  
  if (!sl)
    {
      sl = p;
      sl->prev = sl->next = NULL;
    }
  else
    {
      gvl_i *tmp = NULL;
      if ((tmp = gvl_i_find(name)))
	{
	  free(p);
	  curr_sl = tmp;
	  return tmp;
	}

      p->prev = NULL;
      p->next = sl;
      sl->prev = p;
      sl = p;
    }
  curr_sl = sl;
  sl->h = hash_create(1024);
  sl->p = npool_init();

  if (!sl->h || !sl->p)
    return NULL;

  return sl;
}

gvl_i *
gvl_i_init_d(const char *name, Dbi_index *dbi)
{
  gvl_i *p = NULL;
  if (dbi)
    {
      p = gvl_i_init_(name);
      if (!p->u.d)
	p->u.d = dbi;
    }
  return p;
}

gvl_i *
gvl_i_init_h(const char *name, Hash_table *h)
{
  gvl_i *p = NULL;
  if (h)
    {
      p = gvl_i_init_(name);
      if (!p->u.h)
	p->u.h = h;
    }
  return p;  
}

void
gvl_i_term(const char *name)
{
  gvl_i *tmp = sl;
  int found = 0;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	{
	  gvl_i *next = tmp->next;
	  if (tmp->tsv)
	    hash_free(tmp->u.h, NULL);
	  else
	    sl_term_d(tmp->u.d);
	  hash_free(tmp->h, NULL);
	  npool_term(tmp->p);
	  free(tmp);
	  sl = next;
	  ++found;
	  goto ret;
	}
      else
	tmp = tmp->next;
    }
 ret:
  if (!found)
    fprintf(stderr, "gvl_i named '%s' not found during gvl_i_term\n", name);
}

gvl_g *
gvl_validate(const char *g)
{
  fprintf(stderr, "hello g=%s\n", g);
  return NULL;
}

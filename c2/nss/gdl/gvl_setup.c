#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <oraccsys.h>
#include <oracclocale.h>
#include <memo.h>
#include <pool.h>
#include <hash.h>
#include <tree.h>

#include "sll.h"
#include "gvl.h"

int gvl_void_messages; /* when non-zero only complain about structural
			  issues not about unknown signs and other
			  things that are OK if we are parsing a sign
			  list */

static gvl_i *gvl_i_init_h(const char *name, Hash *h);
static void   gvl_i_term(const char *name);
static gvl_i *sl = NULL; /* sl is always the head of the list of signlist datasets, not necessarily the current one */
gvl_i *curr_sl = NULL; /* the sl that should be used for look up */

gvl_i*
gvl_setup(const char *project, const char *name)
{
  gvl_i *ret = NULL;
  Hash *h = NULL;
  const char *l = NULL;

  if (!(l = setlocale(LC_ALL,ORACC_LOCALE)))
    if (!(l = setlocale(LC_ALL, "en_US.UTF-8")))
      if (!(l = setlocale(LC_ALL, "C")))
        fprintf(stderr, "gvl_setup: failed to setlocale to '%s', 'UTF-8', or 'C'\n", ORACC_LOCALE);

  if (project && name)
    {
      if ((h = sll_init_t(project, name)))
	{
	  ret = gvl_i_init_h(name, h);
	  sll_set_sl(ret->sl);
	}
      else
	fprintf(stderr, "gvl: failed to open TSV %s/%s\n", (char *)project, (char*)name);
    }
  else
    {
      h = hash_create(1);
      sll_init();
      sll_init_si();
      sll_set_sl(h);
      ret = gvl_i_init_h("voidsl", h);
      gvl_void_messages = 1;
    }

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
  
  if (!(p = calloc(1, sizeof(struct gvl_i))))
    return NULL;

  p->n = name;
  
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
  sl->m = memo_init(sizeof(gvl_g), 1024);
  sl->p = pool_init();

  if (!sl->h || !sl->p)
    return NULL;

  return sl;
}

gvl_i *
gvl_i_init_h(const char *name, Hash *h)
{
  gvl_i *p = NULL;
  if (h)
    {
      p = gvl_i_init_(name);
      if (!p->sl)
	p->sl = h;
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
	  hash_free(tmp->sl, NULL);
	  hash_free(tmp->h, NULL);
	  pool_term(tmp->p);
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

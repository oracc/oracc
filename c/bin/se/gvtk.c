/* Grapheme Validation Tool Kit */

#include "gvtk.h"
#include "npool.h"
#include "hash.h"


struct gvtk_i {
  const char *n;	/* name of SL DB */
  Dbi_index *d;		/* SL DB index */
  Hash_table *h;	/* Hash of items validated */
  struct npool *p;	/* General purpose string pool */
  struct gvtk_i *prev;
  struct gvtk_i *next;
};

typedef gvtk_i gvtk_i;

static gvtk_i *sl = NULL;
static const char *curr_sl = NULL;

void
gvtk_init(const char *name, Dbi_index *dbi)
{
  gvtk_i *p = malloc(sizeof(struct gvtk_i));
  if (!sl)
    {
      sl = p;
      sl->prev = sl->next = NULL;
    }
  else
    {
      if (gvtk_find(name))
	{
	  free(p);
	  return;
	}

      p->prev = NULL;
      p->next = sl;
      sl->prev = p;
      sl = p;
    }
  sl->n = name;
  sl->d = dbi;
  sl->h = hash_create(1024);
  sl->p = npool_init();
}

/* Don't error here; caller must decide if not finding the gvkt is an error or not */
gvtk *
gvtk_find(const char *name)
{
  gvtk_i *tmp = sl;
  while (tmp)
    {
      if (!strcmp(tmp->name, name))
	return tmp;
      else
	tmp = tmp->next;
    }
  return NULL;
}

void
gvtk_term(const char *name)
{
  gvtk_i *tmp = sl;
  int found = 0;
  while (tmp)
    {
      if (!strcmp(tmp->name, name))
	{
	  gvtk_i *next = tmp->next;
	  sl_term(tmp->d);
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
    fprintf(stderr, "gvtk named '%s' not found\n", name);
}

gvtk_p
gvtk_validate(const char *g)
{
}

#include <stddef.h>
#include <gutil.h>
#include <signlist.h>
#include <sx.h>

int
g_index_of(const unsigned char *g, const unsigned char *b)
{
  int i = 0;
  if (!b)
    b = g_base_of(g);
  g += strlen((ccp)b);
  if (strlen((ccp)g) == 6)
    {
      i = ((g[2] - 0x80)) * 10;
      g += 3;
    }
  i += (g[2] - 0x80);
  if (i == 13) /* subscript x */
    i = 1000;
  return i;
}

extern Hash *oid_sort_keys;
static int
spv_cmp(const void *a, const void *b)
{
  struct sl_split_value *spv1 = (*(struct sl_split_value**)a);
  struct sl_split_value *spv2 = (*(struct sl_split_value**)b);
  int a1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv1->oid);
  int b1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv2->oid);
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

/* No need to store split_value data in value because for
 * sl->values[i] the split_value is in sl->splitv[i]
 */
void
sx_homophones(struct sl_signlist *sl)
{
  int i;
  Hash *xhomophones = hash_create(1024);
  
  sl->homophones = hash_create(1024);
  sl->splitv = memo_new_array(sl->m_split_v, sl->nvalues);
  for (i = 0; i < sl->nvalues; ++i)
    {
      List *lp = NULL;
      if (sl->values[i]->xvalue)
	{
	  struct sl_inst *ip = NULL;

	  sl->splitv[i].b = sl->values[i]->name;
	  sl->splitv[i].i = 0; /* 1 = no index (i.e., index 1); 0 = ₓ-index */
	  
	  for (ip = list_first(sl->values[i]->fowners); ip; ip = list_next(sl->values[i]->fowners))
	    {
	      struct sl_split_value *sp = memo_new(sl->m_split_v);
	      const char *oid;
	      *sp = sl->splitv[i];
	      if ('s' == ip->type)
		oid = ip->u.s->oid;
	      else if ('f' == ip->type)
		oid = ip->u.f->oid;
	      else
		mesg_verr(ip->mloc, "unexpected type in homophone (expected 's' or 'f'; found '%c'", ip->type);
	      if (oid)
		{
		  sp->oid = oid;
		  if (!(lp = hash_find(xhomophones, (uccp)sl->splitv[i].b)))
		    hash_add(xhomophones, (uccp)sl->splitv[i].b, (lp = list_create(LIST_SINGLE)));
		  list_add(lp, sp);
		}
	    }
	}
      else
	{
	  const char *oid;
	  struct sl_inst *ip = NULL;
	  if (sl->values[i]->sowner)
	    ip = sl->values[i]->sowner->inst;
	  else
	    ip = list_first(sl->values[i]->fowners);
	  if ('s' == ip->type)
	    oid = ip->u.s->oid;
	  else if ('f' == ip->type)
	    oid = ip->u.f->oid;
	  else
	    mesg_verr(ip->mloc, "unexpected type in homophone (expected 's' or 'f'; found '%c'", ip->type);

	  if (oid)
	    sl->splitv[i].oid = oid;

	  sl->splitv[i].b = pool_copy(g_base_of(sl->values[i]->name), sl->p);

	  if (strlen((ccp)sl->splitv[i].b) == strlen((ccp)sl->values[i]->name))
	    sl->splitv[i].i = 1;
	  else
	    sl->splitv[i].i = g_index_of(sl->values[i]->name, sl->splitv[i].b);

	  if (!(lp = hash_find(sl->homophones, sl->splitv[i].b)))
	    hash_add(sl->homophones, sl->splitv[i].b, (lp = list_create(LIST_SINGLE)));
	  
	  list_add(lp, &sl->splitv[i]);
	}
    }

  /* Now we have the main homophones hash and a separate one for the
     x-values; we need to append the x-values to the main homophones,
     sorting them by sign-/form-name first */
  if (xhomophones->key_count)
    {
      const char **keys;
      int nkeys, i, j;
      keys = hash_keys2(xhomophones, &nkeys);
      for (i = 0; i < nkeys; ++i)
	{
	  List *lp = hash_find(xhomophones, (uccp)keys[i]);
	  List *lph = NULL;
	  struct sl_split_value **spv = NULL;
	  unsigned const char *xbase = pool_copy(g_base_of((uccp)keys[i]), sl->p); /* get the base without 'ₓ' */

	  if (list_len(lp) > 1)
	    {
	      spv = (struct sl_split_value**)list2array(lp);
	      qsort(spv, list_len(lp), sizeof(struct sl_split_value *), (cmp_fnc_t)spv_cmp);
	    }
	  else
	    {
	      spv = malloc(sizeof(struct sl_split_value*));
	      spv[0] = list_first(lp);
	    }

	  if (!(lph = hash_find(sl->homophones, xbase)))
	    hash_add(sl->homophones, xbase, (lph = list_create(LIST_SINGLE)));

	  for (j = 0; j < list_len(lp); ++j)
	    list_add(lph, spv[j]);

	  free(spv);
	}
    }
}

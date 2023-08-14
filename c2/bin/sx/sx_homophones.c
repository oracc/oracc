#include <stddef.h>
#include <gutil.h>
#include <signlist.h>
#include <sx.h>

int htrace = 0;

extern Hash *oid_sort_keys;
static int
spv_cmp(const void *a, const void *b)
{
  struct sl_split_value *spv1 = (*(struct sl_split_value**)a);
  struct sl_split_value *spv2 = (*(struct sl_split_value**)b);
  if (spv1->oid && spv2->oid)
    {
      int a1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv1->oid);
      int b1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)spv2->oid);
      if (a1 < b1)
	return -1;
      else if (a1 > b1)
	return 1;
      else
	return 0;
    }
  return 0;
}

static void
sx_h_sub(struct sl_signlist *sl, Hash *xh, unsigned const char *vname, int xvalue, const char *oid)
{
  unsigned const char *b = pool_copy(g_base_of(vname), sl->p);
  int index = g_index_of(vname, b);
  struct sl_split_value *spv = memo_new(sl->m_split_v);
  spv->b = b;
  spv->i = index;
  spv->oid = oid;
  List *lp;
  if (xvalue)
    {
      if (!(lp = hash_find(xh, b)))
	hash_add(xh, b, (lp = list_create(LIST_SINGLE)));
    }
  else
    {
      if (!(lp = hash_find(sl->homophones, b)))
	hash_add(sl->homophones, b, (lp = list_create(LIST_SINGLE)));
    }
  if (htrace)
    fprintf(stderr, "htrace: adding %s with base %s/index %d and OID %s\n", vname, spv->b, spv->i, oid);
  list_add(lp, spv);
}

void
sx_homophones(struct sl_signlist *sl)
{
  int i;
  Hash *xhomophones = hash_create(1024);
  sl->homophones = hash_create(1024);
  
  for (i = 0; i < sl->nvalues; ++i)
    {
      if (sl->values[i]->parents)
	{
	  if (sl->values[i]->parents->signs)
	    {
	      struct sl_inst *ip;
	      for (ip = list_first(sl->values[i]->parents->signs);
		   ip;
		   ip = list_next(sl->values[i]->parents->signs))
		sx_h_sub(sl, xhomophones, sl->values[i]->name,
			 sl->values[i]->xvalue, ip->u.s->oid);
	    }
	  if (sl->values[i]->parents->forms)
	    {
	      struct sl_inst *ip;
	      for (ip = list_first(sl->values[i]->parents->forms);
		   ip;
		   ip = list_next(sl->values[i]->parents->forms))
		{
		  sx_h_sub(sl, xhomophones, sl->values[i]->name,
			   sl->values[i]->xvalue, ip->u.f->oid);
		}
	    }
	}
    }
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

	  if (!(lph = hash_find(sl->homophones, (uccp)keys[i])))
	    hash_add(sl->homophones, (uccp)keys[i], (lph = list_create(LIST_SINGLE)));

	  for (j = 0; j < list_len(lp); ++j)
	    list_add(lph, spv[j]);

	  free(spv);
	}
    }
}

/* No need to store split_value data in value because for
 * sl->values[i] the split_value is in sl->splitv[i]
 */
void
sx_xhomophones(struct sl_signlist *sl)
{
  int i;
  Hash *xhomophones = hash_create(1024);
  
  sl->homophones = hash_create(1024);
  sl->splitv = memo_new_array(sl->m_split_v, sl->nvalues);
  for (i = 0; i < sl->nvalues; ++i)
    {
      List *lp = NULL;

      if (sl->values[i]->atf)
	continue;

      if (sl->values[i]->xvalue)
	{
	  struct sl_inst *ip = NULL;

	  sl->splitv[i].b = sl->values[i]->name;
	  sl->splitv[i].i = 0; /* 1 = no index (i.e., index 1); 0 = ₓ-index */

#if 0
	  for (ip = list_first(sl->values[i]->fowners); ip; ip = list_next(sl->values[i]->fowners))
#else
	  for (ip = list_first(sl->values[i]->insts); ip; ip = list_next(sl->values[i]->insts))
#endif	    
	    {
	      /* ignore @v- */
	      if (!ip->valid)
		continue;
	      
	      struct sl_split_value *sp = memo_new(sl->m_split_v);
	      const char *oid = NULL;
	      *sp = sl->splitv[i];
#if 0
	      if ('s' == ip->type)
		oid = ip->u.s->oid;
	      else if ('f' == ip->type)
		oid = ip->u.f->oid;
	      else
		mesg_verr(&ip->mloc, "unexpected type in homophone (expected 's' or 'f'; found '%c'", ip->type);
#else
	      if (ip->parent_f)
		oid = ip->parent_f->u.f->oid;
	      else if (ip->parent_s)
		oid = ip->parent_s->u.f->oid;
	      else
		mesg_verr(&ip->mloc, "unexpected type in homophone (expected 's' or 'f'; found '%c'", ip->type);
#endif
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
	    mesg_verr(&ip->mloc, "unexpected type in homophone (expected 's' or 'f'; found '%c'", ip->type);

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

#include <list.h>
#include <signlist.h>
#include <sx.h>

static struct sl_signlist *cmpsl = NULL;
static int val_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  struct sl_value *sa1 = hash_find(cmpsl->hventry, (uccp)cc1);
  struct sl_value *sb1 = hash_find(cmpsl->hventry, (uccp)cc2);
  int a1 = sa1->sort;
  int b1 = sb1->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

void
sx_values_by_oid(struct sl_signlist *sl)
{
  Hash *h = hash_create(1024), *h2 = hash_create(1024);
  const char **keys;
  int nkeys, i;
  
  for (i = 0; i < sl->nvalues; ++i)
    {
      if (sl->values[i]->sowner)
	{
	  List *lp;
	  if (!(lp = hash_find(h, (uccp)sl->values[i]->sowner->oid)))
	    {
	      lp = list_create(LIST_SINGLE);
	      hash_add(h, (uccp)sl->values[i]->sowner->oid, lp);
	    }
	  list_add(lp, (void*)sl->values[i]->name);
	}
      if (sl->values[i]->fowners)
	{
	  struct sl_inst *ip;
	  for (ip = list_first(sl->values[i]->fowners); ip; ip = list_next(sl->values[i]->fowners))
	    {
	      List *lp;
	      unsigned const char *oid = (uccp)(ip->type == 's' ? ip->u.s->oid : ip->u.f->sign->oid);
	      if (!(lp = hash_find(h, (uccp)oid)))
		{
		  lp = list_create(LIST_SINGLE);
		  hash_add(h, (uccp)oid, lp);
		}
	      list_add(lp, (void*)sl->values[i]->name);
	    }
	}
    }
  keys = hash_keys2(h, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      List *lp = hash_find(h, (uccp)keys[i]);
      const char **vals = list2chars(lp);
      cmpsl = sl;
      qsort(vals, list_len(lp), sizeof(const char *), (cmp_fnc_t)val_cmp);
      cmpsl = NULL;
      hash_add(h2, (uccp)keys[i], vals);
    }
  sl->values_by_oid = h2;
}

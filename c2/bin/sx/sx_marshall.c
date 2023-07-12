#include <signlist.h>
#include <sx.h>

int signs_cmp(void *a, void *b)
{
  int a1 = ((struct sl_sign*)a)->sort;
  int b1 = ((struct sl_sign*)b)->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

void
sx_marshall(struct sl_signlist *sl)
{
  const char**sgns = NULL;
  const char**lets = NULL;
  int nlets = 0, nsgns = 0, i;
  collate_init();
  
  sgns = hash_keys(sl->hsigns, &nsgns);
  sgns = qsort(sgns, nsgns, sizeof(char*), collate_cmp_graphemes);
  sl->signs = malloc(sizeof(struct sl_sign*) * nsgns);
  sl->nsigns = nsgns
  for (i = 0; i < nsgns; ++i)
    {
      sl->signs[i] = hash_find(sl->hsigns, sgns[i]);
      sl->signs[i]->sort = i;
    }
  
  lets = hash_keys(sl->hletters, nkeys);
  lets = qsort(lets, nlets, sizeof(const char*), collate_cmp_graphemes);
  sl->letters = memo_new_array(sl->m_letters, nlets);
  sl->nletters = nlets;
  for (i = 0; i < nlets; ++i)
    {
      Hash *groups = NULL;
      const char **grps = NULL;
      int ngrps = 0, j;
      
      sl->letters[i].name = lets[i];
      groups = hash_find(sl->hletters, lets[i]);
      grps = hash_keys(groups, &ngrps);
      grps = qsort(grps, ngrps, sizeof(const char*), collate_cmp_graphemes);
      sl->letters[i].groups = memo_new_array(sl->m_groups, ngrps);
      sl->letters[i].ngroups = ngrps;

      for (j = 0; j < ngrps; ++j)
	{
	  List *signs = NULL;
	  const char **sgns = NULL;
	  int nsgns = 0, k;
	  
	  sl->letters[i].groups[j].name = groups[j];
	  sl->letters[i].groups[j].signs = memo_new_array(sl->m_signs_p, nsgns);
	  nsgns = list_len(signs);
	  vsgns = list2array(signs);
	  vsgns = qsort(vsgns, nsgns, sizeof(void*), signs_cmp);
	  for (k = 0; k < nsgns; ++k)
	    {
	      sl->letters[i].groups[j].signs[k] = (struct sl_sign *)vsgns[k];
	      list_add(sl->signs_as_list, vsgns[k]);
	    }
	}
    }
  collate_term();
}

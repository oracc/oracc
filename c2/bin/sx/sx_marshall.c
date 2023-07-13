#include <collate.h>
#include <signlist.h>
#include <sx.h>

int signs_cmp(const void *a, const void *b)
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
  collate_init((ucp)"unicode");
  
  sgns = hash_keys2(sl->hsigns, &nsgns);
  qsort(sgns, nsgns, sizeof(char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->signs = malloc(sizeof(struct sl_sign*) * nsgns);
  sl->nsigns = nsgns;
  for (i = 0; i < nsgns; ++i)
    {
      sl->signs[i] = hash_find(sl->hsigns, (ucp)sgns[i]);
      sl->signs[i]->sort = i;
    }

  /* Dereference structures created in asl_bld.c--see that file for AB1/AB2/AB3 creation */
  lets = hash_keys2(sl->hletters, &nlets); /* obtain list of letters from AB1 */
  qsort(lets, nlets, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->letters = memo_new_array(sl->m_letters, nlets);
  sl->nletters = nlets;
  for (i = 0; i < nlets; ++i)
    {
      Hash *groups = NULL;
      const char **grps = NULL;
      int ngrps = 0, j;
      
      sl->letters[i].name = (ucp)lets[i];
      groups = hash_find(sl->hletters, (ucp)lets[i]);
      grps = hash_keys2(groups, &ngrps); /* obtain list of groups in letter from AB2 */
      qsort(grps, ngrps, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
      sl->letters[i].groups = memo_new_array(sl->m_groups, ngrps);
      sl->letters[i].ngroups = ngrps;

      for (j = 0; j < ngrps; ++j)
	{
	  List *slist = hash_find(groups, (ucp)grps[j]); /* obtain list of signs in group from AB3 */
	  sl->letters[i].groups[j].name = (ucp)grps[j];
	  sl->letters[i].groups[j].nsigns = list_len(slist);
	  sl->letters[i].groups[j].signs = memo_new_array(sl->m_signs_p,
							  sl->letters[i].groups[j].nsigns);
	  sl->letters[i].groups[j].signs = (struct sl_sign **)list2array(slist);
	  qsort(sl->letters[i].groups[j].signs,
		sl->letters[i].groups[j].nsigns, sizeof(void*), (cmp_fnc_t)signs_cmp);
	}
    }
  /*collate_term();*/
}

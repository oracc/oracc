#include <collate.h>
#include <signlist.h>
#include <sx.h>

static struct sl_sign *
form_as_sign(struct sl_signlist *sl, struct sl_form *f)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  struct sl_inst *i = list_first(f->insts);
  s->mloc = i->mloc;
  s->name = f->name;
  s->xref = f;
  asl_register_sign(i->mloc, sl, s);
  return s;
}

#if 0
static int forms_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_form**)a)->sort;
  int b1 = (*(struct sl_form**)b)->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}
#endif

static int forms_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.f->sort;
  int b1 = (*(struct sl_inst**)b)->u.f->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int lists_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.l->sort;
  int b1 = (*(struct sl_inst**)b)->u.l->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int values_inst_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_inst**)a)->u.v->sort;
  int b1 = (*(struct sl_inst**)b)->u.v->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int signs_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_sign**)a)->sort;
  int b1 = (*(struct sl_sign**)b)->sort;  
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
  const char**sgns = NULL, **frms = NULL, **vals = NULL, **lsts;
  const char**lets = NULL;
  int nlets = 0, nsgns = 0, nfrms = 0, nlsts = 0, nvals = 0, i;
  collate_init((ucp)"unicode");

  /* Add forms to the signs hash if they don't already exist as a sign */
  frms = hash_keys2(sl->hforms, &nfrms);
  for (i = 0; i < nfrms; ++i)
    {
      struct sl_sign *s;
      
      if (!(s = hash_find(sl->hsigns, (uccp)frms[i])))
	hash_add(sl->hsigns, (uccp)frms[i], form_as_sign(sl, hash_find(sl->hforms, (uccp)frms[i])));
      else
	{
	  struct sl_form *f = hash_find(sl->hforms, (uccp)frms[i]);
	  f->gdl = s->gdl;
	}
    }

  /* Sort the combined signs and forms-treated-as-signs */
  sgns = hash_keys2(sl->hsigns, &nsgns);
  qsort(sgns, nsgns, sizeof(char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->signs = malloc(sizeof(struct sl_sign*) * nsgns);
  sl->nsigns = nsgns;
  for (i = 0; i < nsgns; ++i)
    {
      sl->signs[i] = hash_find(sl->hsigns, (ucp)sgns[i]);
      sl->signs[i]->sort = i;
    }
  
  /* Provide forms with sort codes based on sign sort sequence */
  sl->forms = malloc(sizeof(struct sl_form*) * nfrms);
  for (i = 0; i < nfrms; ++i)
    {
      struct sl_sign *s = NULL;
      sl->forms[i] = hash_find(sl->hforms, (ucp)frms[i]);
      s = hash_find(sl->hsigns, (ucp)frms[i]);
      sl->forms[i]->sort = s->sort;
    }

  /* Create sort codes for lists--note that a listname used in @sign
     or @form has a different code than the same listname used in
     @list; all @list entries are sorted into a single sequence
     separate from the @sign sort */
  lsts = hash_keys2(sl->hsigns, &nlsts);
  qsort(lsts, nlsts, sizeof(char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->lists = malloc(sizeof(struct sl_sign*) * nlsts);
  sl->nlists = nlsts;
  for (i = 0; i < nlsts; ++i)
    {
      sl->lists[i] = hash_find(sl->hsigns, (ucp)lsts[i]);
      sl->lists[i]->sort = i;
    }
  
  /* Create sort codes for values--the sequence is completely independent of the sign sort codes */
  vals = hash_keys2(sl->hvalues, &nvals);
  qsort(vals, nvals, sizeof(char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->values = malloc(sizeof(struct sl_sign*) * nvals);
  sl->nvalues = nvals;
  for (i = 0; i < nvals; ++i)
    {
      sl->values[i] = hash_find(sl->hvalues, (ucp)vals[i]);
      sl->values[i]->sort = i;
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

  /* Sort the lists, values and forms for each sign */
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_sign *sp = sl->signs[i];
      if (sp->hlists)
	{
	  int nslsts, j;
	  const char **slsts = hash_keys2(sp->hlists, &nslsts);
	  sp->lists = memo_new_array(sl->m_insts, nslsts);
	  sp->nlists = nslsts;
	  for (j = 0; j < sp->nlists; ++j)
	    sp->lists[j] = hash_find(sp->hlists, (uccp)slsts[j]);
	  qsort(sp->lists, sp->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
	}
      if (sp->hvalues)
	{
	  int nsvals, j;
	  const char **svals = hash_keys2(sp->hvalues, &nsvals);
	  sp->values = memo_new_array(sl->m_insts, nsvals);
	  sp->nvalues = nsvals;
	  for (j = 0; j < sp->nvalues; ++j)
	    sp->values[j] = hash_find(sp->hvalues, (uccp)svals[j]);
	  qsort(sp->values, sp->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
	}
      if (sp->hforms)
	{
	  int nsfrms, j;
	  const char **sfrms = hash_keys2(sp->hforms, &nsfrms);
	  sp->forms = memo_new_array(sl->m_insts, nsfrms);
	  sp->nforms = nsfrms;
	  for (j = 0; j < sp->nforms; ++j)
	    {
	      struct sl_inst *fp;
	      fp = sp->forms[j] = hash_find(sp->hforms, (uccp)sfrms[j]);
	      if (fp->lv && fp->lv->hlists)
		{
		  int nslsts, j;
		  const char **slsts = hash_keys2(fp->lv->hlists, &nslsts);
		  fp->lv->lists = memo_new_array(sl->m_insts, nslsts);
		  fp->lv->nlists = nslsts;
		  for (j = 0; j < fp->lv->nlists; ++j)
		    fp->lv->lists[j] = hash_find(fp->lv->hlists, (uccp)slsts[j]);
		  qsort(fp->lv->lists, fp->lv->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
		}
	      if (fp->lv && fp->lv->hvalues)
		{
		  int nsvals, j;
		  const char **svals = hash_keys2(fp->lv->hvalues, &nsvals);
		  fp->lv->values = memo_new_array(sl->m_insts, nsvals);
		  fp->lv->nvalues = nsvals;
		  for (j = 0; j < fp->lv->nvalues; ++j)
		    fp->lv->values[j] = hash_find(fp->lv->hvalues, (uccp)svals[j]);
		  qsort(fp->lv->values, fp->lv->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
		}
	    }
	  qsort(sp->forms, sp->nforms, sizeof(void*), (cmp_fnc_t)forms_inst_cmp);
	}
    }

  /*collate_term();*/
}

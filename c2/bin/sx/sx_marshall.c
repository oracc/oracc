#include <collate.h>
#include <signlist.h>
#include <sx.h>

extern Hash * oid_load(const char *domain);

#if 0
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

static int toks_cmp(const void *a, const void *b)
{
  unsigned const char *t_a = ((struct sl_token*)a)->t;
  unsigned const char *t_b = ((struct sl_token*)b)->t;
  /* This cast is a hack; collate_cmp_graphemes expects its args to be unsigned char ** */
  return collate_cmp_graphemes((ucp)&t_a, (ucp)&t_b);
}

void
sx_marshall(struct sl_signlist *sl)
{
  const char**keys = NULL;
  int nkeys;
  Hash *oids;

  const char**lets = NULL;
  int nlets = 0, i;
  collate_init((ucp)"unicode");

  oids = oid_load("sl");
  if (!oids)
    oids = hash_create(1);
  
#if 0
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
#endif
  
  /* Sort the tokens and set the token sort codes */
  keys = hash_keys2(sl->htoken, &nkeys);
  qsort(keys, nkeys, sizeof(char*), (cmp_fnc_t)toks_cmp);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = hash_find(sl->htoken, (ucp)keys[i]);
      tp->s = i;
    }
  
  /* Provide signs with sort codes base on token sort sequence; add oids while we are at it */
  keys = hash_keys2(sl->hsentry, &nkeys);
  sl->signs = malloc(sizeof(struct sl_sign*) * nkeys);
  sl->nsigns = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->signs[i] = hash_find(sl->hsentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, sl->signs[i]->name);
      sl->signs[i]->sort = tp->s;
      if (!(sl->signs[i]->oid = hash_find(oids, sl->signs[i]->name)))
	mesg_verr(sl->signs[i]->inst->mloc, "OID needed for SIGN %s", sl->signs[i]->name);
    }
  /* Sort the signs */
  qsort(keys, nkeys, sizeof(char*), (cmp_fnc_t)signs_cmp);
  
  /* Provide forms with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hfentry, &nkeys);  
  sl->forms = malloc(sizeof(struct sl_form*) * nkeys);
  sl->nforms = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->forms[i] = hash_find(sl->hfentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->forms[i]->sort = tp->s;
      if (!(sl->forms[i]->oid = hash_find(oids, sl->forms[i]->name)))
	{
	  struct sl_inst *inst = list_first(sl->forms[i]->insts);
	  mesg_verr(inst->mloc, "OID needed for FORM %s", sl->forms[i]->name);
	}
    }

  /* Provide lists with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hlentry, &nkeys);  
  sl->lists = malloc(sizeof(struct sl_list*) * nkeys);
  sl->nlists = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->lists[i] = hash_find(sl->hlentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->lists[i]->sort = tp->s;
    }

  /* Provide values with sort codes based on token sort sequence */
  keys = hash_keys2(sl->hventry, &nkeys);  
  sl->values = malloc(sizeof(struct sl_value*) * nkeys);
  sl->nvalues = nkeys;
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_token *tp = NULL;
      sl->values[i] = hash_find(sl->hventry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, (ucp)keys[i]);
      sl->values[i]->sort = tp->s;
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
      if (sp->hlentry)
	{
	  int nslsts, j;
	  const char **slsts = hash_keys2(sp->hlentry, &nslsts);
	  sp->lists = memo_new_array(sl->m_insts, nslsts);
	  sp->nlists = nslsts;
	  for (j = 0; j < sp->nlists; ++j)
	    sp->lists[j] = hash_find(sp->hlentry, (uccp)slsts[j]);
	  qsort(sp->lists, sp->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
	}
      if (sp->hventry)
	{
	  int nsvals, j;
	  const char **svals = hash_keys2(sp->hventry, &nsvals);
	  sp->values = memo_new_array(sl->m_insts, nsvals);
	  sp->nvalues = nsvals;
	  for (j = 0; j < sp->nvalues; ++j)
	    sp->values[j] = hash_find(sp->hventry, (uccp)svals[j]);
	  qsort(sp->values, sp->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
	}
      if (sp->hfentry)
	{
	  int nsfrms, j;
	  const char **sfrms = hash_keys2(sp->hfentry, &nsfrms);
	  sp->forms = memo_new_array(sl->m_insts, nsfrms);
	  sp->nforms = nsfrms;
	  for (j = 0; j < sp->nforms; ++j)
	    {
	      struct sl_inst *fp;
	      fp = sp->forms[j] = hash_find(sp->hfentry, (uccp)sfrms[j]);
	      if (fp->lv && fp->lv->hlentry)
		{
		  int nslsts, j;
		  const char **slsts = hash_keys2(fp->lv->hlentry, &nslsts);
		  fp->lv->lists = memo_new_array(sl->m_insts, nslsts);
		  fp->lv->nlists = nslsts;
		  for (j = 0; j < fp->lv->nlists; ++j)
		    fp->lv->lists[j] = hash_find(fp->lv->hlentry, (uccp)slsts[j]);
		  qsort(fp->lv->lists, fp->lv->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
		}
	      if (fp->lv && fp->lv->hventry)
		{
		  int nsvals, j;
		  const char **svals = hash_keys2(fp->lv->hventry, &nsvals);
		  fp->lv->values = memo_new_array(sl->m_insts, nsvals);
		  fp->lv->nvalues = nsvals;
		  for (j = 0; j < fp->lv->nvalues; ++j)
		    fp->lv->values[j] = hash_find(fp->lv->hventry, (uccp)svals[j]);
		  qsort(fp->lv->values, fp->lv->nvalues, sizeof(void*), (cmp_fnc_t)values_inst_cmp);
		}
	    }
	  qsort(sp->forms, sp->nforms, sizeof(void*), (cmp_fnc_t)forms_inst_cmp);
	}
    }

  sx_homophones(sl);
  /*sx_qualified(sl);*/

  /*collate_term();*/
}

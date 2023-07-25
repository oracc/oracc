#include <hash.h>
#include <collate.h>
#include <signlist.h>
#include <sx.h>

extern int sortcode_output;
extern Hash * oid_load(const char *domain);
Hash *oids;
Hash *oid_sort_keys;

static int oid_char_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  int a1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)cc1);
  int b1 = (uintptr_t)hash_find(oid_sort_keys, (uccp)cc2);
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static const char **
sx_oid_array(struct sl_sign *s, List *o)
{
  const char **oids = NULL;
  int noids = 0;
  if (s)
    ++noids;
  if (o)
    noids += list_len(o);
  if (noids == 0)
    return NULL;
  else if (noids == 1)
    {
      oids = malloc(2*sizeof(const char *));
      if (s)
	oids[0] = s->oid;
      else
	{
	  struct sl_inst *ip = list_first(o);
	  oids[0] = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
	  oids[1] = NULL;
	}
    }
  else
    {
      Hash *seen = hash_create(16);
      struct sl_inst *ip = NULL;
      int i = 0;
      oids = calloc(noids+1, sizeof(const char *));
      if (s)
	{
	  hash_add(seen, (uccp)s->oid, "");
	  oids[i++] = s->oid;
	}
      for (ip = list_first(o); ip; ip = list_next(o))
	{
	  const char *oid = (ip->type == 's' ? ip->u.s->oid : ip->u.f->oid);
	  if (oid)
	    {
	      if (!hash_find(seen, (uccp)oid))
		{
		  hash_add(seen, (uccp)oid, "");
		  oids[i++] = oid;
		}
	    }
	  else
	    fprintf(stderr, "strange ... no OID on sign or form instance\n");
	}
      noids = i;
      oids[noids] = NULL;
      qsort(oids, noids, sizeof(const char *), (cmp_fnc_t)oid_char_cmp);
      hash_free(seen, NULL);
    }
  return oids;
}

static struct sl_sign *
form_as_sign(struct sl_signlist *sl, struct sl_form *f)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  struct sl_inst *ip = list_first(f->insts);

#if 0
  fprintf(stderr, "form_as_sign: %s; file=%s; line=%d\n", f->name, ip->mloc.file, ip->mloc.line);
#endif
  
  s->inst = ip;
  s->name = f->name;
  s->xref = f;  
  hash_add(sl->hsentry, s->name, s);
  asl_register_sign(&ip->mloc, sl, s);
  return s;
}

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

/* fowners is a list of inst where the insts can be either sign or form insts */
static int fowners_cmp(const void *a, const void *b)
{
  struct sl_inst *ai = (*(struct sl_inst**)a);
  struct sl_inst *bi = (*(struct sl_inst**)b);
  int a1 = ('f'==ai->type) ? ai->u.f->sort : ai->u.s->sort;
  int b1 = ('f'==ai->type) ? bi->u.f->sort : bi->u.s->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

/* form->owners is a list of sl_sign* */
static int owners_cmp(const void *a, const void *b)
{
  struct sl_sign *ai = (*(struct sl_sign**)a);
  struct sl_sign *bi = (*(struct sl_sign**)b);
  int a1 = ai->sort;
  int b1 = bi->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

static int lists_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_list**)a)->sort;
  int b1 = (*(struct sl_list**)b)->sort;  
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

static int values_cmp(const void *a, const void *b)
{
  int a1 = (*(struct sl_value**)a)->sort;
  int b1 = (*(struct sl_value**)b)->sort;  
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
  const char**keys = NULL;
  int nkeys;

  const char**lets = NULL;
  int nlets = 0, i;
  collate_init((ucp)"unicode");

  oids = oid_load("sl");
  if (!oids)
    oids = hash_create(1);
  oid_sort_keys = hash_create(2048);
  
  /* Register forms that don't exist as signs as if they were signs */
  keys = hash_keys2(sl->hfentry, &nkeys);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_sign *s;
      struct sl_form *f = hash_find(sl->hfentry, (uccp)keys[i]);
      
      if (!(s = hash_find(sl->hsentry, (uccp)keys[i])))
	s = form_as_sign(sl, hash_find(sl->hfentry, (uccp)keys[i]));
      f->gdl = s->gdl;
      f->sign = s;
    }

  /* First compounds phase: gather the data for digesting later */
  sx_compounds(sl);
  
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
      int j;
      sl->signs[i] = hash_find(sl->hsentry, (ucp)keys[i]);
      tp = hash_find(sl->htoken, sl->signs[i]->name);
      sl->signs[i]->sort = tp->s;
      if (!(sl->signs[i]->oid = hash_find(oids, sl->signs[i]->name)))
	mesg_verr(&sl->signs[i]->inst->mloc, "OID needed for SIGN %s", sl->signs[i]->name);
      else
	{
	  if (sortcode_output)
	    fprintf(stderr, "SIGN\t%s\t%d\n", sl->signs[i]->oid, sl->signs[i]->sort);
	  hash_add(oid_sort_keys, (uccp)sl->signs[i]->oid, (void*)(uintptr_t)sl->signs[i]->sort);
	}

      if (sl->signs[i]->hventry)
	{
	  const char **vkeys = hash_keys2(sl->signs[i]->hventry, &sl->signs[i]->nvalues);
	  sl->signs[i]->values = memo_new_array(sl->m_insts_p, sl->signs[i]->nvalues);
	  for (j = 0; j < sl->signs[i]->nvalues; ++j)
	    sl->signs[i]->values[j] = hash_find(sl->signs[i]->hventry, (uccp)vkeys[j]);
	}
    }
  /* Sort the signs */
  qsort(sl->signs, sl->nsigns, sizeof(struct sl_sign*), (cmp_fnc_t)signs_cmp);
  
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
	  mesg_verr(&inst->mloc, "OID needed for FORM %s", sl->forms[i]->name);
	}
      else
	{
	  if (!hash_find(oid_sort_keys, (uccp)sl->forms[i]->oid))
	    {
	      if (sortcode_output)
		fprintf(stderr, "FORM\t%s\t%d\n", sl->forms[i]->oid, sl->forms[i]->sort);
	      hash_add(oid_sort_keys, (uccp)sl->forms[i]->oid, (void*)(uintptr_t)sl->forms[i]->sort);
	    }
	}
      if (list_len(sl->forms[i]->insts))
	{
	  struct sl_inst *ip;
	  for (ip = list_first(sl->forms[i]->insts); ip; list_next(sl->forms[i]->insts))
	    {
	      if (ip->lv && ip->lv->hventry)
		{
		  const char **lvv;
		  int nlvv, k;
		  lvv = hash_keys2(ip->lv->hventry, &nlvv);
		  ip->lv->values = memo_new_array(sl->m_insts_p, nlvv);
		  ip->lv->nvalues = nlvv;
		  for (k = 0; k < nlvv; ++k)
		    ip->lv->values[k] = hash_find(ip->lv->hventry, (uccp)lvv[k]);
		  qsort(ip->lv->values, nlvv, sizeof(struct sl_inst*), fowners_cmp);
		}
	    }
	}
    }
  /* Sort the forms */
  qsort(sl->forms, sl->nforms, sizeof(struct sl_form*), (cmp_fnc_t)forms_cmp);

  /* Sort form owners (sl_sign*) if there are any */
  for (i = 0; i < sl->nforms; ++i)
    {
      if (sl->forms[i]->owners)
	{
	  sl->forms[i]->nowners = list_len(sl->forms[i]->owners);
	  if (sl->forms[i]->nowners == 1)
	    {
	      sl->forms[i]->owners_sort = memo_new(sl->m_signs_p);
	      sl->forms[i]->owners_sort[0] = list_first(sl->forms[i]->owners);
	    }
	  else
	    {
	      struct sl_sign *sp;
	      int j = 0;
	      sl->forms[i]->owners_sort = memo_new_array(sl->m_signs_p, sl->forms[i]->nowners);
	      for (sp = list_first(sl->forms[i]->owners); sp; sp = list_next(sl->forms[i]->owners))
		sl->forms[i]->owners_sort[j++] = sp;
	      qsort(sl->forms[i]->owners_sort, sl->forms[i]->nowners, sizeof(struct sl_sign *), (cmp_fnc_t)owners_cmp);
	    }
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
  /* Sort the lists */
  qsort(sl->lists, sl->nlists, sizeof(struct sl_list*), (cmp_fnc_t)lists_cmp);

  /* Create oid arrays for the list entries */
  for (i = 0; i < sl->nlists; ++i)
    sl->lists[i]->oids = sx_oid_array(NULL, sl->lists[i]->insts);

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
      /* Sort fowners if there are any */
      if (sl->values[i]->fowners)
	{
	  sl->values[i]->nfowners = list_len(sl->values[i]->fowners);
	  if (sl->values[i]->nfowners == 1)
	    {
	      sl->values[i]->fowners_i_sort = memo_new(sl->m_insts_p);
	      sl->values[i]->fowners_i_sort[0] = list_first(sl->values[i]->fowners);
	    }
	  else
	    {
	      struct sl_inst *ip;
	      int j = 0;
	      sl->values[i]->fowners_i_sort = memo_new_array(sl->m_insts_p, sl->values[i]->nfowners);
	      for (ip = list_first(sl->values[i]->fowners); ip; ip = list_next(sl->values[i]->fowners))
		sl->values[i]->fowners_i_sort[j++] = ip;
	      qsort(sl->values[i]->fowners_i_sort, sl->values[i]->nfowners, sizeof(struct sl_inst *), (cmp_fnc_t)fowners_cmp);
	    }
	}

    }
  /* Sort the values */
  qsort(sl->values, sl->nvalues, sizeof(struct sl_value*), (cmp_fnc_t)values_cmp);

  /* Create oid arrays for the value entries */
  for (i = 0; i < sl->nvalues; ++i)
    sl->values[i]->oids = sx_oid_array(sl->values[i]->sowner, sl->values[i]->fowners);

  /* Second phase of compounds: invert Hashes of sl_compound
     structures into arrays of OIDs for each category */
  sx_compound_digests(sl);
  
  /* Dereference structures created in asl_bld.c--see that file for AB1/AB2/AB3 creation */
  lets = hash_keys2(sl->hletters, &nlets); /* obtain list of letters from AB1 */
  qsort(lets, nlets, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
  sl->letters = memo_new_array(sl->m_letters, nlets);
  sl->nletters = nlets;
  for (i = 0; i < nlets; ++i)
    {
      const char **grps = NULL;
      int ngrps = 0, j;
      struct sl_letter *letterp = hash_find(sl->hletters, (uccp)lets[i]);
      sl->letters[i] = *letterp;
      grps = hash_keys2(sl->letters[i].hgroups, &ngrps); /* obtain list of groups in letter from AB2 */
      qsort(grps, ngrps, sizeof(const char*), (cmp_fnc_t)collate_cmp_graphemes);
      sl->letters[i].groups = memo_new_array(sl->m_groups, ngrps);
      sl->letters[i].ngroups = ngrps;

      for (j = 0; j < ngrps; ++j)
	{
	  List *slist = hash_find(sl->letters[i].hgroups, (ucp)grps[j]); /* obtain list of signs in group from AB3 */
	  sl->letters[i].groups[j].name = (ucp)grps[j];
	  sl->letters[i].groups[j].nsigns = list_len(slist);
	  sl->letters[i].groups[j].signs = memo_new_array(sl->m_signs_p,
							  sl->letters[i].groups[j].nsigns);
	  sl->letters[i].groups[j].signs = (struct sl_sign **)list2array(slist);
	  qsort(sl->letters[i].groups[j].signs,
		sl->letters[i].groups[j].nsigns, sizeof(void*), (cmp_fnc_t)signs_cmp);
	}
    }

  sx_inherited(sl);
  
  /* Create list of globally known values for each OID; need to do this after sx_inherited */
  sx_values_by_oid(sl);
  
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
		  int nslsts, m;
		  const char **slsts = hash_keys2(fp->lv->hlentry, &nslsts);
		  fp->lv->lists = memo_new_array(sl->m_insts, nslsts);
		  fp->lv->nlists = nslsts;
		  for (m = 0; m < fp->lv->nlists; ++m)
		    fp->lv->lists[m] = hash_find(fp->lv->hlentry, (uccp)slsts[m]);
		  qsort(fp->lv->lists, fp->lv->nlists, sizeof(void*), (cmp_fnc_t)lists_inst_cmp);
		}
	      if (fp->lv && fp->lv->hventry)
		{
		  int nsvals, k, l;
		  const char **svals = hash_keys2(fp->lv->hventry, &nsvals);
		  fp->lv->values = memo_new_array(sl->m_insts, nsvals);
		  fp->lv->nvalues = nsvals;
		  for (k = l = 0; k < fp->lv->nvalues; ++k)
		    if (svals[k])
		      {
			if (!(fp->lv->values[l++] = hash_find(fp->lv->hventry, (uccp)svals[k])))
			  --l;
		      }
		    else
		      fprintf(stderr, "skipping NULL entry in %s::%s hventry\n",
			      sp->forms[j]->u.f->sign->name, sp->forms[j]->u.f->name);
		  fp->lv->nvalues = l;
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

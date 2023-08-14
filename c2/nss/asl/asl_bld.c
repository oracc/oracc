#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>
#include <gdl.h>
#include <asl.tab.h>
#include "signlist.h"

static void check_flags(char *n, int *q, int *l);

struct sl_signlist *curr_asl = NULL;

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = calloc(1, sizeof(struct sl_signlist));
  sl->htoken = hash_create(4192);
  sl->listdefs = hash_create(3);
  sl->hsentry = hash_create(2048);
  sl->hfentry = hash_create(1024);
  sl->hventry = hash_create(2048);
  sl->hlentry = hash_create(1024);
  sl->hsignvvalid = hash_create(1024);
  sl->hletters = hash_create(32);
  sl->m_tokens = memo_init(sizeof(struct sl_token), 1024);
  sl->m_letters = memo_init(sizeof(struct sl_letter), 32);
  sl->m_groups = memo_init(sizeof(struct sl_letter), 128);
  sl->m_signs = memo_init(sizeof(struct sl_sign),512);
  sl->m_signs_p = memo_init(sizeof(struct sl_sign*),512);
  sl->m_forms = memo_init(sizeof(struct sl_form),512);
  sl->m_lists = memo_init(sizeof(struct sl_value),256);
  sl->m_values = memo_init(sizeof(struct sl_value),1024);
  sl->m_insts = memo_init(sizeof(struct sl_inst),1024);
  sl->m_insts_p = memo_init(sizeof(struct sl_inst*),512);
  sl->m_lv_data = memo_init(sizeof(struct sl_lv_data),512);
  sl->m_split_v = memo_init(sizeof(struct sl_split_value),512);
  sl->m_compounds = memo_init(sizeof(struct sl_compound), 512);
  sl->m_digests = memo_init(sizeof(struct sl_compound_digest), 512);
  sl->m_parents = memo_init(sizeof(struct sl_parents), 1024);
  sl->p = pool_init();
  sl->compounds = list_create(LIST_SINGLE);

  sl->notes = memo_new(sl->m_insts);
  sl->notes->type = 'S';
  sl->notes->u.S = sl;

  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->htoken, NULL);
      hash_free(sl->listdefs, free);
      hash_free(sl->hsentry, NULL);
      hash_free(sl->hfentry, NULL);
      hash_free(sl->hventry, NULL);
      hash_free(sl->hlentry, NULL);
      hash_free(sl->hsignvvalid, NULL);
      hash_free(sl->hletters, NULL);
      memo_term(sl->m_tokens);
      memo_term(sl->m_letters);
      memo_term(sl->m_groups);
      memo_term(sl->m_signs);
      memo_term(sl->m_signs_p);
      memo_term(sl->m_forms);
      memo_term(sl->m_lists);
      memo_term(sl->m_values);
      memo_term(sl->m_insts);
      memo_term(sl->m_insts_p);
      memo_term(sl->m_lv_data);
      memo_term(sl->m_split_v);
      memo_term(sl->m_compounds);
      memo_term(sl->m_digests);
      memo_term(sl->m_parents);
      pool_term(sl->p);
      free(sl);
    }
}

static void
asl_bld_list_string(const unsigned char *t, List **lp)
{
  if (!*lp)
    *lp = list_create(LIST_SINGLE);
  list_add(*lp, (void*)t);
}

static void
asl_bld_singleton_string(Mloc *locp, const unsigned char *t, const char *tag, unsigned char const* *dest, Boolean *seen)
{
  if (*seen)
    mesg_verr(locp, "tag @%s can only be used once in a @sign or @form", tag);
  else
    {
      *dest = t;
      *seen = 1;
    }
}

static Tree *
asl_bld_gdl(Mloc *locp, unsigned char *s)
{
  Tree *tp = NULL;
  mesg_init();
  tp = gdlparse_string(locp, (char*)s);
  gdlparse_reset();
  return tp;
}

void
asl_bld_token(Mloc *locp, struct sl_signlist *sl, unsigned char *t, int literal)
{
  if (!hash_find(sl->htoken, t))
    {
      struct sl_token *tokp = memo_new(sl->m_tokens);
      Tree *tp;
      tokp->t = t;
      if (literal)
	tp = gdl_literal(locp, (char*)t);
      else
	tp = asl_bld_gdl(locp, t);
      tokp->gdl = tp->root;
      tokp->gdl->name = "g:w";
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "form", tokp->gdl->text);
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", "sux");
      tokp->gsh = gsort_prep(tp);
      hash_add(sl->htoken, t, tokp);
    }
}

/* This routine builds the signlist tree of letter/group/signs */
void
asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s)
{
  struct sl_letter *lp;
  struct sl_token *tokp;
  unsigned char *group;

  s->sl = sl;
  
  if ('|' == *s->name)
    list_add(sl->compounds, (void*)s->inst);

#if 0
  tp = asl_bld_gdl(locp, (char*)pool_copy(s->name,sl->p));
  s->gdl = tp->root;
#endif

  /* get the group sign */
  tokp = hash_find(sl->htoken, s->name);
  if ((group = pool_copy(gdl_first_s(tokp->gdl), sl->p)))
    {
      /* get the letter from the group sign */
      unsigned char *letter = NULL;
      List *gslist; /* group signs */
      int code = -1;
#if 0
      unsigned char *group_orig = group;
#endif
      char *at = NULL;
      unsigned char *slist = (ucp)sll_is_signlist((ccp)group);
      
      if ((at = strchr((ccp)group, '@')))
	*at = '\0';
      else if ((at = strchr((ccp)group, '/')))
	*at = '\0';
      else if ('O' == *group)
	group[1] = '\0';
      
      group = pool_copy(g_base_of_preserve_case(group), sl->p);
 
      if (isdigit(*group))
	{
	  letter = pool_alloc(2, sl->p);
	  letter[0] = '0';
	  letter[1] = '\0';
	  code = 0; /* generates a letter code 'l0000' */
	  if (!slist)
	    {
	      if (group[1] && isdigit(group[1]))
		group[2] = '\0';
	      else
		group[1] = '\0';
	    }
	}
      else if (*group < 128)
	{
	  letter = pool_alloc(2, sl->p);
	  letter[0] = *group;
	  letter[1] = '\0';
	  code = (int)letter[0];
	  if (group[1] && group[1] < 128)
	    group[2] = '\0';
	  else
	    {
	      wchar_t *w = NULL;
	      unsigned char *c = NULL;
	      size_t len = 0;
	      w = utf2wcs(group, &len);
	      if (len > 1)
		{
		  w[2] = L'\0';
		  c = wcs2utf(w, 2);
		  group = pool_copy(c,sl->p);
		}
	      else
		group = letter;
	    }
	}
      else
	{
	  wchar_t *w = NULL;
	  unsigned char *c = NULL;
	  size_t len = 0;
	  w = utf2wcs(group, &len);
	  code = (int)w[0];
	  if (len == 1)
	    {
	      c = wcs2utf(w, 1);
	      group = letter = pool_copy(c,sl->p);
	    }
	  else
	    {
	      wchar_t z = w[1];
	      w[1] = L'\0';
	      c = wcs2utf(w, 1);
	      letter = pool_copy(c,sl->p);
	      w[1] = z;
	      w[2] = L'\0';
	      c = wcs2utf(w, 2);
	      group = pool_copy(c,sl->p);
	    }	  
	}

      if (slist)
	group = slist;
      else
	{
	  if (isdigit(group[0]))
	    group = (ucp)"0";
	  else if (!strchr("AEIU", group[0]) && 'N' == group[1])
	    {
	      group = (ucp)"NN";
	      letter = (ucp)"N";
	    }
	}

#if 0
      fprintf(stderr, "[asl] letter %s group %s -> %s\n", letter, group_orig, group);
#endif

      /* This is where the structure of the signlist is built */

      /* remember the letter */	  
      if (!(lp = hash_find(sl->hletters, letter)))/* AB1: hash of letters in signlist;
						     value is struct sl_letter* */
	{
	  lp = memo_new(sl->m_letters);
	  lp->name = letter;
	  lp->code = code;
	  lp->hgroups = hash_create(32);
	  hash_add(sl->hletters, letter, lp);
	}
      
      /* remember the group belongs to the letter */
      if (!(gslist = hash_find(lp->hgroups, group)))	      
	hash_add(lp->hgroups, group,
		 (gslist = list_create(LIST_SINGLE)));  /* AB2: hash of groups in letter;
							   value is list of struct sl_sign * */
      list_add(gslist, s->inst); 			/* AB3: list of signs in group,
					   		   data member is struct sl_inst* */ /* WHAT ABOUT @comp ? */
    }
  else
    mesg_verr(locp, "no sign name found in GDL of %s", s->name);
}

void
asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list,
	     const unsigned char *ref, int minus_flag)
{
  int literal, query;
  
  sl->curr_value = NULL;
  check_flags((char*)n, &query, &literal);

  asl_bld_token(locp, sl, (ucp)n, 0);

  if (sl->curr_sign->hfentry && hash_find(sl->curr_sign->hfentry, n))
    {
      mesg_verr(locp, "duplicate form %s in sign %s\n", n, sl->curr_sign->name);
    }
  else
    {
      struct sl_form *f = NULL;
      struct sl_inst *i = memo_new(sl->m_insts);

      if (!(f = hash_find(sl->hfentry, n)))
	{
	  f = memo_new(sl->m_forms);
	  f->name = n;
	  f->name_is_listnum = list;
	  f->insts = list_create(LIST_SINGLE);
	  list_add(f->insts, i);
	  f->owners = list_create(LIST_SINGLE);
	  list_add(f->owners, sl->curr_sign); 		/* list of signs that have this n as a form */	  
	  hash_add(sl->hfentry, (uccp)f->name, f); 	/* The forms that belong to the signlist are sl_form* */
	}
      else
	{
	  list_add(f->owners, sl->curr_sign); 		/* list of signs that have this n as a form */	  
	  list_add(f->insts, i);
	}

      sl->curr_form = sl->curr_inst = i;

      i->parent_s = sl->curr_sign->inst;
      i->type = 'f';
      i->u.f = f;
      i->ref = ref;
      i->mloc = *locp;
      i->valid = (Boolean)!minus_flag;
      i->query = (Boolean)query;
      i->literal = literal;
      i->lv = memo_new(sl->m_lv_data);
      
      if (!sl->curr_sign->hfentry)
	sl->curr_sign->hfentry = hash_create(128);

      hash_add(sl->curr_sign->hfentry, f->name, i); /* The forms that belong to signs are sl_inst* */
    }
}

static void
asl_add_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int lit, int q, int m)
{
  struct sl_list *l = NULL;
  struct sl_inst *i = memo_new(sl->m_insts);
  Hash *h = NULL;
  
  i->mloc = *locp;
  i->type = 'l';
  i->valid = (Boolean)!m;
  i->literal = (Boolean)lit;
  i->query = (Boolean)q;
  /*sl->curr_inst = i;*/

  /* If this list is already in the lists hash for the sign or the form-instance it's an error */
  if (sl->curr_form)
    {
      if (!sl->curr_form->lv->hlentry)
	h = sl->curr_form->lv->hlentry = hash_create(1);
    }
  else
    h = sl->curr_sign->hlentry;

  /* After setting h to an hlentry hash we can check if we've seen a
     valid @list with this name in the appropriate context */
  if (h && (l = hash_find(h, n)))
    {
      const char *f_or_s = (sl->curr_form ? "form" : "sign");
      const char *f_or_s_name = (sl->curr_form ? (ccp)sl->curr_form->u.f->name : (ccp)sl->curr_sign->name);
      mesg_verr(locp, "duplicate @list %s within @%s %s\n", n, f_or_s, f_or_s_name);
    }
  else
    {
      /* Now check the signlist's registry of @lists and add the new
	 sl_list* or add the inst to the existing sl_list* */
      struct sl_inst *parent_inst = (sl->curr_form ? sl->curr_form : sl->curr_sign->inst);
      if ((l = hash_find(sl->hlentry, n)))
        list_add(l->insts, parent_inst);
      else
	{
	  l = memo_new(sl->m_lists);
	  l->name = n;
	  l->insts = list_create(LIST_SINGLE);
	  list_add(l->insts, parent_inst);
	  hash_add(sl->hlentry, n, l);
	}
    }

  /* Check that the @list is valid against the signlists's listdefs
     and register it as seen */
  char name[32];
  if (strlen((ccp)n) < 32)
    {
      strcpy(name, (ccp)n);
      char *end = strpbrk(name, "0123456789");
      if (end)
	{
	  struct sl_listdef *ldp = NULL;
	  *end = '\0';	  
	  if ((ldp = hash_find(sl->listdefs, (uccp)name)))
	    {
	      if (!(hash_find(ldp->seen, l->name)))
		hash_add(ldp->seen, l->name, l);
	    }
	  else
	    mesg_verr(locp, "@list %s has unknown list-name part %s", n, name);
	}
      else
	{
	  mesg_verr(locp, "@list %s has no digits", n);
	  return;
	}
    }
  else
    {
      mesg_verr(locp, "@list entry too long (max 31 characters)");
      return;
    }
  
  i->u.l = l;

  if (sl->curr_form)
    {
      if (!sl->curr_form->lv->hlentry)
	sl->curr_form->lv->hlentry = hash_create(1);
      hash_add(sl->curr_form->lv->hlentry, l->name, i);
      list_add(l->insts, sl->curr_form);
    }
  else
    {
      if (!sl->curr_sign->hlentry)
	sl->curr_sign->hlentry = hash_create(1);
      hash_add(sl->curr_sign->hlentry, l->name, i);
      list_add(l->insts, sl->curr_sign->inst);
    }
}

void
asl_bld_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  int literal, query = 0;

  check_flags((char*)n, &query, &literal);
  
  asl_bld_token(locp, sl, (ucp)n, 1);

  if (sl->curr_form)
    asl_add_list(locp, sl, n, literal, query, minus_flag);
  else
    asl_add_list(locp, sl, n, literal, query, minus_flag);
}

/* m for meta */

void
asl_bld_inote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (sl->curr_inst)
    asl_bld_list_string(t, &sl->curr_inst->n.inotes);
  else
    mesg_verr(locp, "misplaced @inote");
}

void
asl_bld_lit(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (sl->curr_inst)
    asl_bld_list_string(t, &sl->curr_inst->n.lit);
  else
    mesg_verr(locp, "misplaced @inote");
}

void
asl_bld_note(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (sl->curr_inst)
    asl_bld_list_string(t, &sl->curr_inst->n.notes);
  else
    mesg_verr(locp, "misplaced @inote");
}

void
asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags((char*)t, &query, &literal);
  if (literal)
    mesg_verr(locp, "'*' is ignored on @aka");
  if (query)
    mesg_verr(locp, "'?' is ignored on @aka");
    
  asl_bld_token(locp, sl, (ucp)t, 0);

  if (sl->curr_form)
    {
      if (!sl->curr_form->u.f->aka)
	sl->curr_form->u.f->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_form->u.f->aka, (void*)t);
    }
  else if (sl->curr_sign)
    {
      if (!sl->curr_sign->aka)
	sl->curr_sign->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_sign->aka, (void*)t);
    }
  else
    mesg_verr(locp, "misplaced @aka");
}

void
asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags((char*)t, &query, &literal);

  if (literal)
    mesg_verr(locp, "'*' is ignored on @aka");
  if (query)
    mesg_verr(locp, "'?' is ignored on @pname");
    
  asl_bld_token(locp, sl, (ucp)t, 0);

  if (sl->curr_form)
    {
      if (sl->curr_form->u.f->pname)
	mesg_verr(locp, "@pname can only be given once for each @form");
      else
	sl->curr_form->u.f->pname = pool_copy(t, sl->p);
    }
  else if (sl->curr_sign)
    {
      if (sl->curr_sign->pname)
	mesg_verr(locp, "@pname can only be given once for each @sign");
      else
	sl->curr_sign->pname = pool_copy(t, sl->p);
    }
  else
    mesg_verr(locp, "misplaced @pname");
}

/* Top-level entities other than sign set sl->curr_inst to host
   metadata but NULL-out sl->curr-sign because it's an error for them
   to have values */
void
asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, enum sx_tle type)
{
  asl_bld_sign(locp, sl, n, 0, 0);
  sl->curr_sign->type = type;
  sl->curr_inst = sl->curr_sign->inst;
  sl->curr_sign = NULL;
}

static void
asl_bld_sign_sub(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
		 int list, int minus_flag, enum sx_tle type)
{
  int literal = 0, query = 0;
  struct sl_sign *s;

  if (!sl)
    {
      fprintf(stderr, "asl: signlist must begin with @signlist [PROJECTNAME]\n");
      exit(1);
    }

  check_flags((char*)n, &query, &literal);
  asl_bld_token(locp, sl, (ucp)n, 0);

  sl->curr_form = NULL;
  sl->curr_value = NULL;

  if ((s = hash_find(sl->hsentry, n)))
    {
      if (s->inst->valid && minus_flag)
	mesg_verr(locp, "@sign- %s duplicates @sign- %s; remove one", n, n);
      else if (!s->inst->valid || minus_flag)
	mesg_verr(locp, "@sign- %s duplicates valid @sign %s; remove one", n, n);
      else
	mesg_verr(locp, "duplicate @sign %s (first occurrence at line %d)\n", n, s->inst->mloc.line);
    }
  else
    {
      struct sl_inst *i = memo_new(sl->m_insts);
      s = memo_new(sl->m_signs);
      s->name = n;
      s->name_is_listnum = list;
      s->inst = i;
      i->type = 's';
      i->mloc = *locp;
      i->valid = (Boolean)!minus_flag;
      i->query = (Boolean)query;
      i->literal = (Boolean)literal;
      sl->curr_inst = i;
      i->u.s = s;
      hash_add(sl->hsentry, s->name, s);
      asl_register_sign(locp, sl, s);
    }
  s->type = type;
  sl->curr_sign = s;
}

void
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list, int minus_flag)
{
  asl_bld_sign_sub(locp, sl, n, list, minus_flag, sx_tle_sign);
}

void
asl_bld_end_sign(Mloc *locp, struct sl_signlist *sl)
{
  if (sl->curr_sign)
    {
      sl->curr_sign = NULL;
      sl->curr_form = NULL;
      sl->curr_inst = NULL;
    }
  else
    mesg_verr(locp, "misplaced @end sign, not in an @sign");    
}

void
asl_bld_signlist(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list)
{
  curr_asl = asl_bld_init();
  curr_asl->mloc = *locp;
  if (n)
    {
      while (isspace(*n))
	++n;
      curr_asl->project = (ccp)n;
    }
}

void
asl_bld_uchar(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  asl_bld_singleton_string(locp, t, "uchar",
			   sl->curr_form ? &sl->curr_form->u.f->U.uchar : &sl->curr_sign->U.uchar,
			   sl->curr_form ? &sl->curr_form->uchar : &sl->curr_sign->uchar);
}

void
asl_bld_ucode(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  asl_bld_singleton_string(locp, t, "ucode",
			   (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.ucode : &sl->curr_sign->U.ucode),
			   sl->curr_form ? &sl->curr_form->ucode : &sl->curr_sign->ucode);
}

void
asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  asl_bld_singleton_string(locp, t, "uname",
			   (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.uname : &sl->curr_sign->U.uname),
			   sl->curr_form ? &sl->curr_form->uname : &sl->curr_sign->uname);
}

void
asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  asl_bld_list_string(t, sl->curr_form ? &sl->curr_form->u.f->U.unotes : &sl->curr_sign->U.unotes);
}

void
asl_bld_uphase(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  asl_bld_singleton_string(locp, t, "uphase",
			   (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.uphase : &sl->curr_sign->U.uphase),
			   sl->curr_form ? &sl->curr_form->uphase : &sl->curr_sign->uphase);
}

void
asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
	      const char *lang, const unsigned char *ref, int atf_flag, int minus_flag)
{
  struct sl_value *v;
  struct sl_inst *i = NULL; 
  int literal = 0, xvalue = 0, uvalue = 0, query = 0;
  const unsigned char *base = NULL;

  check_flags((char*)n, &query, &literal);

  base = g_base_of(n);

  /* if the base of the new value duplicates a base in the current
     form/sign, it's an error; don't proceed with it, just warn and
     return */

  if (sl->curr_form)
    {
      unsigned const char *b = NULL;
      if (!minus_flag && sl->curr_form->lv && sl->curr_form->lv->hvbases && (b = hash_find(sl->curr_form->lv->hvbases, base)))
	{
	  mesg_verr(&sl->curr_inst->mloc, "form %s values %s and %s have the same base %s\n", sl->curr_form->u.f->name, b, n, base);
	  return;
	}
      else if (!minus_flag) /* only add valid values to hvbases */
	{
	  if (!sl->curr_form->lv->hvbases)
	    sl->curr_form->lv->hvbases = hash_create(1);
	  hash_add(sl->curr_form->lv->hvbases, pool_copy((uccp)base, sl->p), (void*)n);
	}
    }
  else
    {
      unsigned const char *b = NULL;
      if (!minus_flag && (b = hash_find(sl->curr_sign->hvbases, base)))
	{
	  mesg_verr(&sl->curr_inst->mloc, "sign %s values %s and %s have the same base %s\n", sl->curr_sign->name, b, n, base);
	  return;
	}
      else if (!minus_flag)
	{
	  if (!sl->curr_sign->hvbases)
	    sl->curr_sign->hvbases = hash_create(1);
	  hash_add(sl->curr_sign->hvbases, pool_copy((uccp)base, sl->p), (void*)n);
	}
    }

  asl_bld_token(locp, sl, (ucp)n, 0);
  
  if (strlen((ccp)n) > 3)
    {
      if (!strcmp((ccp)(n+(strlen((ccp)n)-3)), U_s_x_u8str))
	xvalue = 1;
      /*fprintf(stderr, "detected x-value %s\n", n);*/
    }
  else if (!strcmp((ccp)n, "x"))
    uvalue = 1;

  i = memo_new(sl->m_insts);
  i->type = 'v';
  i->mloc = *locp;
  i->ref = ref;
  i->valid = (Boolean)!minus_flag;
  i->query = (Boolean)query;
  if (sl->curr_form)
    i->parent_f = sl->curr_form;
  else
    i->parent_s = sl->curr_sign->inst;
  sl->curr_inst = i;
  
  /* If we are processing a sign and the v is already in
     signlist's sign-values it's an error for it to occur again */
  if ((v = hash_find(sl->hsignvvalid, n)))
    {
      if (!sl->curr_form && !xvalue && !v->atf && !minus_flag && sl->curr_sign->inst->valid)
	{
	  if (!strcmp((ccp)v->sowner->name, (ccp)sl->curr_sign->name))
	    mesg_verr(locp, "value %s occurs more than once in sign %s\n", n, sl->curr_sign->name);
	  else
	    mesg_verr(locp, "duplicate value %s in sign %s (first occurs in %s)\n", n, sl->curr_sign->name, v->sowner->name);
	  return;
	}
    }

  if (!v && !(v = hash_find(sl->hventry, n)))
    {
      v = memo_new(sl->m_values);
      hash_add(sl->hventry, n, v);
      v->name = n;
      v->atf = atf_flag;
      v->unknown = uvalue;
      if (lang)
	v->lang = lang;

      v->insts = list_create(LIST_SINGLE);
      list_add(v->insts, i);
      
      /* Add inst to sign or form's values */
      if (sl->curr_form)
	{
	  if (!v->fowners)
	    v->fowners = list_create(LIST_SINGLE);
	  list_add(v->fowners, sl->curr_form);
	  if (!sl->curr_form->lv->hventry)
	    sl->curr_form->lv->hventry = hash_create(1);
	  hash_add(sl->curr_form->lv->hventry, v->name, i);
	}
      else
	{
	  if (xvalue)
	    {
	      if (!v->fowners)
		v->fowners = list_create(LIST_SINGLE);
	      list_add(v->fowners, sl->curr_sign->inst);
	    }
	  else
	    v->sowner = sl->curr_sign;
	  if (!sl->curr_sign->hventry)
	    sl->curr_sign->hventry = hash_create(1);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	  if (sl->curr_sign->inst->valid && i->valid)
	    hash_add(sl->hsignvvalid, v->name, v);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	}
    }      
  else
    {
      if (sl->curr_form)
	{
	  if (!sl->curr_form->lv->hventry)
	    sl->curr_form->lv->hventry = hash_create(1);
	  if (hash_find(sl->curr_form->lv->hventry, n))
	    {
	      if (!xvalue)
		{
		  mesg_verr(locp, "duplicate value %s in form %s::%s", n,
			    sl->curr_sign->name, sl->curr_form->u.f->name);
		  return;
		}
	    }
	  else
	    {
	      hash_add(sl->curr_form->lv->hventry, v->name, i);
	      if (!v->fowners)
		v->fowners = list_create(LIST_SINGLE);
	      list_add(v->fowners, sl->curr_form);
	    }
	}
      else
	{
	  if (!sl->curr_sign->hventry)
	    sl->curr_sign->hventry = hash_create(1);
	  hash_add(sl->curr_sign->hventry, v->name, i);
	  if (xvalue)
	    list_add(v->fowners, sl->curr_sign->inst);
	}
      list_add(v->insts, i);
    }

  i->u.v = v;
  i->u.v->xvalue = xvalue;
  sl->curr_value = i;
}

/* The lexer rules only allow the flags in the order:
 *
 *   optional ?
 *   optional =
 */
static void
check_flags(char *n, int *q, int *l)
{
  char *last = (char*)(n + (strlen((ccp)n)-1));
  *l = *q = 0;
  if ('=' == *last)
    {
      *last-- = '\0';
      *l = 1;
    }
  if ('?' == *last)
    {
      *last = '\0';
      *q = 1;
    }
}

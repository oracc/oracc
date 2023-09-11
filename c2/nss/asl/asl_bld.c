#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>
#include <sll.h>
#include <gdl.h>
#include <oid.h>
#include <asl.tab.h>
#include "signlist.h"

extern int asl_literal_flag;

static void check_flags(Mloc* locp, char *n, int *q, int *l);

struct sl_signlist *curr_asl = NULL;

static Hash *oids;
static unsigned const char *asl_oid_lookup(unsigned const char *key)
{
  return hash_find(oids, key);
}

Hash *
asl_get_oids(void)
{
  return oids;
}

static int
asl_sign_guard(Mloc *locp, struct sl_signlist *sl, const char *tag)
{
  if (!sl->curr_sign)
    {
      mesg_verr(locp, "misplaced @%s--no @sign in effect", tag);
      return 0;
    }
  return 1;
}

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = calloc(1, sizeof(struct sl_signlist));
  sl->htoken = hash_create(4192);
  sl->listdefs = hash_create(7);
  sl->sysdefs = hash_create(3);
  sl->hsentry = hash_create(2048);
  sl->hfentry = hash_create(1024);
  sl->haka = hash_create(128);
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
  sl->m_notes = memo_init(sizeof(struct sl_note), 512);
  sl->m_memostr = memo_init(sizeof(Memo_str), 512);
  sl->m_syss = memo_init(sizeof(struct sl_sys), 512);
  sl->p = pool_init();
  sl->compounds = list_create(LIST_SINGLE);

  sl->notes = memo_new(sl->m_insts);
  sl->notes->type = 'S';
  sl->notes->u.S = sl;

  /* Preload the current set of SL oids */
  oids = oid_domain_hash(NULL, "oid", "sl");

  (void)gvl_setup(NULL, NULL);
  gvl_set_lookup_ptr(asl_oid_lookup);
  
  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->htoken, NULL);
      hash_free(sl->listdefs, free);
      hash_free(sl->sysdefs, free);
      hash_free(sl->hsentry, NULL);
      hash_free(sl->hfentry, NULL);
      hash_free(sl->haka, NULL);
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
      memo_term(sl->m_notes);
      memo_term(sl->m_memostr);
      memo_term(sl->m_syss);
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
      const char *gsig = NULL;
      extern int asl_literal_flag;
      tokp->t = t;
      if (literal || asl_literal_flag)
	tp = gdl_literal(locp, (char*)t);
      else
	{
	  extern int gdlsig_depth_mode;
	  tp = asl_bld_gdl(locp, t);
	  /* deep? or a special mode for unicode rendering? */
	  gdlsig_depth_mode = -1;
	  gsig = gdlsig(tp);
	}
      tokp->gdl = tp->root;
      tokp->gdl->name = "g:w";
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "form", tokp->gdl->text);
      gdl_prop_kv(tokp->gdl, GP_ATTRIBUTE, PG_GDL_INFO, "xml:lang", "sux");
      tokp->gsh = gsort_prep(tp);
      tokp->gsig = gsig;
      hash_add(sl->htoken, t, tokp);
    }
  asl_literal_flag = 0;
}

/* This routine builds the signlist tree of letter/group/signs */
void
asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s)
{
  struct sl_letter *lp;
  struct sl_token *tokp;
  unsigned char *group;

  s->sl = sl;
  
  if ('|' == *s->name && s->inst->valid)
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
asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  int literal, query;
  
  sl->curr_value = NULL;
  check_flags(locp, (char*)n, &query, &literal);

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
	  else if (!sll_signlist(name,strlen(name)))
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
  if (asl_sign_guard(locp, sl, "list"))
    {
      int literal, query = 0;

      check_flags(locp, (char*)n, &query, &literal);
  
      asl_bld_token(locp, sl, (ucp)n, 1);
  
      if (sl->curr_form)
	asl_add_list(locp, sl, n, literal, query, minus_flag);
      else
	asl_add_list(locp, sl, n, literal, query, minus_flag);

      /* U+ list entries are both lists and uhex; they are specialcased on
	 output and emitted only as @list U+ but within the Unicode
	 block */
      if ('U' == n[0] && '+' == n[1])
	asl_bld_uhex(locp, sl, n);
    }
}

void
asl_bld_sysdef(Mloc *locp, struct sl_signlist *sl, const char *name, const char *comment)
{
  if (!hash_find(sl->sysdefs, (uccp)name))
    {
      struct sl_sysdef *sdp = calloc(1, sizeof(struct sl_sysdef));
      sdp->name = pool_copy((uccp)name, sl->p);
      sdp->comment = (ccp)pool_copy((uccp)comment, sl->p);
      sdp->inst.type = 'y';
      sdp->inst.u.y = sdp;
      sl->curr_inst = &sdp->inst;
      hash_add(sl->sysdefs, sdp->name, sdp);
    }
  else
    {
      mesg_verr(locp, "repeated @sysdef %s\n", name);
    }
}

#if 0
static void
asl_check_sys(Mloc *locp, struct sl_signlist *sl, const char *txt)
{
  /* NAME VALUE (=> VALUES)? */
  char *c = strdup(txt), *s, *t;
  enum sysstate { sys_name, sys_value, sys_goesto, sys_values };
  enum sysstate currstate = sys_name;
  s = c;
  while ((t = strtok(s," \t\n")))
    {
      if (s)
	s = NULL;
      if (currstate == sys_name)
	{
	  if (!hash_find(sl->sysdefs, (uccp)t))
	    {
	      mesg_verr(locp, "undefined system name %s in @sys", t);
	      return;
	    }
	  ++currstate;
	}
      else if (currstate == sys_value || currstate == sys_values)
	{
	  if (!hash_find(sl->hventry, (uccp)t))
	    {
	      mesg_verr(locp, "undefined value %s in @sys", t);
	      return;
	    }
	  if (currstate == sys_value)
	    ++currstate;
	}
      else if (currstate == sys_goesto)
	{
	  if (strcmp(t, "=>"))
	    {
	      mesg_verr(locp, "unexpected token in @sys %s; expected '=>'", t);
	      return;
	    }
	  ++currstate;
	}
      else
	{
	  abort();
	}
    }
  
}
#endif

void
asl_bld_note(Mloc *locp, struct sl_signlist *sl, const char *tag, const char *txt)
{
  if (sl->curr_inst)
    {
      struct sl_note *n = memo_new(sl->m_notes);
      n->tag = tag; /* tag is a constant from asl.y */
      n->txt = txt;
      if (!sl->curr_inst->notes)
	sl->curr_inst->notes = list_create(LIST_SINGLE);
      list_add(sl->curr_inst->notes, n);
#if 0
      /* @sys is implemented as a type of note but has special
	 location and syntax rules which we check here rather than in
	 the lexer/parser */
      if (!strcmp(tag,"sys"))
	{
	  if (sl->curr_inst->type != 'v')
	    mesg_verr(locp, "misplaced @sys -- must occur in the notes block belonging to an @v");
	  else
	    asl_check_sys(locp, sl, txt);
	}
#endif
    }
  else
    (void)asl_sign_guard(locp, sl, tag);
}

void
asl_bld_aka(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  static int one = 1;
  Memo_str *m = NULL;

  check_flags(locp, (char*)t, &query, &literal);

  if (query)
    mesg_verr(locp, "'?' is ignored on @aka");

  asl_bld_token(locp, sl, (ucp)t, literal);

  if (sl->curr_form)
    {
      if (!sl->curr_form->u.f->aka)
	sl->curr_form->u.f->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_form->u.f->aka, (m = memo_str(locp, t)));
      if (literal)
	m->user = &one;
      else
	m->user = NULL;
      /* If @aka is on a @form check that the form doesn't have a
	 parent sign; if it does, the @aka should be up there */
      if (sl->curr_form->u.f->sign)
	{
	  if (!sl->curr_form->u.f->sign->xref)
	    {
	      mesg_verr(locp, "@aka %s should be on @sign %s, not @form", t, sl->curr_form->u.f->sign->name);
	    }
	  else
	    {
	      if (!hash_find(sl->haka, t))
		hash_add(sl->haka, t, sl->curr_sign->inst);
	      else
		mesg_verr(locp, "duplicate @aka %s", t);
	    }
	}
      else
	{
	  if (!hash_find(sl->haka, t))
	    hash_add(sl->haka, t, sl->curr_form);
	  else
	    mesg_verr(locp, "duplicate @aka %s", t);
	}
    }
  else if (sl->curr_sign)
    {
      if (!sl->curr_sign->aka)
	sl->curr_sign->aka = list_create(LIST_SINGLE);
      list_add(sl->curr_sign->aka, (m = memo_str(locp, t)));
      if (literal)
	m->user = &one;
      else
	m->user = NULL;
      if (!hash_find(sl->haka, t))
	hash_add(sl->haka, t, (void*)sl->curr_sign);
      else
	mesg_verr(locp, "duplicate @aka %s", t);
    }
  else
    (void)asl_sign_guard(locp, sl, "aka");
}

void
asl_bld_smap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags(locp, (char*)t, &query, &literal);
  if (literal)
    mesg_verr(locp, "'*' is ignored on @smap");
  if (query)
    mesg_verr(locp, "'?' is ignored on @smap");

  asl_bld_token(locp, sl, (ucp)t, 0);

  if (sl->curr_form)
    {
      mesg_verr(locp, "@smap %s should be on @sign %s, not @form", t, sl->curr_form->u.f->sign->name);
    }
  else if (sl->curr_sign)
    {
      if (!sl->curr_sign->smap)
	sl->curr_sign->smap = (ccp)t;
      else
	mesg_verr(locp, "duplicate @smap %s", t);
    }
  else
    (void)asl_sign_guard(locp, sl, "smap");
}

void
asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int literal, query;
  check_flags(locp, (char*)t, &query, &literal);

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
    (void)asl_sign_guard(locp, sl, "pname");
}

/* Top-level entities other than sign set sl->curr_inst to host
   metadata but NULL-out sl->curr-sign because it's an error for them
   to have values */
void
asl_bld_tle(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, const unsigned char *m, enum sx_tle type)
{
  asl_bld_sign(locp, sl, n, 0);
  sl->curr_sign->type = type;
  sl->curr_sign->pname = pool_copy(m, sl->p);
  sl->curr_inst = sl->curr_sign->inst;
  sl->curr_sign = NULL;  
}

static void
asl_bld_sign_sub(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
		 int minus_flag, enum sx_tle type)
{
  int literal = 0, query = 0;
  struct sl_sign *s;

  if (!sl)
    {
      fprintf(stderr, "asl: signlist must begin with @signlist [PROJECTNAME]\n");
      exit(1);
    }

  check_flags(locp, (char*)n, &query, &literal);
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
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int minus_flag)
{
  asl_bld_sign_sub(locp, sl, n, minus_flag, sx_tle_sign);
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
asl_bld_signlist(Mloc *locp, const unsigned char *n, int list)
{
  curr_asl = asl_bld_init();
  curr_asl->mloc = *locp;
  if (n)
    {
      while (isspace(*n))
	++n;
      curr_asl->project = (ccp)n;
    }
  curr_asl->curr_inst = curr_asl->notes = memo_new(curr_asl->m_insts);
}

void
asl_bld_sys(Mloc *locp, struct sl_signlist *sl, const char *sysname, unsigned const char *v, unsigned const char *vv)
{
  struct sl_inst *ip = NULL;
  if (sl->curr_form)
    ip = sl->curr_form;
  else if (sl->curr_sign)
    ip = sl->curr_sign->inst;
  else
    mesg_verr(locp, "misplaced @sys: must belong to @sign or @form");
  if (ip)
    {
      if (!hash_find(sl->sysdefs, (uccp)sysname))
	{
	  mesg_verr(locp, "undefined system name %s in @sys", sysname);
	  return;
	}
      /* FIXME: need to validate values as well, but probably best
	 done in sx_marshall when all values are known */
      if (!ip->sys)
	{
	  ip->sys = list_create(LIST_SINGLE);
	  if (!sl->syslists)
	    sl->syslists = list_create(LIST_SINGLE);
	  list_add(sl->syslists, ip->sys);
	}
      struct sl_sys *sp = memo_new(sl->m_syss);
      sp->name = sysname;
      sp->v = v;
      sp->vv = pool_copy(vv, sl->p);
      list_add(ip->sys, sp);
    }
}

void
asl_bld_uhex(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "uhex"))
    asl_bld_singleton_string(locp, t, "uhex",
			     sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.uhex : (uccp*)&sl->curr_sign->U.uhex,
			     sl->curr_form ? &sl->curr_form->uhex : &sl->curr_sign->inst->uhex);
}

void
asl_bld_upua(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "upua"))
    asl_bld_singleton_string(locp, t, "upua",
			     sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.upua : (uccp*)&sl->curr_sign->U.upua,
			     sl->curr_form ? &sl->curr_form->upua : &sl->curr_sign->inst->upua);
}

void
asl_bld_utf8(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "ucun"))
    asl_bld_singleton_string(locp, t, "ucun",
			     sl->curr_form ? &sl->curr_form->u.f->U.utf8 : &sl->curr_sign->U.utf8,
			     sl->curr_form ? &sl->curr_form->utf8 : &sl->curr_sign->inst->utf8);
}

void
asl_bld_umap(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "umap"))
    asl_bld_singleton_string(locp, t, "umap",
			     sl->curr_form ? (uccp*)&sl->curr_form->u.f->U.umap : (uccp*)&sl->curr_sign->U.umap,
			     sl->curr_form ? &sl->curr_form->umap : &sl->curr_sign->inst->umap);
}

void
asl_bld_useq(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "useq"))
    asl_bld_singleton_string(locp, t, "useq",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.useq : &sl->curr_sign->U.useq),
			     sl->curr_form ? &sl->curr_form->useq : &sl->curr_sign->inst->useq);
}

void
asl_bld_uname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "uname"))
    asl_bld_singleton_string(locp, t, "uname",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.uname : &sl->curr_sign->U.uname),
			     sl->curr_form ? &sl->curr_form->uname : &sl->curr_sign->inst->uname);
}

void
asl_bld_unote(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "unote"))
    asl_bld_list_string(t, sl->curr_form ? &sl->curr_form->u.f->U.unotes : &sl->curr_sign->U.unotes);
}

void
asl_bld_urev(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  if (asl_sign_guard(locp, sl, "urev"))
    asl_bld_singleton_string(locp, t, "urev",
			     (uccp*)(sl->curr_form ? &sl->curr_form->u.f->U.urev : &sl->curr_sign->U.urev),
			     sl->curr_form ? &sl->curr_form->urev : &sl->curr_sign->inst->urev);
}

void
asl_bld_value(Mloc *locp, struct sl_signlist *sl, const unsigned char *n,
	      const char *lang, int atf_flag, int minus_flag)
{
  struct sl_value *v;
  struct sl_inst *i = NULL; 
  int literal = 0, xvalue = 0, uvalue = 0, query = 0;
  const unsigned char *base = NULL;

  check_flags(locp, (char*)n, &query, &literal);

  base = g_base_of(n);

  /* if the base of the new value duplicates a base in the current
     form/sign, it's an error; don't proceed with it, just warn and
     return */

  if (sl->curr_form)
    {
      unsigned const char *b = NULL;
      if (!minus_flag && sl->curr_form->lv && sl->curr_form->lv->hvbases && (b = hash_find(sl->curr_form->lv->hvbases, base)))
	{
	  mesg_verr(locp, "form %s values %s and %s have the same base %s\n", sl->curr_form->u.f->name, b, n, base);
	  return;
	}
      else if (!minus_flag) /* only add valid values to hvbases */
	{
	  if (!sl->curr_form->lv->hvbases)
	    sl->curr_form->lv->hvbases = hash_create(1);
	  hash_add(sl->curr_form->lv->hvbases, pool_copy((uccp)base, sl->p), (void*)n);
	}
    }
  else if (sl->curr_sign)
    {
      unsigned const char *b = NULL;
      if (!minus_flag && (b = hash_find(sl->curr_sign->hvbases, base)))
	{
	  mesg_verr(locp, "sign %s values %s and %s have the same base %s\n", sl->curr_sign->name, b, n, base);
	  return;
	}
      else if (!minus_flag)
	{
	  if (!sl->curr_sign->hvbases)
	    sl->curr_sign->hvbases = hash_create(1);
	  hash_add(sl->curr_sign->hvbases, pool_copy((uccp)base, sl->p), (void*)n);
	}
    }
  else
    {
      (void)asl_sign_guard(locp, sl, "v");
      return;
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
  i->valid = (Boolean)!minus_flag;
  i->query = (Boolean)query;
  if (lang)
    i->lang = lang+1; /* skip the % */
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
	    mesg_verr(locp, "duplicate value %s in sign %s (first occurs in %s)\n",
		      n, sl->curr_sign->name, v->sowner->name);
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
check_flags(Mloc *locp, char *n, int *q, int *l)
{
  char *last = (char*)(n + (strlen((ccp)n)-1));
  *l = *q = 0;
  if ('=' == *last)
    {
      mesg_verr(locp, "literal strings are no longer marked with '='; enclose in \"...\" instead");
#if 0      
      *last-- = '\0';
      *l = 1;
#endif
    }
  if ('?' == *last)
    {
      *last = '\0';
      *q = 1;
    }
  if (asl_literal_flag)
    *l = 1;
}

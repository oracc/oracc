#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gdl.h>
#include <asl.tab.h>
#include "signlist.h"

static void check_query(char *n, int *q);

struct sl_signlist *curr_asl = NULL;

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = malloc(sizeof(struct sl_signlist));
  sl->htoken = hash_create(4192);
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
  sl->p = pool_init();
  sl->compounds = list_create(LIST_SINGLE);
  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->htoken, NULL);
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
      pool_term(sl->p);
      list_free(sl->compounds, NULL);
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

static void
asl_bld_token(struct sl_signlist *sl, const unsigned char *t)
{
  if (!hash_find(sl->htoken, t))
    {
      struct sl_token *tp = memo_new(sl->m_tokens);
      tp->t = t;
      hash_add(sl->htoken, t, tp);
    }
}

Tree *
asl_bld_gdl(Mloc *locp, char *s)
{
  Tree *tp = NULL;
  mesg_init();
  tp = gdlparse_string(locp, s);
  gdlparse_reset();
  return tp;
}

/* This routine builds the signlist tree of letter/group/signs */
void
asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s)
{
  Tree *tp;
  unsigned const char *group;

  if ('|' == *s->name)
    list_add(sl->compounds, (void*)s->name);

  tp = asl_bld_gdl(locp, (char*)pool_copy(s->name,sl->p));
  s->gdl = tp->root;
  /* get the group sign */
  if ((group = gdl_first_s(s->gdl)))
    {
      /* get the letter from the group sign */
      unsigned char *letter = NULL;
      Hash *lghash; /* letter groups */
      List *gslist; /* group signs */

      if (*group < 128)
	{
	  letter = pool_alloc(2, sl->p);
	  letter[0] = *group;
	  letter[1] = '\0';
	}
      else
	{
	  wchar_t *w = NULL;
	  unsigned char *c = NULL;
	  size_t len = 0;
	  w = utf2wcs(group, &len);
	  w[1] = L'\0';
	  c = wcs2utf(w, 1);
	  letter = pool_copy(c,sl->p);
	}

      /* This is where the structure of the signlist is built */
	  
      /* remember the letter */
      if (!(lghash = hash_find(sl->hletters, letter)))
	hash_add(sl->hletters, letter, (lghash = hash_create(128))); /* AB1: hash of letters in signlist;
									value is hash of groups in letter */
      /* remember the group belongs to the letter */
      if (!(gslist = hash_find(lghash, group)))	      
	hash_add(lghash, group, (gslist = list_create(LIST_SINGLE))); /* AB2: hash of groups in letter;
									 value is list of struct sl_sign * */
      list_add(gslist, s); /* AB3: list of signs in group, data member is struct sl_sign* */
    }
  else
    mesg_verr(locp, "no sign name found in GDL of %s", s->name);
}

void
asl_bld_form(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list,
	     const unsigned char *var, const unsigned char *ref, int minus_flag)
{
  int query;
  
  sl->curr_value = NULL;
  check_query((char*)n, &query);
  asl_bld_token(sl, n);

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
	  list_add(f->insts, i);
	}

      sl->curr_form = i;

      i->type = 'f';
      i->u.f = f;
      i->var = var;
      i->ref = ref;
      i->mloc = locp;
      i->valid = (Boolean)!minus_flag;
      i->query = (Boolean)query;
      sl->curr_inst = i;
      
      if (!sl->curr_sign->hfentry)
	sl->curr_sign->hfentry = hash_create(128);

      hash_add(sl->curr_sign->hfentry, f->name, i); /* The forms that belong to signs are sl_inst* */
    }
}

static void
asl_add_list(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int q, int m)
{
  struct sl_list *l = NULL;
  struct sl_inst *i = memo_new(sl->m_insts);
  Hash *h = NULL;
  
  i->mloc = locp;
  i->type = 'l';
  i->valid = (Boolean)!m;
  i->query = (Boolean)q;
  sl->curr_inst = i;

  /* If this list is already in the lists hash for the sign or the form-instance it's an error */
  if (sl->curr_form)
    {
      if (sl->curr_form->lv)
	h = sl->curr_form->lv->hlentry;
      else
	{
	  sl->curr_form->lv = memo_new(sl->m_lv_data);
	  if (!sl->curr_form->lv->hlentry)
	    h = sl->curr_form->lv->hlentry = hash_create(1);
	}      
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

  i->u.l = l;

  if (sl->curr_form)
    {
      if (!sl->curr_form->lv)
	sl->curr_form->lv = memo_new(sl->m_lv_data);
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
  int query = 0;

  check_query((char*)n, &query);
  asl_bld_token(sl, n);
 
  if (sl->curr_form)
    asl_add_list(locp, sl, n, query, minus_flag);
  else
    asl_add_list(locp, sl, n, query, minus_flag);
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
asl_bld_pname(Mloc *locp, struct sl_signlist *sl, const unsigned char *t)
{
  int query;
  check_query((char*)t, &query);
  if (query)
    mesg_verr(locp, "'?' is ignored on @pname");
    
  asl_bld_token(sl, t);

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

void
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list, int minus_flag)
{
  int query = 0;
  struct sl_sign *s;

  if (!sl)
    {
      fprintf(stderr, "asl: signlist must begin with @signlist [PROJECTNAME]\n");
      exit(1);
    }

  check_query((char*)n, &query);
  asl_bld_token(sl, n);

  sl->curr_form = NULL;
  sl->curr_value = NULL;

  if ((s = hash_find(sl->hsentry, n)))
    {
      if (s->inst->valid && minus_flag)
	mesg_verr(locp, "@sign- %s duplicates @sign- %s; remove one", n, n);
      else if (!s->inst->valid || minus_flag)
	mesg_verr(locp, "@sign- %s duplicates valid @sign %s; remove one", n, n);
      else
	mesg_verr(locp, "duplicate @sign %s\n", n);
    }
  else
    {
      struct sl_sign *s = memo_new(sl->m_signs);
      struct sl_inst *i = memo_new(sl->m_insts);
      s->name = n;
      s->name_is_listnum = list;
      s->inst = i;
      i->type = 's';
      i->mloc = locp;
      i->valid = (Boolean)!minus_flag;
      i->query = (Boolean)query;
      sl->curr_inst = i;
      i->u.s = s;
      sl->curr_sign = s;
      hash_add(sl->hsentry, s->name, s);
      asl_register_sign(locp, sl, s);
    }
}

struct sl_signlist *
asl_bld_signlist(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list)
{
  curr_asl = asl_bld_init();
  if (n)
    {
      while (isspace(*n))
	++n;
      curr_asl->project = (ccp)n;
    }
  return curr_asl;
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
  struct sl_inst *i = memo_new(sl->m_insts);
  int xvalue = 0, uvalue = 0, query = 0;

  check_query((char*)n, &query);
  asl_bld_token(sl, n);
  
  if (strlen((ccp)n) > 3)
    {
      if (!strcmp((ccp)(n+(strlen((ccp)n)-3)), U_s_x_u8str))
	xvalue = 1;
      /*fprintf(stderr, "detected x-value %s\n", n);*/
    }
  else if (!strcmp((ccp)n, "x"))
    uvalue = 1;
  i->type = 'v';
  i->mloc = locp;
  i->ref = ref;
  i->valid = (Boolean)!minus_flag;
  i->query = (Boolean)query;
  sl->curr_inst = i;
  
  if ((v = hash_find(sl->hsignvvalid, n)))
    {
      /* If we are processing a sign and the v is already in
	 signlist's sign-values it's an error for it to occur again */
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
      Tree *tp = NULL;
      v = memo_new(sl->m_values);
      hash_add(sl->hventry, n, v);
      v->name = n;
      v->atf = atf_flag;
      v->unknown = uvalue;
      if (!v->atf) /* should really parse ATF */
	{
	  tp = asl_bld_gdl(locp, (char*)pool_copy(v->name,sl->p));
	  v->gdl = tp->root;
	}
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
	  if (!sl->curr_form->lv)
	    sl->curr_form->lv = memo_new(sl->m_lv_data);
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
	  if (!sl->curr_form->lv)
	    sl->curr_form->lv = memo_new(sl->m_lv_data);
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

static void
check_query(char *n, int *q)
{
  char *last = (char*)(n + (strlen((ccp)n)-1));
  if ('?' == *last)
    {
      *last = '\0';
      *q = 1;
    }
  else
    *q = 0;
}

#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gdl.h>
#include <asl.tab.h>
#include "signlist.h"

struct sl_signlist *curr_asl = NULL;

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = malloc(sizeof(struct sl_signlist));
  sl->hsigns = hash_create(1024);
  sl->hforms = hash_create(1024);
  sl->hvalues = hash_create(1024);
  sl->hsignvalues = hash_create(1024);
  sl->hletters = hash_create(32);
  sl->m_letters = memo_init(sizeof(struct sl_letter), 32);
  sl->m_groups = memo_init(sizeof(struct sl_letter), 128);
  sl->m_signs = memo_init(sizeof(struct sl_sign),512);
  sl->m_forms = memo_init(sizeof(struct sl_form),512);
  sl->m_lists = memo_init(sizeof(struct sl_value),256);
  sl->m_values = memo_init(sizeof(struct sl_value),1024);
  sl->m_insts = memo_init(sizeof(struct sl_inst),1024);
  sl->m_signs_p = memo_init(sizeof(struct sl_sign *),512);
  sl->m_valdata = memo_init(sizeof(struct sl_value_data),512);
  sl->p = pool_init();
  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->hsigns, NULL);
      hash_free(sl->hforms, NULL);
      hash_free(sl->hvalues, NULL);
      hash_free(sl->hsignvalues, NULL);
      memo_term(sl->m_letters);
      memo_term(sl->m_groups);
      memo_term(sl->m_signs);
      memo_term(sl->m_forms);
      memo_term(sl->m_lists);
      memo_term(sl->m_values);
      memo_term(sl->m_insts);
      memo_term(sl->m_signs_p);
      memo_term(sl->m_valdata);
      free(sl);
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

void
asl_register_sign(Mloc *locp, struct sl_signlist *sl, struct sl_sign *s)
{
  Tree *tp;
  unsigned const char *group;
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
	     const unsigned char *var, const unsigned char *ref)
{
  sl->curr_value = NULL;

  if (sl->curr_sign->forms && hash_find(sl->curr_sign->hforms, n))
    {
      mesg_verr(locp, "duplicate form %s in sign %s\n", n, sl->curr_sign->name);
    }
  else
    {
      struct sl_form *f = NULL;
      struct sl_inst *i = memo_new(sl->m_insts);

      if (!(f = hash_find(sl->hforms, n)))
	{
	  f = memo_new(sl->m_forms);
	  f->name = n;
	  f->var = var;
	  f->name_is_listnum = list;
	  f->insts = list_create(LIST_SINGLE);
	  list_add(f->insts, i);
	  f->owners = list_create(LIST_SINGLE);
	  list_add(f->owners, sl->curr_sign); 		/* list of signs that have this n as a form */	  
	  hash_add(sl->hforms, (uccp)f->name, f); 	/* The forms that belong to the signlist are sl_form* */
	}
      else
	{
	  list_add(f->insts, i);
	}

      sl->curr_form = i;

      i->type = 'f';
      i->u.f = f;
      i->ref = ref;
      i->mloc = locp;
      
      if (!sl->curr_sign->hforms)
	sl->curr_sign->hforms = hash_create(128);

      hash_add(sl->curr_sign->hforms, f->name, i); /* The forms that belong to signs are sl_inst* */
    }
}

void
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list)
{
  if (!sl)
    {
      fprintf(stderr, "asl: signlist must begin with @signlist [PROJECTNAME]\n");
      exit(1);
    }
  if (hash_find(sl->hsigns, n))
    {
      mesg_verr(locp, "duplicate sign %s\n", n);
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
      i->u.s = s;
      sl->curr_sign = s;
      sl->curr_form = NULL;
      sl->curr_value = NULL;
      hash_add(sl->hsigns, s->name, s);
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
asl_bld_value(Mloc *locp, struct sl_signlist *sl, int type, const unsigned char *n,
	      const char *lang, const unsigned char *ref, int atf_flag)
{
  struct sl_value *v;
  struct sl_inst *i = memo_new(sl->m_insts);
  int xvalue = 0, uvalue = 0;

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
  if (NOV == type)
    i->deprecated = 1;
  else if (QRYV == type)
    i->query = 1;
  
  if ((v = hash_find(sl->hsignvalues, n)))
    {
      if (!sl->curr_form && !xvalue && !uvalue && !v->atf && !v->phonetic)
	{
	  /* If we are processing a sign and the v is already in
	     signlist's sign-values it's an error for it to occur
	     again */
	  if (!strcmp((ccp)v->sowner->name, (ccp)sl->curr_sign->name))
	    mesg_verr(locp, "value %s occurs more than once in sign %s\n", n, sl->curr_sign->name);
	  else
	    mesg_verr(locp, "duplicate value %s in sign %s (first occurs in %s)\n", n, sl->curr_sign->name, v->sowner->name);
	  return;
	}
    }
  
  if (!v && !(v = hash_find(sl->hvalues, n)))
    {
      Tree *tp = NULL;
      v = memo_new(sl->m_values);
      v->name = n;
      v->atf = atf_flag;
      if ('/' == *n)
	v->phonetic = 1;
      v->unknown = uvalue;
      if (!v->phonetic && !v->atf) /* should really parse ATF */
	{
	  tp = asl_bld_gdl(locp, (char*)pool_copy(v->name,sl->p));
	  v->gdl = tp->root;
	}
      if (lang)
	v->lang = lang;
      hash_add(sl->hvalues, n, v);
      if (!sl->curr_form)
	hash_add(sl->hsignvalues, n, v);
      v->insts = list_create(LIST_SINGLE);
      list_add(v->insts, i);
      
      /* Add inst to sign or form's values */
      if (sl->curr_form)
	{
	  struct sl_inst *known = NULL;
	  if (!v->fowners)
	    v->fowners = list_create(LIST_SINGLE);
	  list_add(v->fowners, sl->curr_form);
	  if (!sl->curr_form->vd)
	    sl->curr_form->vd = memo_new(sl->m_valdata);
	  if (!sl->curr_form->vd->hvalues)
	    sl->curr_form->vd->hvalues = hash_create(1);
	  else if ((known = hash_find(sl->curr_form->vd->hvalues, n)))
	    {
	      if (!xvalue)
		{
		  mesg_verr(locp, "duplicate value %s in form %s::%s", n, sl->curr_sign, sl->curr_form);
		  return;
		}
	    }
	  if (!known)
	    hash_add(sl->curr_form->vd->hvalues, v->name, i);
	}
      else
	{
	  if (xvalue)
	    {
	      if (!v->fowners)
		v->fowners = list_create(LIST_SINGLE);
	      list_add(v->fowners, sl->curr_form);
	    }
	  else	    
	    v->sowner = sl->curr_sign;
	  if (!sl->curr_sign->hvalues)
	    sl->curr_sign->hvalues = hash_create(1);
	  hash_add(sl->curr_sign->hvalues, v->name, i);
	}
    }      
  else
    {
      if (sl->curr_form)
	{
	  if (!sl->curr_form->vd)
	    sl->curr_form->vd = memo_new(sl->m_valdata);
	  if (!sl->curr_form->vd->hvalues)
	    sl->curr_form->vd->hvalues = hash_create(1);	  
	  hash_add(sl->curr_form->vd->hvalues, v->name, i);
	  if (!v->fowners)
	    v->fowners = list_create(LIST_SINGLE);
	  list_add(v->fowners, sl->curr_form);
	}
      else
	{
	  if (!sl->curr_sign->hvalues)
	    sl->curr_sign->hvalues = hash_create(1);
	  hash_add(sl->curr_sign->hvalues, v->name, i);
	  if (xvalue)
	    list_add(v->fowners, sl->curr_form);
	}
      list_add(v->insts, i);
    }

  i->u.v = v;
  sl->curr_value = i;
}

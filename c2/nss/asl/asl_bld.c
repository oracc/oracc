#include <string.h>
#include <ctype128.h>
#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <gdl.h>
#include "signlist.h"

struct sl_signlist *curr_asl = NULL;

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = malloc(sizeof(struct sl_signlist));
  sl->hsigns = hash_create(1024);
  sl->hforms = hash_create(1024);
  sl->hforms_which_are_not_signs = hash_create(128);
  sl->hsignvalues = hash_create(1024);
  sl->hletters = hash_create(32);
  sl->m_letters = memo_init(sizeof(struct sl_letter), 32);
  sl->m_groups = memo_init(sizeof(struct sl_letter), 128);
  sl->m_signs = memo_init(sizeof(struct sl_sign),512);
  sl->m_forms = memo_init(sizeof(struct sl_form),512);
  sl->m_lists = memo_init(sizeof(struct sl_value),256);
  sl->m_values = memo_init(sizeof(struct sl_value),1024);
  sl->m_signs_p = memo_init(sizeof(struct sl_sign *),512);
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
      hash_free(sl->hforms_which_are_not_signs, NULL);
      hash_free(sl->hsignvalues, NULL);
      memo_term(sl->m_letters);
      memo_term(sl->m_groups);
      memo_term(sl->m_signs);
      memo_term(sl->m_forms);
      memo_term(sl->m_lists);
      memo_term(sl->m_values);
      memo_term(sl->m_signs_p);
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
  /* get the group sign -- the first g:s node */
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

#if 0
      fprintf(stderr, "%s\t%s\t%s\n", n, letter, group);
#endif

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
  if (sl->curr_sign->forms && hash_find(sl->curr_sign->hforms, n))
    {
      mesg_verr(locp, "duplicate form %s in sign %s\n", n, sl->curr_sign->name);
    }
  else
    {
      struct sl_form *f = memo_new(sl->m_forms);
      f->mloc = locp;
      f->name = n;
      f->var = var;
      /*f->ref = ref;*/
      f->name_is_listnum = list;
      sl->curr_form = f;
      if (!hash_find(sl->hforms, f->name))
	hash_add(sl->hforms, (uccp)f->name, f);
      if (!sl->curr_sign->hforms)
	sl->curr_sign->hforms = hash_create(128);
      hash_add(sl->curr_sign->hforms, f->name, f);
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
      s->mloc = locp;
      s->name = n;
      s->name_is_listnum = list;
      sl->curr_sign = s;
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

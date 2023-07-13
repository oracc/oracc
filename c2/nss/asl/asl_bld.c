#include <tree.h>
#include <mesg.h>
#include <oraccsys.h>
#include <gdl.h>
#include "signlist.h"

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
  Tree *tp = gdlparse_string(locp, s);
  gdlparse_reset();
  return tp;
}

void
asl_bld_sign(Mloc *locp, struct sl_signlist *sl, const unsigned char *n, int list)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  s->name = n;
  s->name_is_listnum = list;
  if (hash_find(sl->hsigns, n))
    {
      mesg_verr(locp, "duplicate sign %s\n", n);
    }
  else
    {
      Tree *tp;
      unsigned const char *group;
      hash_add(sl->hsigns, n, s);
      sl->curr_sign = s;
      tp = asl_bld_gdl(locp, (char*)pool_copy(n,sl->p));
      s->gdl = tp->root;
      /* get the group sign -- the first g:s node */
      if ((group = gdl_first_s(s->gdl)))
	{
	  /* get the letter from the group sign */
	  unsigned char *letter = NULL;
	  Hash *lg; /* letter groups */
	  List *gs; /* group signs */

	  if (*group < 128)
	    {
	      letter = pool_alloc(2, sl->p);
	      letter[0] = *group;
	      letter[1] = '\0';
	    }
	  else
	    {
	      size_t x;
	      wchar_t w[2];
	      size_t n = 0;
	      w[0] = utf1char(group, &x);
	      w[1] = L'\0';
	      letter = pool_alloc((n = (wcstombs(NULL,w,0)+1)), sl->p);
	      if (n != wcstombs((char*)letter,w,n))
		mesg_verr(locp, "conversion of multibyte first letter in %s failed", group);
	    }

	  /* This is where the structure of the signlist is built */
	  
	  /* remember the letter */
	  if (!(lg = hash_find(sl->hletters, letter)))
	    hash_add(sl->hletters, letter, (lg = hash_create(128))); /* AB1: hash of letters in signlist;
									value is hash of groups */
	  /* remember the group belongs to the letter */
	  if (!(gs = hash_find(lg, group)))
	    hash_add(lg, group, (gs = list_create(LIST_SINGLE))); /* AB2: hash of groups in letter;
								     value is list of struct sl_sign * */
	  list_add(gs, s); /* AB3: list of signs in group, expressed as struct sl_sign* */
	}
      else
	mesg_verr(locp, "no sign name found in GDL of %s", n);
    }
}

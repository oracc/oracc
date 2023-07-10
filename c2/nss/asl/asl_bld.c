#include "signlist.h"

struct sl_signlist *
asl_bld_init(void)
{
  struct sl_signlist *sl = malloc(sizeof(struct sl_signlist));
  sl->signs = hash_create(1024);
  sl->forms_which_are_not_signs = hash_create(128);
  sl->m_letters = memo_init(sizeof(struct sl_letter), 32);
  sl->m_groups = memo_init(sizeof(struct sl_letter), 128);
  sl->m_signs = memo_init(sizeof(struct sl_sign),512);
  sl->m_forms = memo_init(sizeof(struct sl_form),512);
  sl->m_lists = memo_init(sizeof(struct sl_value),256);
  sl->m_values = memo_init(sizeof(struct sl_value),1024);
  return sl;
}

void
asl_bld_term(struct sl_signlist *sl)
{
  if (sl)
    {
      hash_free(sl->signs, NULL);
      hash_free(sl->forms_which_are_not_signs, NULL);
      memo_term(sl->m_letters);
      memo_term(sl->m_groups);
      memo_term(sl->m_signs);
      memo_term(sl->m_forms);
      memo_term(sl->m_lists);
      memo_term(sl->m_values);
    }
}

void
asl_bld_sign(struct sl_signlist *sl, const unsigned char *n, int list)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  s->name = n;
  s->name_is_listnum = list;
  if (hash_find(sl->signs, n))
    {
      /* error: duplicate sign */
    }
  else
    {
      hash_add(sl->signs, n, s);
      sl->curr_sign = s;
    }
}

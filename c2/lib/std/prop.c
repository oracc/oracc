#include <stdlib.h>
#include <memo.h>
#include <prop.h>

Prop *
prop_add(Memo *propmem, Prop *p, int ptype, int gtype, const char *key, const char *value)
{
  p = prop_last(p);
  p->next = memo_new(propmem);
  p = p->next;
  p->p = ptype;
  p->g = gtype;
  p->k = key;
  p->v = value;
  return p;
}

Prop*
prop_last(Prop *p)
{
  if (p)
    while (p->next)
      p = p->next;
  return p;
}

int
prop_user_group(void)
{
  static int pug = PG_LAST;
  return ++pug;
}

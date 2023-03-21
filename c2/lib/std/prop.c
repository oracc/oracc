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

#include <stdlib.h>
#include <string.h>
#include <memo.h>
#include <prop.h>

Prop *
prop_add(Memo *propmem, Prop *p, int ptype, int gtype, const char *key, const char *value)
{
  Prop *newprop = memo_new(propmem);
  p = prop_last(p);
  if (p)
    {
      p->next = newprop;
      p = p->next;
    }
  else
    p = newprop;
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

Prop*
prop_find_pg(Prop *p, int ptype, int gtype)
{
  while (p)
    {
      if (p->p == ptype && p->g == gtype)
	return p;
      p = p->next;
    }
  return NULL;
}

Prop*
prop_find_kv(Prop *p, const char *key, const char *value)
{
  if (!key || !value)
    return NULL;
  while (p)
    {
      if (p->k && p->v && !strcmp(p->k, key) && !strcmp(p->v, value))
	return p;
      p = p->next;
    }
  return NULL;  
}


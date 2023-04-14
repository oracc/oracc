#include <stdlib.h>
#include <string.h>
#include <memo.h>
#include <prop.h>

Prop *
prop_add(Memo *propmem, Prop *p, int ptype, int gtype, const char *key, const char *value)
{
  Prop *newprop = memo_new(propmem);
  Prop *ret = p;
  p = prop_last(p);
  if (p)
    {
      p->next = newprop;
      p = p->next;
    }
  else
    ret = p = newprop;
  p->p = ptype;
  p->g = gtype;
  p->k = key;
  p->v = value;
  return ret;
}

Prop*
prop_last(Prop *p)
{
  if (p)
    while (p->next)
      p = p->next;
  return p;
}

/* This is a lazy version; should really avoid adding duplicates */
void
prop_merge(Prop *into, Prop *from)
{
  Prop *l;
  if (into && from)
    {
      l = prop_last(into);
      l->next = from;
    }
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


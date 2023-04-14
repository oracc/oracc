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

void
prop_node_add(struct node *np, int ptype, int gtype, const char *key, const char *value)
{
  if (np->props)
    (void)prop_add(np->tree->propmem, np->props, ptype, gtype, key, value);
  else
    np->props = prop_add(np->tree->propmem, np->props, ptype, gtype, key, value);
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

/* Caller can use value==NULL to match key only */
Prop*
prop_find_kv(Prop *p, const char *key, const char *value)
{
  if (!key)
    return NULL;
  while (p)
    {
      if (p->k && p->v && !strcmp(p->k, key) && (NULL == value || !strcmp(p->v, value)))
	return p;
      p = p->next;
    }
  return NULL;  
}


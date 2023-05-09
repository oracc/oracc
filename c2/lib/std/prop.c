#include <stdlib.h>
#include <string.h>
#include <memo.h>
#include <prop.h>

struct gdlstate
prop_state(Node *np)
{
  if (np->props)
    {
      if (np->props->gtype != PU_GDLSTATE)
	{
	  Prop p = memo_new(np->tp->propmem);
	  p->gtype = PU_GDLSTATE;
	  p->next = np->props;
	  np->props = p;
	}
    }
  else
    {
      p->next = np->props;
      np->props = memo_new(np->tp->propmem);
      np->props->gtype = PU_GDLSTATE;
    }
  return np->props->u.g;
}

Prop *
prop_add(Memo *propmem, Prop *p, int ptype, int gtype)
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
  return ret;
}

Prop *
prop_add_kv(Memo *propmem, Memo *kevamem, Prop *p, int ptype, int gtype, const char *key, const char *value)
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
  if (key && value)
    p->u.k = keva_create(kevamem, key, value);
  return ret;
}

Prop *
prop_add_v(Memo *propmem, Prop *p, int ptype, int gtype, void *vp)
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
  p->u.v = vp;
  return ret;
}

void
prop_node_add(struct node *np, int ptype, int gtype, const char *key, const char *value)
{
  Prop *p = NULL;

  if (key)
    {
      if (value)
	p = prop_add_kv(np->tree->propmem, np->tree->kevamem, np->props, ptype, gtype, key, value);
      else
	p = prop_add_kv(np->tree->propmem, np->props, ptype, gtype, (void*)key);
    }
  else
    p = prop-add(np->tree->propmem, np->props, ptype, gtype);

  if (!np->props)
    np->props = p;
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
      if (p->g > 0 && p->g < PU_VOIDSTAR)
	{
	  if (p->u.k->k
	      && !strcmp(p->u.k->k, key)
	      && (NULL == value
		  || (p->u.k->v && !strcmp(p->u.k->v, value))))
	    return p;
	}
      p = p->next;
    }
  return NULL;  
}


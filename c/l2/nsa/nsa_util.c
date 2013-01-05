#include <string.h>
#include "nsa.h"

const char *
nsa_trim_morph(struct nsa_context *c,const char *s)
{
  static char buf[1024], *p;
  if (strlen(s) < 1024)
    {
      strcpy(buf,s);
      p = buf + strlen(buf);
      while (1)
	{
	  while (p > buf && '-' != p[-1])
	    --p;
	  if (p > buf)
	    {
	      if (hash_find(c->morph_suffixes,(unsigned char *)p))
		*--p = '\0';
	      else
		break;
	    }
	  else
	    break;
	}
      return buf;
    }
  return s;
}

/* Add n2 into n1 */
void
nsa_add_frac(struct nsa_count *n1, struct nsa_count *n2)
{
  if (n1->den == n2->den)
    n1->num += n2->num;
  else if (n2->den > n1->den && (n2->den%n1->den) == 0)
    {
      n1->num *= n2->den/n1->den;
      n1->den = n2->den;
      n1->num += n2->num;
    }
  else if ((n1->den%n2->den) == 0)
    {
      n1->num += n2->num * (n1->den/n2->den);
    }
  else
    {
      int oden = n1->den;
      n1->den = n1->den * n2->den;
      n1->num = n1->num * (n1->den/oden);
      n1->num += n2->num * (n1->den/n2->den);
    }
}

void
nsa_mul_frac(struct nsa_count *n1, struct nsa_count *n2)
{
  n1->num *= n2->num;
  n1->den *= n2->den;
}

struct nsa_count *
nsa_parse_count(const char *n, int sign)
{
  static struct nsa_count c;
  char *endp;
  c.num = strtoll(n,&endp,10);
  if (sign == -1)
    c.num *= -1;
  if (endp && *endp == '/')
    {
      if (!(c.den = atoi(endp+1)))
	fprintf(stderr,"nsa: denominator parse failed\n");
    }
  else
    c.den = 1;
  return &c;
}

struct nsa_token *
nsa_last_grapheme(List *toks)
{
  List_node *r = toks->last;
  while (r && ((struct nsa_token*)r->data)->type != NSA_T_GRAPHEME)
    r = r->prev;
  if (r)
    return r->data;
  else
    return NULL;
}

struct nsa_token *
nsa_last_unit(List *toks)
{
  List_node *r = toks->last;
  while (r 
	 && ((struct nsa_token*)r->data)->type != NSA_T_QUANTITY
	 && ((struct nsa_token*)r->data)->type != NSA_T_UNIT)
    r = r->prev;
  if (r)
    {
      struct nsa_token *rt = r->data;
      if (rt->type == NSA_T_UNIT)
	return rt;
      else
	return quantity_unit(rt);
    }
  else
    return NULL;
}

struct nsa_token *
nsa_next_grapheme(List *toks)
{
  List_node *r = toks->rover;
  while (r && ((struct nsa_token*)r->data)->type != NSA_T_GRAPHEME)
    r = r->next;
  if (r)
    return r->data;
  else
    return NULL;
}

void
nsa_append_child(struct nsa_token *parent, struct nsa_token *child)
{
  struct nsa_token **newkids;
  int nkids;
  for (nkids = 0; parent->children[nkids]; ++nkids)
    ;
  newkids = new_children(nkids+1);
  for (nkids = 0; parent->children[nkids]; ++nkids)
    newkids[nkids] = parent->children[nkids];
  newkids[nkids] = child;
  free(parent->children);
  parent->children = newkids;
}

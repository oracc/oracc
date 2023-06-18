#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <oraccsys.h>
#include "gvl.h"

int
gvl_v_isupper(unsigned const char *v)
{
  static size_t s, i;
  wchar_t *wv = utf2wcs(v,&s);
  for (i = 0; i < s; ++i)
    if (iswalpha(wv[i]))
      break;
  return iswupper(wv[i]);
}

#if 0
void
gvl_iterator_pre_fnc(Node *np, void *user)
{
  if (np && np->name && !strcmp(np->name, "g:g"))
    {
      gvl_g *g = gvl_validate((uccp)np->data);
      g->orig = (uccp)np->data;
      np->parsed = g;
    }
}

void
gvl_iterator_post_fnc(Node *np, void *user)
{
  if (np && np->name)
    {
      if (!strcmp(np->name, "g:q"))
	np->parsed = gvl_q_node(np);
      else if (!strcmp(np->name, "g:c"))
	np->parsed = gvl_c_node(np);
    }
}
#endif

unsigned char *
gvl_vmess(char *s, ...)
{
  unsigned char *ret = NULL;
  if (s)
    {
      va_list ap, ap2;
      char *e;
      int need;
      
      va_start(ap, s);
      va_copy(ap2, ap);
      need = vsnprintf(NULL, 0, s, ap);
      e = malloc(need+1);
      vsprintf(e, s, ap2);
      va_end(ap2);
      va_end(ap);
      ret = pool_copy((unsigned char *)e, curr_sl->p);
      free(e);
    }
  return ret;
}

unsigned const char *
gvl_cuneify(unsigned const char *g)
{
  return gvl_cuneify_gv(gvl_validate(g));
}

unsigned const char *
gvl_cuneify_gv(gvl_g*gg)
{
  if (gg)
    {
      if (!gg->utf8)
	gg->utf8 = gvl_lookup(sll_tmp_key((uccp)gg->oid,"uchar"));
      return gg->utf8;
    }
  else
    return NULL;
}

#if 0
/* NOTE: THIS ONLY WORKS WITH HASH-BASED VERSION OF GVL */
static unsigned const char *
gvl_key_of(unsigned const char *v)
{
  return hash_exists(curr_sl->sl, v);
}
#endif

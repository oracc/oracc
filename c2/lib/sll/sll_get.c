#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include <oraccsys.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

static char *
sll_ret_pair(const char *k, const char *oid)
{
  char *p = NULL;
  if (k && oid)
    {
      p = malloc(strlen(k) + strlen(oid) + 2);
      (void)sprintf(p, "%s\t%s", k, oid);
    }
  else if (k)
    {
      p = malloc(strlen(k) + 3);
      (void)sprintf(p, "%s\t-", k);
    }
  else if (oid)
    {
      p = malloc(strlen(oid) + 3);
      (void)sprintf(p, "-\t%s", oid);
    }
  return p;
}

const char *
sll_get_name(unsigned char *k)
{
  const char *oid = (ccp)sll_lookup(k);
  if (oid)
    return sll_ret_pair((ccp)sll_lookup(sll_tmp_key((uccp)oid, "")), oid);
  else
    return NULL;
}

const char *
sll_get_oid(unsigned char *k)
{
  return (ccp)sll_ret_pair((ccp)k, (ccp)sll_lookup(k));
}

List *
sll_get_list(const char *k)
{
  const char *o = (ccp)sll_lookup((uccp)k);
  char *otok = strdup(o);
  List *op = list_from_str(otok, NULL, LIST_SINGLE);
  char *o2 = NULL;
  List *r = list_create(LIST_SINGLE);
  
  for (o2 = list_first(op); o2; o2 = list_next(op))
    list_add(r, sll_ret_pair((ccp)sll_lookup((uccp)o2), o2));
	     
  free(otok);
  return r;
}

List *
sll_get_one(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_lookup(k));
  return r;
}

List *
sll_get_one_n(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_get_name(k));
  return r;
}

List *
sll_get_one_o(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_get_oid(k));
  return r;
}

List *
sll_get_h(const char *k)
{
  const char *h = (ccp)sll_lookup(sll_tmp_key((uccp)k, "h"));
  List *lp = NULL;
  if (h)
    {
      char *h2 = strdup(h);
      List *hp = list_from_str(h2,NULL,LIST_SINGLE);
      const char *h1 = NULL;
      char *k2 = strdup(k);
      char *b = k2, *bsemi;
      bsemi = strchr(b, ';'); if (bsemi) { *bsemi = '\0'; }
      lp = list_create(LIST_SINGLE);
      
      for (h1 = list_first(hp); h1; h1 = list_next(hp))
	{
	  char *s = strchr(h1,'/');
	  if (s)
	    {
	      *s++ = '\0';
	      list_add(lp, sll_ret_pair((ccp)sll_lookup((uccp)h1), sll_ret_pair((ccp)sll_v_from_h((uccp)b,(uccp)s), h1)));
	    }
	  else
	    list_add(lp, sll_ret_pair((ccp)sll_lookup((uccp)h1), sll_ret_pair((ccp)b, h1)));
	}
      free(k2);
      free(h2);
    }
  return lp;
}

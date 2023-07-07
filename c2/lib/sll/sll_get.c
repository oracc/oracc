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
sll_get_h(const char *k)
{
  const char *h = (ccp)sll_lookup(sll_tmp_key((uccp)k, "h"));
  List *lp = NULL;
  if (h)
    {
      lp = list_create(LIST_SINGLE);
      list_add(lp, sll_ret_pair(k, h));
    }
  return lp;
}

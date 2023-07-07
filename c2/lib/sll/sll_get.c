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

const char *
sll_get_name(unsigned char *k)
{
  const char *oid = (ccp)sll_lookup(k);
  if (oid)
    return (ccp)sll_lookup(sll_tmp_key((uccp)oid, ""));
  else
    return NULL;
}

const char *
sll_get_oid(unsigned char *k)
{
  return (ccp)sll_lookup(k);
}

List *
sll_get_h(const char *k)
{
  return NULL;
}

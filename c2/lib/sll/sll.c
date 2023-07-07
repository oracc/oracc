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

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

int sll_trace = 0;
const char *oracc = NULL;
Pool *sllpool = NULL;
Hash *sll_sl = NULL;
Dbi_index *sll_db = NULL;

unsigned const char *
sll_lookup(unsigned const char *key)
{
  if (sll_sl)
    return sll_lookup_t(key);
  else
    return sll_lookup_d(sll_db, key);
}

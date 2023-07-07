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

int raw_output = 1;
int sll_trace = 0;
const char *oracc = NULL;
Pool *sllpool = NULL;
Hash *sll_sl = NULL;
Dbi_index *sll_db = NULL;

void
sll_output(List *lp)
{
  const char *d = NULL;
  for (d = list_first(lp); d; d = list_next(lp))
    printf("%s\n", d);
}

void
sll_handle(unsigned const char *key)
{
  unsigned const char *res = sll_lookup(key);
  if (raw_output)
    printf("%s\t%s\n", key, res);
  else
    sll_output(sll_resolve(key, res));
}

unsigned const char *
sll_lookup(unsigned const char *key)
{
  if (sll_sl)
    return sll_lookup_t(key);
  else
    return sll_lookup_d(sll_db, key);
}

List *
sll_resolve(unsigned const char *key, unsigned const char *res)
{
  List *r = list_create(LIST_SINGLE);
  const char *semi = NULL;
  if (!res)
    {
      if ((semi = strchr((ccp)key, ';')))
	{
	  ++semi;
	  if (!strcmp(semi, "name"))
	    list_add(r, (void*)sll_get_name(sll_tmp_key(key,"")));
	  else if (!strcmp(semi, "oid"))
	    list_add(r, (void*)sll_get_oid(sll_tmp_key(key,"")));
	  else
	    ;
	}
    }
  else
    {
      if ((semi = strchr((ccp)key, ';')))
	{
	  struct sllext *s = NULL;
	  ++semi;
	  if ((s = sllext((ccp)key, strlen((ccp)key))))
	    {
	      List *slp = s->fnc((ccp)key);
	      if (slp)
		{
		  list_free(r, NULL);
		  r = slp;
		}
	    }
	}
      else
	list_add(r, (void*)sll_get_oid((ucp)key));
    }
  if (list_len(r))
    return r;
  else
    {
      list_free(r, NULL);
      return NULL;
    }
}

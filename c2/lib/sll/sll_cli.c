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

static void sll_cli_error(const char *err);
static void sll_cli_output(List *lp);

void
sll_cli_handler(unsigned const char *key)
{
  if (sll_raw_output)
    {
      unsigned const char *res = sll_lookup(key);
      printf("%s\t%s\n", key, res);
    }
  else
    {
      struct sllext *ep = NULL;
      unsigned char *g = NULL, *e = NULL;
      g = (ucp)pool_copy(key, sllpool);
      if ((e = (ucp)strchr((ccp)g, ';')))
	*e++ = '\0';
      wgrapheme = (ccp)g;
      wextension = (ccp)e;
      if (e && !(ep = sllext((ccp)e, strlen((ccp)e))))
	sll_cli_error("error");
      else
	sll_cli_output(sll_resolve((uccp)g, (ccp)e, ep));
      /*free(g);*/ /* NEED TO FREE AT END if !caller free(wgrapheme) */
    }
}

static void
sll_cli_error(const char *err)
{
  fprintf(stderr, "%s\n", err);
}

static void
sll_cli_output(List *lp)
{
  if (lp)
    {
      const char *d = NULL;
      for (d = list_first(lp); d; d = list_next(lp))
	printf("%s\n", d);
    }
}

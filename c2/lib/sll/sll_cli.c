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
#include <gdl.h>
#include "sll.h"

static void sll_cli_error(const char *err);
static void sll_cli_output(List *lp);

static int sll_voidsl;

void
sll_cli_voidsl(void)
{
  sll_voidsl = 1;
}

int
is_oid(const char *s)
{
  if (s && 'o' == *s)
    {
      while (isdigit(s[1]))
	++s;
      if ('\0' == s[1])
	return 1;
    }
  return 0;
}

void
sll_cli_handler(unsigned const char *key)
{
  if (sll_raw_output)
    {
      char *gdloo = gdl_one_off("<cli>", 1, (ccp)key, sll_voidsl);
      if (gdloo)
	{
	  unsigned const char *res = sll_lookup((uccp)gdloo);
	  printf("%s\t%s\t%s\n", key, gdloo, res);
	}
      else
	printf("%s\t#error\n", key);
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
      if (wgrapheme)
	{
	  char *gdloo = (is_oid(wgrapheme) ? (char*)wgrapheme : gdl_one_off("<cli>", 1, (ccp)wgrapheme, sll_voidsl));
	  if (!gdloo)
	    sll_cli_error("invalid GDL grapheme syntax");
	  else
	    {
	      if (e && !(ep = sllext((ccp)e, strlen((ccp)e))))
		sll_cli_error("error in sll key extension");
	      else
		sll_cli_output(sll_resolve((uccp)gdloo, (ccp)e, ep));
	      /*free(g);*/ /* NEED TO FREE AT END if !caller free(wgrapheme) */
	    }
	}
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

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

int sll_raw_output = 0;
int sll_trace = 0;
const char *oracc = NULL;
Pool *sllpool = NULL;
Hash *sll_sl = NULL;
Dbi_index *sll_db = NULL;

/* Web variables */

const char *wcaller;
const char *wext;
const char *wsign;
const char *wproject;

static unsigned const char *sll_ext_check(unsigned const char *k, enum sll_t t);
static List *sll_resolve(unsigned const char *g, const char *e, struct sllext *ep);

void
sll_web_error(const char *err)
{
  fprintf(stderr, "%s\n", err);
}

void
sll_web_output(List *lp)
{
  if (lp && list_len(lp))
    {
      if (!strcmp(wcaller, "esp"))
	sll_esp_output(lp);
      else
	sll_uri_output(lp);
    }
  else
    {
      if (!strcmp(wgrapheme, "#none"))
	{
	  sll_html_header();
	  sll_sign_frame("");
	  sll_html_trailer();
	}
      else
	{
	  sll_html_header();
	  printf("<p class=\"nomatch\">No matches</p>");
	  sll_html_trailer();
	}
    }
}

void
sll_web_handler(const char *wcaller, const char *wextension, const char *wgrapheme, const char *wproject)
{
  struct sllext *ep = NULL;
  if (wextension && !(ep = sllext(wextension, strlen(wextension))))
    sll_web_error("error");
  else
    {
      Dbi_index *dbi = dbi_init(wproject, NULL);
      sll_web_output(sll_resolve(wgrapheme, wextension, ep));
      dbi_term(dbi);
    }
  fflush(stdout);
  exit(0);
}

void
sll_cli_error(const char *err)
{
  fprintf(stderr, "%s\n", err);
}

void
sll_cli_output(List *lp)
{
  if (lp)
    {
      const char *d = NULL;
      for (d = list_first(lp); d; d = list_next(lp))
	printf("%s\n", d);
    }
}

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
      List *lp = NULL;
      const unsigned char *g = NULL, *e = NULL;
      g = strdup(key);
      if ((e = strchr(key, ';')))
	*e++ = '\0';
      if (e && !(ep = sllext(e, strlen(e))))
	sll_cli_error("error");
      else
	sll_cli_output(sll_resolve(g, e, ep));
      free(g);
    }
}

unsigned const char *
sll_lookup(unsigned const char *key)
{
  if (sll_sl)
    return sll_lookup_t(key);
  else
    return sll_lookup_d(sll_db, key);
}

static List *
sll_resolve(unsigned const char *g, const char *e, struct sllext *ep)
{
  List *r = NULL;
  if (ep)
    r = ep->fnc(g);
  else
    r = sll_get_one(g);
  if (list_len(r))
    return r;
  else
    {
      list_free(r, NULL);
      return NULL;
    }
}

static unsigned const char *
sll_ext_check(unsigned const char *k, enum sll_t t)
{
  if (t == SLL_ID)
    {
      if (*k == 'o' && isdigit(k[1]))
	return k;
      else
	return sll_lookup(k);
    }
  else if (t == SLL_SN)
    {
      if (sll_has_sign_indicator(k))
	return k;
      else
	{
	  const char *oid = sll_lookup(k);
	  if (oid)
	    return sll_lookup(oid);
	  else
	    return NULL;
	}
    }
}


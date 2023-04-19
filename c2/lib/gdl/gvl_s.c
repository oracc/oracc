#include <stdlib.h>
#include <wctype.h>
#include <memo.h>
#include <pool.h>
#include <oraccsys.h>
#include "gdl.h"
#include "gvl.h"
#include "unidef.h"

extern int gvl_strict;
extern int gvl_trace;
extern int curr_lang;

static const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";

gvl_g *
gvl_s(Node *ynp)
{
  gvl_g *gp = NULL;
  unsigned const char *l = NULL;

  gp = memo_new(curr_sl->m);
  
  gp->orig = (uccp)ynp->text;
  gp->type = ynp->name + 2;
  if ('g' == *gp->type)
    {
      if (sll_has_sign_indicator(gp->orig))
	{
	  gp->type = "s";
	  ynp->name = "g:s";
	}
      else
	{
	  gp->type = "v";
	  ynp->name = "g:v";
	}
    }
  else if ('B' == *gp->type)
    {
      /* This is a barenum--we need to sexify and return the result */
      gvl_n_sexify(ynp);
      return ynp->user;
    }

  if ('a' != curr_lang && 's' != curr_lang && (curr_lang < '0' || curr_lang > '9'))
    return gp;
  
  if ((l = gvl_lookup(gp->orig)))
    {
      /* best case: g is a known sign or value */
      gp->oid = (ccp)l;
      gp->sign = gvl_lookup(sll_tmp_key(l,""));
      gp->c10e = gp->orig;
    }
  else if ('s' == *gp->type || 'N' == *gp->type)
    {
      const unsigned char *lg = NULL, *l = NULL;
      unsigned const char *c10e = NULL;
      if ((lg = utf_lcase(gp->orig)))
	c10e = gvl_s_c10e(lg);
      if (c10e)
	{
	  gp->oid = (ccp)gvl_lookup(c10e);
	  if ('N' == *gp->type)
	    gp->c10e = gp->orig;
	  else
	    gp->c10e = gp->sign = c10e;
	  if (gvl_strict && 'N' != *gp->type)
	    gp->mess = gvl_vmess("pseudo-signname %s should be %s", gp->orig, gp->sign);
	}
      else if (lg && (l = gvl_lookup(sll_tmp_key(lg, "q"))))
	{
	  gp->c10e = gp->sign = l; /* we should really set gp->sign to, say, #q and store l in a prop */
	  gp->mess = gvl_vmess("pseudo-signname %s must be qualifed by one of %s",gp->orig,l);
	}
      else
	{
	  if (!gdl_legacy_check(ynp, gp->orig))
	    gp->mess = gvl_vmess("unknown sign name: %s", gp->orig);
	}
    }
  else if ('l' == *gp->type)
    {
      if ((l = gvl_lookup(sll_tmp_key(gp->orig,"l"))))
	{
	  gp->type = "l";
	  gp->oid = (ccp)l;
	  gp->sign = gvl_lookup(sll_tmp_key(l,""));
	  gp->c10e = gp->orig;
	}
      else
	{
	  if (!gdl_legacy_check(ynp, gp->orig))
	    gp->mess = gvl_vmess("unknown signlist name: %s", gp->orig);
	}

    }
  else
    {
      const unsigned char *gq = gvl_lookup(sll_tmp_key(gp->orig,"q"));
      if (gq)
	gp->mess = gvl_vmess("value %s must be qualified with one of %s", gp->orig, gq);
      else if (*gp->orig == '*' && !gp->orig[1])
	{
	  gp->type = "p";
	  gp->c10e = gp->sign = (uccp)"DIŠ";
	  gp->oid = (ccp)gvl_lookup(gp->sign);
	}
      else
	{
	  if (!gdl_legacy_check(ynp, gp->orig))
	    gp->mess = gvl_vmess("unknown value: %s. To request adding it please visit:\n\t%s", gp->orig, report);
	}
    }

  return gp;
}

unsigned const char *
gvl_s_c10e(unsigned const char *orig)
{
  unsigned const char *l;
  if ((l = gvl_lookup(orig)))
    return gvl_lookup(sll_tmp_key(l,""));
  else
    return NULL;
}

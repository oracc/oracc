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
  static int c10e_err = 0;
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
      gp->c10e = gp->sign;
    }
  else if ('s' == *gp->type)
    {
      unsigned const char *c10e = gvl_s_c10e(gp->orig);
      if (c10e)
	{
	  gp->oid = gvl_lookup(c10e);
	  gp->c10e = gp->sign = c10e;
	  if (gvl_strict)
	    gp->mess = gvl_vmess("pseudo-signname %s should be %s", gp->orig, gp->sign);
	}
      else if ((l = gvl_lookup(sll_tmp_key(lg,"q"))))
	{
	  gp->sign = l;
	  if (gvl_strict)
	    gp->mess = gvl_vmess("pseudo-signname %s must be qualifed by one of %s",gp->orig,l);
	}
    }
  else if ('l' == *gp->type)
    {
      if ((l = gvl_lookup(sll_tmp_key(gp->c10e,"l"))))
	{
	  gp->type = "l";
	  gp->oid = (ccp)l;
	  gp->c10e = gp->sign = gvl_lookup(sll_tmp_key(l,""));
	}
      else
	gp->mess = gvl_vmess("unknown sign name: %s", gp->orig);
    }
  else
    {
      const unsigned char *gq = gvl_lookup(sll_tmp_key(gp->c10e,"q"));
      if (gq)
	gp->mess = gvl_vmess("value %s must be qualified with one of %s", gp->orig, gq);
      else if (*gp->orig == '*' && !gp->orig[1])
	{
	  gp->type = "p";
	  gp->sign = (uccp)"DIŠ";
	  gp->oid = (ccp)gvl_lookup(gp->sign);
	}
      else
	gp->mess = gvl_vmess("unknown value: %s. To request adding it please visit:\n\t%s", gp->orig, report);
    }

  return gp;
}

#define G_C10E_MIXED_CASE 0x02
#define G_C10E_FINAL_SUBX 0x04

unsigned char *
gvl_s_c10e(unsigned const char *orig)
{
  const unsigned char *lg = utf_lcase(orig);
  if ((l = gvl_lookup(lg)))
    return gvl_lookup(sll_tmp_key(l,""));
  else
    return NULL;
}

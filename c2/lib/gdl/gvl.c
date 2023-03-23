/* Grapheme Validation Library */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>

#include <oraccsys.h>
#include <memo.h>
#include <pool.h>
#include <hash.h>
#include <tree.h>

#include "gdl.h"
#include "gvl.h"

static int gvl_trace = 1;
extern gvl_i *curr_sl;
int gvl_strict = 0;

static const char *report = "http://oracc.museum.upenn.edu/ogsl/reportingnewvalues/";

unsigned char *gvl_v_from_h(const unsigned char *b, const unsigned char *qsub);
unsigned char *gvl_val_base(const unsigned char *v);

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

ucp c10e_compound(uccp g){ return (ucp)g; }

#define gvl_BAD ""
#define gvl_v "v"
#define gvl_s "s"
#define gvl_n "n"
#define gvl_p "p"
#define gvl_c "cs"
#define gvl_q "qv"

static int
gnr(const char *g)
{
  return ((*g == 'n' || *g == 'N') && (g[1]=='(' || g[1]=='+')) ? '1' : 0;
}

static const char *
gvl_type(unsigned const char *g)
{
  if ((*g > 127 || isalpha(*g) || *g == '\'') && !gnr((const char *)g))
    {
      if (strchr((ccp)g,'('))
	{
	  while (*g)
	    if ('!' == *g)
	      return gvl_v;
	    else if ('(' == *g)
	      return gvl_q;
	    else
	      ++g;
	  abort(); /* can't happen */
	}
      else
	{
	  if (u_isupper(g))
	    {
	      g += mbtowc(NULL,(const char *)g,6);
	      while (*g)
		if (u_islower(g))
		  return gvl_v;
		else if (isdigit(*g) || '@' == *g || '~' == *g)
		  break;
		else
		  g += mbtowc(NULL,(const char *)g,6);
	      return gvl_s;
	    }
	  else
	    return gvl_v;
	}
    }
  else if (isdigit(*g) || *g == 'n' || *g == 'N')
    return gvl_n;
  else if (*g == '|')
    {
      register unsigned const char *e = g;
      while (*++e != '|')
	;
      if (*e == '(')
	return gvl_q;
      else
	return gvl_c;
    }
  else if (*g == '*' || *g == ':' || *g == '/')
    return gvl_p;
  else
    return gvl_BAD;
}

unsigned const char *
gvl_ucode(gvl_g*gg)
{
  if (gg)
    {
      if (!gg->uhex)
	gg->uhex = gvl_lookup(sll_tmp_key((uccp)gg->oid,"ucode"));
      return gg->uhex;
    }
  else
    return NULL;
}

unsigned const char *
gvl_get_id(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return (uccp)gg->oid;
  else
    return NULL;
}

unsigned const char *
gvl_get_sname(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return gg->sign;
  else
    return NULL;
}

int
gvl_is_sname(unsigned const char *g)
{
  gvl_g*gg = gvl_validate(g);
  if (gg)
    return gg->type && *gg->type == 's';
  else
    return 0;
}

int
gvl_looks_like_sname(unsigned const char *g)
{
  return sll_has_sign_indicator(g);
}

/* using gvl_validate here changes the semantics of

     #define psl_is_value gvl_is_value

   because gvl_validate succeeds on
   graphemes spelled with heth and psl_is_value fails
 */
int
gvl_is_value(unsigned const char *g)
{
#if 1
  return gvl_lookup(g) != NULL;
#else
  return gvl_validate(g) != NULL;
#endif
}

unsigned const char *
gvl_lookup(unsigned const char *key)
{
  if (gvl_trace)
    {
      if (key)
	{
	  uccp res = hash_find(curr_sl->sl, (const unsigned char *)key);
	  if (res)
	    fprintf(stderr, "gvl: gvl_lookup found %s for key %s\n", res, key);
	  return res;
	}
      else
	return NULL;
    }
  else
    return key ? hash_find(curr_sl->sl, (const unsigned char *)key) : NULL;
  return NULL;
}

#if 0
static int
gvl_v_isupper(unsigned const char *v)
{
  static size_t s, i;
  wchar_t *wv = utf2wcs(v,&s);
  for (i = 0; i < s; ++i)
    if (iswalpha(wv[i]))
      break;
  return iswupper(wv[i]);
}
#endif

#if 0
static int
gvl_try_h(gvl_g *gp, gvl_g *vp, gvl_g *qp, unsigned char *q_fixed, unsigned char **mess)
{
  int qv_bad = 1;
  unsigned char *p = sll_try_h(qp->oid, vp->text);
  
  /* If the gp->text value is uppercase, make the result value
     uppercase; then if value == qp->sign, elide the value and
     just print the qp-sign with no parens */
  if (gvl_v_isupper(gp->text) && !strcmp((ccp)(p=g_uc(p)), (ccp)qp->sign))
    *mess = gvl_vmess("%s: should be %s%s", gp->text, qp->sign, QFIX);
  else if (strcmp((ccp)vp->text,(ccp)p) || (ccp)q_fixed)
    *mess = gvl_vmess("%s: should be %s(%s)%s", gp->text, p, qp->sign, QFIX);
  
  qv_bad = 0;

  if (p)
    free(p);

  return qv_bad;
}
#endif

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;

  if (g)
    {
      if (gvl_trace)
	fprintf(stderr, "gvl_validate: called with g=%s\n", g);
      if (!(gp = hash_find(curr_sl->h,g)))
	{
	  unsigned const char *l = NULL;
	  unsigned char *a = NULL;
	  static int c10e_err = 0;

	  c10e_err = 0;
	  a = g_c10e(g, &c10e_err);
	  if (a)
	    {
	      if (strcmp((ccp)a,(ccp)g))
		{
		  g = pool_copy(g,curr_sl->p);
		  if (gvl_trace)
		    fprintf(stderr, "gvl_validate: %s mapped to %s\n", g, a);
		  gvl_g *gp2 = hash_find(curr_sl->h,a);
		  if (gp2)
		    {
		      gp2->accn = g;
		      hash_add(curr_sl->h, g, gp2);
		      /* note: don't free a directly as it belongs to wcs2utf */
		      return gp2;
		    }
		}
	      else
		{
		  a = NULL;
		}
	    }

	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s not found in seen-hash\n", g);

	  gp = memo_new(curr_sl->m);
	  if (a)
	    {
	      gp->text = pool_copy(a,curr_sl->p);
	      gp->accn = pool_copy(g,curr_sl->p);
	      hash_add(curr_sl->h, gp->text, gp);
	      hash_add(curr_sl->h, gp->accn, gp);
	      g = gp->text;
	      a = NULL;
	    }
	  else
	    {
	      gp->text = pool_copy(g, curr_sl->p);
	      hash_add(curr_sl->h, gp->text, gp);
	    }
	  
	  gp->type = gvl_type(g);
	  
	  if (*gp->type == 'q')
	    {
	      gvl_vq(g, gp);
	    }
	  else if ((l = gvl_lookup(g)))
	    {
	      /* best case: g is a known sign or value */
	      gp->oid = (ccp)l;
	      gp->sign = gvl_lookup(sll_tmp_key(l,""));
	    }
	  else if (*gp->type == 'n')
	    {
	      char *paren = strchr((ccp)g, '(');
	      if (paren)
		{
		  if ((l = gvl_lookup(g)))
		    {
		      gp->oid = (ccp)l;
		      gp->sign = gvl_lookup(sll_tmp_key(l,""));
		    }
		  else
		    {
		      const unsigned char *lg = utf_lcase(g);
		      if ((l = gvl_lookup(lg)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(sll_tmp_key(l,""));
#if 0
			  /* allow 1(U@c) as well as 1(u@c) without a warning--is this right? */
			  if (gvl_strict)
			    gp->mess = gvl_vmess("numeric pseudo-signname %s should be %s", g, gp->sign);
#endif
			}
		      else if ('n' == *lg)
			{
			  char *oneify = strdup((ccp)lg);
			  *oneify = '1';
			  if ((l = gvl_lookup((uccp)oneify)))
			    {
			      gp->oid = (ccp)l;
			      gp->sign = gvl_lookup(sll_tmp_key(l,""));
			    }
			  else
			    gp->mess = gvl_vmess("expected to validate %s via %s but %s doesn't exist", g, oneify, oneify);
			  if (oneify)
			    free(oneify);
			}
		      else
			{
			  gp->mess = gvl_vmess("unknown numeric sign %s", g);
			}
		    }
		}
	      else
		{
		  int n = 0;
		  if ((n = atoi((ccp)g)))
		    {
		      unsigned char *sx = sexify(n, "disz");
		      hash_add(curr_sl->h, pool_copy(sx,curr_sl->p), gp);
		      if ((l = gvl_lookup(sx)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(sll_tmp_key(l,""));
			}
		      else
			{
#if 0
			  /* TBD: if this has spaces; split and possibly use a list in gp */
			  gp->mess = gvl_vmess("unknown sexified number sign %s [< %s]", sx, g);
#endif
			}
		    }
		  else if (strchr((char*)g, '/'))
		    {
		      char *qnum = malloc(strlen((char*)g) + strlen("(diš)") + 1);
		      (void)sprintf((char*)qnum, "%s(diš)", g);
		      hash_add(curr_sl->h, pool_copy((uccp)qnum,curr_sl->p), gp);
		      if ((l = gvl_lookup((uccp)qnum)))
			{
			  gp->oid = (ccp)l;
			  gp->sign = gvl_lookup(sll_tmp_key(l,""));
			}
		      else
			gp->mess = gvl_vmess("unknown sexified fraction %s [< %s]", qnum, g);
		      free(qnum);
		    }
		  else
		    gp->mess = gvl_vmess("unable to sexify non-numeric %s", g);
		}
	    }
	  else if (*gp->type == 'p')
	    {
	      if ((l = gvl_lookup(g)))
		{
		  gp->oid = (ccp)l;
		  gp->sign = gvl_lookup(sll_tmp_key(l,""));
		}
	      else
		{
		  char *paren = strchr((ccp)g, '(');
		  if (paren)
		    {
		      unsigned char *q = malloc(strlen((ccp)paren));
		      strcpy((char*)q,paren+1);
		      if ((paren = strrchr((ccp)q,')')))
			{
			  *paren = '\0';
			  if ((l = gvl_lookup(q)))
			    {
			      gp->oid = (ccp)l;
			      gp->sign = gvl_lookup(sll_tmp_key(l,""));
			    }
			  else
			    gp->mess = gvl_vmess("%s: punctuation with unknown sign qualifier", g);
			}
		      else
			gp->mess = gvl_vmess("%s: qualifier without closing ')'", g);
		    }
		}
	    }
	  else if (sll_has_sign_indicator(g))
	    {
	      const unsigned char *lg = utf_lcase(g);
	      if ((l = gvl_lookup(lg)))
		{
		  gp->oid = (ccp)l;
		  gp->sign = gvl_lookup(sll_tmp_key(l,""));
		  if (gvl_strict)
		    gp->mess = gvl_vmess("pseudo-signname %s should be %s", g, gp->sign);
		}
	      else if ((l = gvl_lookup(sll_tmp_key(lg,"q"))))
		{
		  gp->sign = l;
		  if (gvl_strict)
		    gp->mess = gvl_vmess("pseudo-signname %s must be qualifed by one of %s",g,l);
		}
	      else
		{
		  if ('|' == *g)
		    {
		      unsigned char *c10e = c10e_compound(g);
		      if (c10e)
			{
			  if ((l=gvl_lookup(c10e)))
			    {
			      gp->oid = (ccp)l;
			      gp->sign = gvl_lookup(sll_tmp_key(l,""));
			      if (gvl_strict)
				gp->mess = gvl_vmess("compound %s should be %s", g, c10e);
			    }
			  else
			    {
			      unsigned char *c10e_no_p = sll_strip_pp(c10e);
			      if (c10e_no_p && (l=gvl_lookup(c10e_no_p)))
				{
				  gp->oid = (ccp)l;
				  gp->sign = gvl_lookup(sll_tmp_key(l,""));
				  if (gvl_strict)
				    gp->mess = gvl_vmess("compound %s should be %s", g, c10e_no_p);
				}
			      else
				{
				  if (!strchr((ccp)g,'X')) /* don't error on compounds with X--should be configurable */
				    {
				      if (strcmp((ccp)g, (ccp)c10e) && c10e_no_p && strcmp((ccp)c10e, (ccp)c10e_no_p))
					gp->mess = gvl_vmess("unknown compound: %s (also tried %s/%s)", g, c10e, c10e_no_p);
				      else if (strcmp((ccp)g, (ccp)c10e))
					gp->mess = gvl_vmess("unknown compound: %s (also tried %s)", g, c10e);
				      else if (c10e_no_p && strcmp((ccp)g, (ccp)c10e_no_p))
					gp->mess = gvl_vmess("unknown compound: %s (also tried %s)", g, c10e_no_p);
				      else
					gp->mess = gvl_vmess("unknown compound: %s", g);
				    }
				}
			      free(c10e_no_p);
			    }
			}
		    }
		  else if ((l = gvl_lookup(sll_tmp_key(g,"l"))))
		    {
		      gp->type = "l";
		      gp->oid = (ccp)l;
		      gp->sign = gvl_lookup(sll_tmp_key(l,""));
		    }
		  else
		    gp->mess = gvl_vmess("unknown sign name: %s", g);
		}
	    }
	  else
	    {
	      const unsigned char *gq = gvl_lookup(sll_tmp_key(g,"q"));
	      if (gq)
		gp->mess = gvl_vmess("value %s must be qualified with one of %s", g, gq);
	      else
		gp->mess = gvl_vmess("unknown value: %s. To request adding it please visit:\n\t%s", g, report);
	    }
	}
      else
	{
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s was found in hash\n", g);
	}
    }
  
  return gp;
}

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

int gvl_no_mesg_add = 0;
int gvl_sans_report = 0;
int gvl_trace = 0;
int gvl_strict = 0;
int c_delim_sentinel = 0;

extern int curr_lang;

gvl_lookup_ptr gvl_lookup = gvl_lookup_sl;

unsigned char *gvl_v_from_h(const unsigned char *b, const unsigned char *qsub);
unsigned char *gvl_val_base(const unsigned char *v);

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

ucp c10e_compound(uccp g){ return (ucp)g; }

#if 0
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

#endif

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

/* Backward compatibility so ox can work with c2/gdl  */
int
gvl_psl_lookup(unsigned const char *g)
{
  return gvl_lookup(g) != NULL;
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
gvl_lookup_sl(unsigned const char *key)
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

gvl_g *
gvl_make_gp(unsigned const char *g, unsigned const char *mess)
{
  gvl_g *gp = memo_new(curr_sl->m);
  gp->orig = (uccp)pool_copy(g, curr_sl->p);
  gp->mess = mess;
  return gp;
}

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;
  const char *oid = NULL;

  if ((gp = hash_find(curr_sl->h,g)))
    return gp;
  else if ((oid = hash_find(curr_sl->sl,g)))
    {
      gp = memo_new(curr_sl->m);
      gp->orig = (uccp)pool_copy(g, curr_sl->p);
      gp->sign = hash_find(curr_sl->sl, (uccp)oid);
      gp->oid = (ccp)pool_copy((uccp)oid, curr_sl->p);
      gp->type = "v";
      hash_add(curr_sl->h,gp->orig,gp);
      return gp;
    }
  else
    return NULL;
}

void
gvl_compound(Node *ynp)
{
  extern int gdl_orig_mode;
  
  if (!ynp || !ynp->kids)
    return;

  if (gvl_trace)
    fprintf(stderr, "gvl_compound: called\n");

  /* In |A.A|~a ynp is currently at the g:b wrapper which is first
     child of g:c node */
  if (!strcmp(ynp->name, "g:b"))
    ynp = ynp->rent;
  
  if (!strcmp(ynp->name, "g:c"))
    {
      gvl_g *cp = NULL;
      unsigned char *c_orig = gvl_c_orig(ynp);
      if (!c_delim_sentinel)
	mesg_verr(ynp->mloc, "unnecessary pipes on '%s'", c_orig);

      if (!(cp = hash_find(curr_sl->h, c_orig)))
	{
	  cp = memo_new(curr_sl->m);
	  cp->type = "c";
	  cp->orig = pool_copy(c_orig, curr_sl->p);
	  cp->c10e = pool_copy(gvl_c_c10e(ynp), curr_sl->p);

	  gvl_c(cp);

#if 0
	  /* temp? removal */
	  hash_add(curr_sl->h, cp->orig, cp);
#endif

#if 0
	  /*UPDATE: it's always an error to hash c10e in this way
	    because after finding |MUL.BAR| input |3×AN.BAR| gets
	    treated as |MUL.BAR| */
	  /* In gdl_orig_mode it's an error to make the c10e version
	     map to the orig version; if |GIŠ₃&GIŠ₃| comes before
	     |UŠ&UŠ| then |UŠ&UŠ| wrongly maps to |GIŠ₃&GIŠ₃|.
	   */
	  if (!gdl_orig_mode && strcmp((ccp)cp->orig, (ccp)cp->c10e))
	    hash_add(curr_sl->h, cp->c10e, cp);
#endif
	}
      ynp->user = cp;
      if (gdl_orig_mode)
	{
	  ynp->text = (ccp)cp->orig;
	  gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)cp->orig);
	}
      else
	{
	  ynp->text = (ccp)cp->c10e;
	  gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)cp->c10e);
	}
      if (cp->mess && !gvl_no_mesg_add)
	mesg_err(ynp->mloc, (ccp)cp->mess);
    }
}

void
gvl_simplexg(Node *ynp)
{
  gvl_g *gp = NULL;
  unsigned const char *g = NULL;

  if (!ynp || !ynp->text || ynp->name[2] == 'x' || !strcmp(ynp->name, "g:gp") || 'n' == curr_lang)
    return;

  g = (uccp)ynp->text;

  if (gvl_trace)
    fprintf(stderr, "gvl_simplexg: called with g=%s\n", g);

  if (!(gp = hash_find(curr_sl->h,g)))
    gp = gvl_s(ynp);
  
  /* it isn't an error for gp to be NULL because when we gvl_n_sexify
     in gvl_s the result ynp is a g:gp and doesn't have a gp node */
  if (gp)
    {
      ynp->user = gp;
      if (ynp->kids)
	gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)gp->orig);

      if (gp->mess && !gvl_no_mesg_add)
	mesg_err(ynp->mloc, (ccp)gp->mess);
    }
}

static int
ndig(const char *text)
{
  return 'N' == *text && isdigit(text[1]);
}

void
gvl_valuqual(Node *vqnp)
{
  if (!vqnp || !vqnp->kids || !vqnp->kids->next)
    return;

  if (gvl_trace)
    fprintf(stderr, "gvl_valuqual: called\n");
  
  if ('q' == vqnp->name[2])
    {
      vqnp->mloc = vqnp->kids->mloc;
      
      if ('R' == vqnp->kids->name[2]
	  || 'n' == vqnp->kids->name[2]
	  || 'N' == vqnp->kids->name[2]
	  || (vqnp->kids->text && !strcmp(vqnp->kids->text, "X")
	      && vqnp->kids->next && ndig(vqnp->kids->next->text))
	  ) /* gvl_s make n or N type g:n */
	gvl_n(vqnp);
      else
	gvl_q(vqnp);

      if (strcmp(vqnp->name, "g:corr"))
	{
	  if (vqnp->user)
	    {
	      if (((gvl_g*)vqnp->user)->mess && !gvl_no_mesg_add)
		mesg_err(vqnp->mloc, (ccp)((gvl_g*)(vqnp->user))->mess);
	    }
	  else	
	    {
	      mesg_verr(vqnp->mloc, "failed to create grapheme structure from '%s'", vqnp->text);
	    }
	}
    }
}

#include <stdlib.h>
#include <wctype.h>
#include <memo.h>
#include <pool.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>
#include "gdl.h"
#include "gvl.h"
#include "gdl.tab.h"

#define G_C10E_MIXED_CASE 0x02
#define G_C10E_FINAL_SUBX 0x04

static Hash *legacy_reported_h = NULL;
extern const char *curr_pqx;
extern int curr_pqx_line;

unsigned char *
gdl_unlegacy_str(Mloc *mlp, unsigned const char *g)
{
  unsigned char *ret = NULL;
  wchar_t *w;
  size_t len;
  int i;
  int suppress_case_check = 0;
  static int err;

  if ((w = utf2wcs(g, &len)))
    {
      wchar_t *x = malloc(3*len*sizeof(wchar_t));
      wchar_t cued_sub_23 = 0;
      size_t xlen = 0;
      int found_l = 0;
      int found_u = 0;
      
      for (i = 0; i < len; ++i)
	{
	  switch (w[i])
	    {
	    case '@':
	    case '~':
	    case '\\':
	      if (w[i+1] && iswalpha(w[i+1]) && iswlower(w[i+1]))
		suppress_case_check = 1;
	    case '|':
	    case '.':
	    case U_times:
	    case '-':
	    case ':':
	    case '+':
	    case '&':
	    case '%':
	    case '(':
	    case ')':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      x[xlen++] = w[i];
	      break;
	    case '*':
	      /* In initial position this is a bullet and must be
		 kept; otherwise it's a collation flag and is dropped */
	      if (0 == i)
		x[xlen++] = '*';
	      break;
	    case '[':
	      gdl_balance_break(*mlp, '[', "[");
	      break;
	    case ']':
	      gdl_balance_break(*mlp, ']', "]");
	      break;
	    case U_ulhsq:
	      gdl_balance_break(*mlp, L_uhs, NULL);
	      break;
	    case U_urhsq:
	      gdl_balance_break(*mlp, R_uhs, NULL);
	      break;
	    case U_llhsq:
	      gdl_balance_break(*mlp, L_lhs, NULL);
	      break;
	    case U_lrhsq:
	      gdl_balance_break(*mlp, R_lhs, NULL);
	      break;
	    case '#':
	    case '?':
	    case '!':
	    case U_cdot:
	    case U_degree:
	      /* delete these */
	      break;
	    case '<':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      if (w[i+1] == '(')
		{
		  x[xlen++] = '<';
		  x[xlen++] = '(';
		  ++i;
		}
	      break;
	    case '>':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      if (w[i-1] == ')')
		x[xlen++] = '>';
	      break;
	    case U_heth:
	      x[xlen++] = 'h';
	      ++found_l;
	      break;
	    case U_HETH:
	      x[xlen++] = 'H';
	      ++found_u;
	      break;
	    case '\'':
	      x[xlen++] = U_aleph;
	      break;
	    case a_acute:
	    case e_acute:
	    case i_acute:
	    case u_acute:
	    case a_grave:
	    case e_grave:
	    case i_grave:
	    case u_grave:
	      cued_sub_23 = g_subdig_of(w[i]);
	      x[xlen++] = g_vowel_of(w[i]);
	      ++found_l;
	      break;
	    case A_acute:
	    case E_acute:
	    case I_acute:
	    case U_acute:
	    case A_grave:
	    case E_grave:
	    case I_grave:
	    case U_grave:
	      cued_sub_23 = g_subdig_of(w[i]);
	      x[xlen++] = g_vowel_of(w[i]);
	      ++found_u;
	      break;
	    case U_s_x:
	      /* This block may be unnecessary with GVL */
	      x[xlen++] = w[i];
	      if ('(' != w[i+1])
		err |= G_C10E_FINAL_SUBX;
	      break;
	    default:
	      if (iswalpha(w[i]) && !suppress_case_check)
		{
		  if (iswupper(w[i]))
		    ++found_u;
		  else
		    ++found_l;
		}
	      x[xlen++] = w[i];
	      break;
	    }
	}

      if (cued_sub_23)
	x[xlen++] = cued_sub_23;
      x[xlen] = 0;

      if (found_l && found_u && !suppress_case_check)
	{
	  /* size_t i; */
	  err |= G_C10E_MIXED_CASE;
#if 0
	  /* this may not be worth the problems it causes */
	  if (found_l > found_u)
	    for (i = 0; i < xlen; ++i)
	      x[i] = towlower(x[i]);
	  else
	    for (i = 0; i < xlen; ++i)
	      x[i] = towupper(x[i]);
#endif
	}
      ret = wcs2utf(x,xlen);
      free(x);
    }
  return ret;
}

/* if gdl_legacy is set (normally via #atf: use legacy) this routine
   stores the value of np->text in a property named 'legacy' and
   replaces np->text with a cleaned version without the brackets. This
   means that GVL never sees legacy bracketed data */
void
gdl_unlegacy(Node *np)
{
  unsigned char *res = NULL;
  
  /* While this is only called from gdl on 's' nodes, np can only have
     kids if it is a sign+mod in which case the sign has already been
     through gdl_unlegacy */
  if (np->kids && np->kids->props && prop_find_kv(np->kids->props, "legacy", NULL))
    {
      np->text = np->kids->text;
      return;
    }

  if ((res = gdl_unlegacy_str(np->mloc, (uccp)np->text)))
    {
      if (strcmp(np->text, (ccp)res))
	{
	  prop_node_add(np, GP_TRACKING, PG_GDL_INFO, "legacy", np->text);
	  np->text = (ccp)pool_copy(res, np->tree->tm->pool);
	}
    }
  else
    mesg_verr(np->mloc, "gdl_unlegacy failed to convert %s\n", np->text);
}

int
gdl_legacy_check(Node *ynp, unsigned const char *t)
{
  int need_legacy = 0;

  if (gdl_legacy)
    return 0;
  
  if (ynp == NULL && t == NULL && legacy_reported_h)
    {
      hash_free(legacy_reported_h, NULL);
      legacy_reported_h = NULL;
      return 0;
    }

  if (!legacy_reported_h)
    legacy_reported_h = hash_create(1);
  
  if (curr_pqx)
    {
      if (!hash_find(legacy_reported_h, (uccp)curr_pqx))
	{
#if 0
	  unsigned char *res = gdl_unlegacy_str(ynp->mloc, t);
	  if (strcmp((ccp)res, (ccp)t))
	    {
#endif
	      Mloc m = *ynp->mloc;
	      m.line = curr_pqx_line;
	      mesg_verr(&m, "Text %s needs '#atf: use legacy'", curr_pqx);
	      hash_add(legacy_reported_h, (uccp)curr_pqx, "");
	      need_legacy = 1;
#if 0
	    }
#endif
	}
      else
	need_legacy = 1;
    }
  return need_legacy;
}

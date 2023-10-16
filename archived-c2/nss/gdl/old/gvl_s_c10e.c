#include <stdlib.h>
#include <wctype.h>
#include <memo.h>
#include <pool.h>
#include <oraccsys.h>
#include "gvl.h"
#include "unidef.h"

#if 0
extern wchar_t subdig_of(wchar_t w);
extern wchar_t vowel_of(wchar_t w);
#endif

#define G_C10E_MIXED_CASE 0x02
#define G_C10E_FINAL_SUBX 0x04

unsigned char *
gvl_s_c10e(const unsigned char *g, int *err)
{
  wchar_t *w;
  size_t len;
  int i;
  static int errx;
  int suppress_case_check = 0;

  if (NULL == err)
    err = &errx;

  if ((w = utf2wcs(g, &len)))
    {
      wchar_t *x = malloc(3*len*sizeof(wchar_t));
      wchar_t cued_sub_23 = 0;
      size_t xlen = 0;
      int found_l = 0;
      int found_u = 0;
      unsigned char *ret;
      
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
	    case '#':
	    case '?':
	    case '!':
	    case '[':
	    case ']':
	    case U_ulhsq:
	    case U_urhsq:
	    case U_llhsq:
	    case U_lrhsq:
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
	      cued_sub_23 = subdig_of(w[i]);
	      x[xlen++] = vowel_of(w[i]);
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
	      cued_sub_23 = subdig_of(w[i]);
	      x[xlen++] = vowel_of(w[i]);
	      ++found_u;
	      break;
	    case U_s_x:
	      /* This block may be unnecessary with GVL */
	      x[xlen++] = w[i];
	      if ('(' != w[i+1])
		*err |= G_C10E_FINAL_SUBX;
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
	  *err |= G_C10E_MIXED_CASE;
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
      return ret;
    }

  return NULL;
}

unsigned char *
base_of(const unsigned char *v)
{
  if (v)
    {
      unsigned char *b = NULL, *sub = NULL, *ret;
      
      b = malloc(strlen((ccp)v)+1);
      strcpy((char*)b, (ccp)v);
      if (strlen((ccp)v) > 4)
	{
	  sub = b + strlen((ccp)b);
	  while (1)
	    {
	      if ('\0' == *sub && sub - 3 > b && sub[-3] == 0xe2 && sub[-2] == 0x82)
		{
		  if ((sub[-1] >= 0x80 && sub[-1] <= 0x89) || sub[-1] == 0x93)
		    {
		      sub -= 3;
		      *sub = '\0';
		    }
		}
	      else
		break;
	    }
	}
      ret = g_lc(b);
      free(b);
      return ret;
    }
  return NULL;
}

const char *
sub_of(int i)
{
  switch (i)
    {
    case 1:
      return "₁";
    case 2:
      return "₂";
    case 3:
      return "₃";
    case 4:
      return "₄";
    case 5:
      return "₅";
    case 6:
      return "₆";
    case 7:
      return "₇";
    case 8:
      return "₈";
    case 9:
      return "₉";
    case 0:
      return "₀";
    }
  return NULL;
}

wchar_t
subdig_of(wchar_t w)
{
  switch (w)
    {
    case a_acute:
    case e_acute:
    case i_acute:
    case u_acute:
    case A_acute:
    case E_acute:
    case I_acute:
    case U_acute:
      return U_s2;
    case a_grave:
    case e_grave:
    case i_grave:
    case u_grave:
    case A_grave:
    case E_grave:
    case I_grave:
    case U_grave:
      return U_s3;
    }
  return w;
}

wchar_t
vowel_of(wchar_t w)
{
  switch (w)
    {
    case A_acute:
    case A_grave:
      return (wchar_t)'A';
    case E_acute:
    case E_grave:
      return (wchar_t)'E';
    case I_acute:
    case I_grave:
      return (wchar_t)'I';
    case U_acute:
    case U_grave:
      return (wchar_t)'U';
    case a_acute:
    case a_grave:
      return (wchar_t)'a';
    case e_acute:
    case e_grave:
      return (wchar_t)'e';
    case i_acute:
    case i_grave:
      return (wchar_t)'i';
    case u_acute:
    case u_grave:
      return (wchar_t)'u';
    }
  return w;
}

wchar_t *
g_wlc(wchar_t *w)
{
  wchar_t *w_end = w;

  while (*w_end && *w_end != '~' && *w_end != '\\'
	 && (*w_end != '@' || iswupper(w_end[1]))
	 && (*w_end < U_s0 || *w_end > U_s9)
	 && *w_end != U_s_x
	 )
    {
      *w_end = towlower(*w_end);
      ++w_end;
    }

  return w;
}

wchar_t *
g_wuc(wchar_t *w)
{
  wchar_t *w_end = w;

  while (*w_end && *w_end != '~' && *w_end != '\\'
	 && (*w_end != '@' || iswupper(w_end[1]))
	 && (*w_end < U_s0 || *w_end > U_s9)
	 && *w_end != U_s_x
	 )
    {
      *w_end = towupper(*w_end);
      ++w_end;
    }

  return w;
}

unsigned char *
g_lc(unsigned const char *g)
{
  wchar_t *w;
  size_t len;  
  if ((w = utf2wcs(g, &len)))
    {
      w = g_wlc(w);
      return wcs2utf(w,len);
    }
  return NULL;
}

unsigned char *
g_uc(unsigned const char *g)
{
  wchar_t *w;
  size_t len;  
  if ((w = utf2wcs(g, &len)))
    {
      w = g_wuc(w);
      return wcs2utf(w,len);
    }
  return NULL;
}

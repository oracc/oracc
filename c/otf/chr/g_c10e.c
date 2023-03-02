#include <stdlib.h>
#include <wctype.h>
#include "atf.h"

extern wchar_t subdig_of(wchar_t w);
extern wchar_t vowel_of(wchar_t w);

#define G_C10E_MIXED_CASE 0x02
#define G_C10E_FINAL_SUBX 0x04

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

unsigned char *
g_c10e(const unsigned char *g, int *err)
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
	    case 0xd7:
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

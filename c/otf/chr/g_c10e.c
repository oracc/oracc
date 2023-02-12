#include <stdlib.h>
#include <wctype.h>
#include "atf.h"

extern wchar_t subdig_of(wchar_t w);
extern wchar_t vowel_of(wchar_t w);


unsigned char *
g_c10e(const unsigned char *g)
{
  wchar_t *w;
  size_t len;
  int i;

  if ((w = utf2wcs(g, &len)))
    {
      wchar_t *x = malloc(3*len*sizeof(wchar_t));
      wchar_t cued_sub_23 = 0;
      size_t xlen = 0;
      for (i = 0; i < len; ++i)
	{
	  switch (w[i])
	    {
	    case '.':
	    case '-':
	    case ':':
	    case '+':
	    case '&':
	    case '%':
	    case '@':
	    case '(':
	    case ')':
	    case '|':
	      if (cued_sub_23)
		{
		  x[xlen++] = cued_sub_23;
		  cued_sub_23 = 0;
		}
	      x[xlen++] = w[i];
	      break;
	    case '[':
	    case ']':
	    case '<':
	    case '>':
	    case U_ulhsq:
	    case U_urhsq:
	    case U_llhsq:
	    case U_lrhsq:
	    case U_cdot:
	    case U_degree:
	      /* delete these */
	      break;
	    case U_heth:
	      x[xlen++] = 'h';
	    case U_HETH:
	      x[xlen++] = 'H';
	      break;
	    case '\'':
	      x[xlen++] = U_aleph;
	      break;
	    case a_acute:
	    case e_acute:
	    case i_acute:
	    case u_acute:
	    case A_acute:
	    case E_acute:
	    case I_acute:
	    case U_acute:
	    case a_grave:
	    case e_grave:
	    case i_grave:
	    case u_grave:
	    case A_grave:
	    case E_grave:
	    case I_grave:
	    case U_grave:
	      cued_sub_23 = subdig_of(w[i]);
	      x[xlen++] = vowel_of(w[i]);
	      break;
	    default:
	      x[xlen++] = w[i];
	      break;
	    }
	}
      x[xlen] = 0;
      return wcs2utf(x,xlen);
    }

  return NULL;
}

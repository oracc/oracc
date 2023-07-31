#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <oraccsys.h>
#include <unidef.h>
#include <gutil.h>

/****************************** GRAPHEME MANIPULATION UTILITY ROUTINES **********************************/

unsigned char *
g_base_of(const unsigned char *v)
{
  if (v)
    {
      unsigned char *b = NULL, *sub = NULL, *ret;
      
      b = malloc(strlen((ccp)v)+1);
      strcpy((char*)b, (ccp)v);
      if (strlen((ccp)v) > 3)
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

int
g_index_of(const unsigned char *g, const unsigned char *b)
{
  int i = 0;
  if (!b)
    b = g_base_of(g);
  g += strlen((ccp)b);
  if (g[0])
    {
      if (strlen((ccp)g) == 6)
	{
	  i = ((g[2] - 0x80)) * 10;
	  g += 3;
	}
      i += (g[2] - 0x80);
      if (i == 19) /* subscript x char 3 = 0x93 - 0x80 = 0x13 = 19 */
	i = 1000;
    }
  else
    i = 1;
  return i;
}

const char *
g_sub_of(int i)
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
g_subdig_of(wchar_t w)
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
g_vowel_of(wchar_t w)
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

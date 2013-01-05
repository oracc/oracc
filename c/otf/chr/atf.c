#include <locale.h>
#include "atf.h"
void
atf_init(void)
{
  setlocale(LC_ALL,LOCALE /*"en_GB.utf8"*/);
  atf2utf_init();
}

void
atf_term(void)
{
  utf2wcs(NULL,0);
  wcs2atf(NULL,0);
  wcs2utf(NULL,0);
}

const unsigned char *
atf_strip_base_chars(const unsigned char *u)
{
  wchar_t *w = NULL;
  size_t n = 0;
  w = utf2wcs(u,&n);
  if (w)
    {
      wchar_t *w2 = malloc(sizeof(wchar_t) * (strlen((char*)u)+1));
      int i, dest;
      for (dest = i = 0; i < n; ++i)
	{
	  if (w[i] != U_cdot && w[i] != U_degree)
	    w2[dest++] = w[i];
	}
      if (dest < n)
	{
	  unsigned char *n_u;
	  w2[dest] = 0;
	  n_u = wcs2utf(w2,dest);
	  free(w2);
	  return n_u;
	}
      else
	{
	  free(w2);
	  return u;
	}
    }
  else
    return NULL;
}

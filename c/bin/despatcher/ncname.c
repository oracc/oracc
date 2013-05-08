#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <ctype128.h>

static wchar_t utf1char(const unsigned char *src,size_t *len);

static int
_is_ncname_char(const char *start, size_t *len)
{
  wchar_t w = utf1char(start, len);
  return iswalnum(w);
}

static int
_is_ncname_letter(const char *start, size_t *len)
{
  wchar_t w = utf1char(start, len);
  return iswalpha(w);
}

int
_is_ncname(const char *name)
{
  const char *np = name;
  static size_t used = 0;

  if (np)
    {
      if ('_' == *np || isalpha(*np))
	++np;
      else if (*(unsigned char*)np > 127 && _is_ncname_letter(np, &used))
	np += used;
      else
	return 0;

      while (*np)
	{
	  if (isalnum(*np) || '_' == *np || '-' == *np || '.' == *np)
	    ++np;
	  else if (*(unsigned char*)np > 127 && _is_ncname_char(np, &used))
	    np += used;
	  else
	    return 0;
	}
    }
  return 1;
}

static wchar_t
utf1char(const unsigned char *src,size_t *len)
{
  static wchar_t wc = 0;
  int ret = 0;
  size_t todo = 0;
  const unsigned char *s = src;
  while (*s > 127)
    ++s;
  todo = s - src;
  if (todo)
    {
      ret = mbtowc(&wc,(const char *)src,todo);
      if (ret < 0)
	{
	  int i;
	  fprintf(stderr,"oracc-resolver: bad utf8 in NCName: mbtowc failed on:");
	  for (i = 0; i < todo; ++i)
	    fprintf(stderr," %x",src[i]);
	  fprintf(stderr,"\n");
	  if (len)
	    *len = (size_t)-1;
	  return 0;
	}
      else
	{
	  if (len)
	    *len = ret;
	  return wc;
	}
    }
  else
    {
      if (len)
	*len = 0;
      return 0;
    }
}

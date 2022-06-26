#include <ctype128.h>
#include "gx.h"

unsigned char *
tok(unsigned char *s, unsigned char *end)
{
  while (isspace(*s))
    ++s;
  if (*s)
    {
      unsigned char *e = s;
      while (!isspace(*e))
	++e;
      *end = *e;
      if (*e)
	*end = '\0';
    }
  else
    *end = '\0';     
  return s;
}

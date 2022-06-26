#include <ctype128.h>
#include "gx.h"

unsigned char *
tok(unsigned char *s, unsigned char *end)
{
  while (isspace(*s))
    ++s;
  if (*s)
    {
      while (!isspace(*s))
	++s;
    }
  *end = *s;
  return s;
}

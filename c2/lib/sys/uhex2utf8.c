#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <oraccsys.h>

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 *
 * (from an uncopyrighted post on StackOverflow by user radhoo, 20160820)
 */
uint32_t
hex2int(char *hex)
{
  uint32_t val = 0;
  while (*hex)
    {
      /* get current character then increment */
      uint8_t byte = *hex++; 
      /* transform hex character to the 4bit equivalent number, using the ascii table indexes */
      if (byte >= '0' && byte <= '9') byte = byte - '0';
      else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
      else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
      /* shift 4 to make space for new digit, and add the 4 bits of the new digit */
      val = (val << 4) | (byte & 0xF);
    }
  return val;
}

/**
 * ucode2utf8
 * take a Unicode sequence expressed in hex and convert to UTF8
 * e.g.,
 *
 * x12000 =>
 *
 * U+12000.U+12000 => 
 */
unsigned char *
uhex2utf8(unsigned const char *uhex)
{
  int nchars = 1, last = 0, nhw = 0;
  wchar_t *wp = NULL;
  unsigned char *utmp = (unsigned char *)strdup((const char *)uhex), *s, *t;
  for (s = utmp; *s; ++s)
    if ('.' == *s)
      ++nchars;
  wp = malloc((nchars+1) * sizeof(wchar_t));
  for (s = utmp; *s; ++s)
    {
      t = s;

      while (*s && '.' != *s)
	++s;
      if (*s)
	*s = '\0';
      else
	last = 1;

      if ('0'==t[0] && 'x'==t[1])
	t += 2;
      else if ('x' == t[0])
	++t;
      else if ('U' == t[0] && '+' == t[1])
	t += 2;

      if ('X' == *t)
	wp[nhw++] = L'X';
      else
	wp[nhw++] = (wchar_t)hex2int((char*)t);

      if (last)
	break;
    }
  wp[nhw] = L'\0';
  s = wcs2utf(wp,nhw);
  free(wp);
  free(utmp);
  return s;
}

#if 0
#include <oracclocale.h>
int main(int argc, char **argv)
{
  unsigned const char *c;
  if (olocale())
    {
      fprintf(stderr, "failed to set locale. Stop\n");
      exit(1);
    }
  c = uhex2utf8((unsigned char*)"x12000");
  fprintf(stderr, "%s\n", c);
  c = uhex2utf8((unsigned char*)"x12000.x12000");
  fprintf(stderr, "%s\n", c);
  c = uhex2utf8((unsigned char*)"U+12000.U+12000");
  fprintf(stderr, "%s\n", c);  
}
#endif

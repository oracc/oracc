#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <roco.h>

#define ccp const char *
#define uccp unsigned const char *

/**roco_printf -- print cells from a row according to a format
   template.
   
   The format template consists of %DIGITS; % is escaped as %%.

   If DIGITS begins with '0' then the leading zeroes indicate padding
   to the number of leading zeroes.

   The DIGITS following leading zeroes are used as an index into the Roco.

   E.g., "<esp:link page="%1">%3</esp:link>".

 */
static void
pad(FILE *fp, int col, int num)
{
  if (num < UINT16_MAX)
    {
      char i[8];
      sprintf(i, "%d", num);
      int pad = col - strlen(i);
      while (pad-- > 0)
	fputc('0',fp);
    }
}
void
roco_row_format(FILE*fp, const unsigned char **row)
{
  const char *s = roco_format;
  while (*s)
    {
      if ('%' == *s)
        {
	  if ('%' == s[1])
	    {
	      fputc('%', fp);
	      s += 2;
	    }
	  else
	    {
	      ++s;
	      if (isdigit(*s))
		{
		  int zeroes = 0;
		  int colnum;
		  while ('0' == *s)
		    {
		      ++zeroes;
		      ++s;
		    }
		  char *e;
		  colnum = (int)strtol(s, &e, 10);
		  /* FIXME: initial implementation without
		     range-checking; r-c requires mods to Roco
		     structure */
		  if (zeroes)
		    {
		      long colval = strtoul((ccp)row[colnum], NULL, 10);
		      if (colval >= 0)
			pad(fp, zeroes, colval);
		      fputs((ccp)row[colnum-1], fp);
		    }
		  else
		    fputs((ccp)row[colnum-1], fp);
		}
	      else
		{
		  fprintf(stderr, "bad character after percent sign: must be digit or percent sign\n");
		}
	      ++s;
	    }
	}
      else
	{
	  fputc(*s, fp);
	  ++s;
	}
    }
  if (roco_newline)
    fputc('\n', fp);
}

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
  register int c, lastc = -1;
  while (EOF != (c = getchar()))
    {
      if ('>' == c)
	{
	  if ('/' == lastc)
	    {
	      putchar('\n');
	      putchar(lastc);
	      putchar(c);
	      lastc = -1;
	    }
	  else if ('?' != lastc)
	    {
	      putchar(lastc);
	      putchar('\n');
	      putchar(c);
	      lastc = -1;
	    }
	  else
	    {
	      putchar(lastc);
	      putchar(c);
	      lastc = -1;
	    }
	}
      else
	{
	  if (lastc >= 0)
	    putchar(lastc);
	  lastc = c;
	}
    }
  return 0;
}

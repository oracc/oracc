#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gdl.h"

/* test harness for gvl/gdl libraries */

int
main(int argc, char **argv)
{
  if (argv[1])
    {
      gdlparse_init();
      if (!strcmp(argv[1], "-s"))
	{
	  char buf[1024], *s;	  
	  while ((s = fgets(buf, 1024, stdin)))
	    {
	      gdlparse_string(s);
	      gdlparse_reset();
	    }
	}
      else
	gdlparse_string(argv[1]);
      gdlparse_term();
    }
  else
    {
      fprintf(stderr, "gvlx: give grapheme on command line\n");
    }
  return 0;
}

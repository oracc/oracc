#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gdl.h"
#include "gvl.h"

/* test harness for gvl/gdl libraries */

int
main(int argc, char **argv)
{
  gvl_setup("ogsl", "ogsl");

  if (argv[1])
    {
      gdlparse_init();
      if (!strcmp(argv[1], "-s"))
	{
	  char buf[1024], *s;	  
	  while ((s = fgets(buf, 1024, stdin)))
	    {
	      Tree *tp = gdlparse_string(s);
	      gdl_xml(stdout, tp);
	      gdlparse_reset();
	    }
	}
      else
	{
	  Tree *tp = gdlparse_string(argv[1]);
	  gdl_xml(stdout, tp);
	}
      gdlparse_term();
    }
  else
    {
      fprintf(stderr, "gvlx: give grapheme on command line\n");
    }

  return 0;
}

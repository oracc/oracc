#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>

#include "gdl.h"
#include "gvl.h"

/* test harness for gvl/gdl libraries */

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

int check_mode = 0;

int
main(int argc, char **argv)
{
  mesg_init();
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");

  options(argc, argv, "c");

  gdlparse_init();
  if (check_mode)
    {
      char buf[1024], *s;	  
      while ((s = fgets(buf, 1024, stdin)))
	{
	  Tree *tp = NULL;
	  if (s[strlen(s)-1] == '\n')
	    s[strlen(s)-1] = '\0';
	  tp = gdlparse_string(s);
	  mesg_print(stdout);
	  gdlparse_reset();
	  tree_term(tp);
	}
    }
  else if (argv[optind])
    {
      Tree *tp = gdlparse_string(argv[1]);
      mesg_print(stderr);
      tree_xml(stdout, tp);
    }
  else
    {
      help();
    }
  gdlparse_term();
      
  return 0;
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'c':
      check_mode = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  fprintf(stderr, "gdlx: give grapheme on command line or use -c and read lines from stdin\n");
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <tree.h>

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
int identity_mode = 0;

static void
test_identity(char *s, Tree *t)
{
  const char *res = NULL;
  if (t->root && t->root->kids)
    {
      if (t->root->kids->text)
	res = t->root->kids->text;
      else
	printf("%s\t(null)\n", s);
    }

  if (res && strcmp(s, res))
    printf("%s\t%s\n", s, res);
}

int
main(int argc, char **argv)
{
  static Mloc ml;
  mesg_init();
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");

  options(argc, argv, "ci");
  
  gdlparse_init();
  if (check_mode || identity_mode)
    {
      char buf[1024], *s;	  
      ml.file = "<stdin>";
      ml.line = 0;
      while ((s = fgets(buf, 1024, stdin)))
	{
	  Tree *tp = NULL;
	  if (s[strlen(s)-1] == '\n')
	    s[strlen(s)-1] = '\0';
	  ++ml.line;
	  tp = gdlparse_string(&ml, s);
	  if (check_mode)
	    mesg_print(stdout);
	  else
	    test_identity(s, tp);
	  gdlparse_reset();
	  tree_term(tp);
	}
    }
  else if (argv[optind])
    {
      Tree *tp = NULL;
      ml.file = "<argv1>";
      ml.line = 1;
      tp = gdlparse_string(&ml, argv[1]);
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
    case 'i':
      identity_mode = 1;
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
  fprintf(stderr, "gdlx: give grapheme on command line or use -c / -i and read lines from stdin\n");
}

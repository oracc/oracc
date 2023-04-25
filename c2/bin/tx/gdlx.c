#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <tree.h>

#include "gdl.h"
#include "gvl.h"

/* test harness for gvl/gdl libraries */

static Mloc ml;

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int gdl_flex_debug, gdldebug;

int check_mode = 0;
const char *fname = NULL;
int identity_mode = 0;
int trace_mode = 0;
int wrapper = 0;

FILE *fp;

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

void
do_one(char *s)
{
  Tree *tp = NULL;
  List *mp = NULL;
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  ++ml.line;
  mesg_init();
  tp = gdlparse_string(&ml, s);
  
  mp = mesg_retrieve();
  if (mp && list_len(mp))
    {
      List *tmp = list_create(LIST_SINGLE);
      unsigned const char *msg = NULL;
      for (msg = list_first(mp); msg; msg = list_next(mp))
	if (!strstr(msg, "qualified"))
	  list_add(tmp, msg);
      if (list_len(tmp))
	mesg_print2(stderr, tmp);
    }

  if (identity_mode)
    test_identity(s, tp);
  else if (!check_mode)
    tree_xml(stdout, tp);
  gdlparse_reset();
  tree_term(tp);
}

void
do_many(const char *fn)
{
  if ('-' == *fn)
    {
      fp = stdin;
      ml.file = "<stdin>";
      ml.line = 1;
    }
  else
    fp = fopen(fn, "r");
  if (fp)
    {
      char buf[1024], *s;
      ml.file = fn;
      ml.line = 1;
      if (wrapper)
	printf("<gdlx>");
      while ((s = fgets(buf, 1024, fp)))
	do_one(s);
      if (wrapper)
	printf("</gdlx>");
    }
  else
    fprintf(stderr, "gdlx: file %s can't be read\n", fn);
}

int
main(int argc, char **argv)
{
  gdl_flex_debug = gdldebug = 0;
  
  options(argc, argv, "cf:itw");

  gdl_flex_debug = gdldebug = trace_mode;
  
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");
  gdlparse_init();

  if (argv[optind])
    {
      ml.file = "<argv1>";
      ml.line = 1;
      do_one(argv[optind]);
    }
  else if (fname)
    do_many(fname);
  else
    do_many("-");

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
    case 'f':
      fname = optarg;
      break;
    case 'i':
      identity_mode = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'w':
      wrapper = 1;
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <tree.h>
#include <xml.h>
#include <xnn.h>
#include <ns-gdl.h>
#include <gdl.h>
#include <gvl.h>

/* Tool to produce gdl signatures from transliteration */

static Mloc ml;

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int gdl_flex_debug, gdldebug, gdl_orig_mode;

int backward = 0;
int check_mode = 0;
extern int deep_sig;
int error_stdout = 0;
const char *fname = NULL;
int gdl_c10e_mode = 1;
int identity_mode = 0;
int ns_output = 0;
int pedantic = 0;
int sortsigs = 0;
int tabbed = 0;
int validate = 0;
int wrapper = 0;

FILE *fp;

void
do_one(char *s)
{
  Tree *tp = NULL;
  List *mp = NULL;
  static int saved_deep = -1;

  if (-1 == saved_deep)
    saved_deep = deep_sig;
  
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  if (s[strlen(s)-1] == '+')
    {
      s[strlen(s)-1] = '\0';
      saved_deep = deep_sig;
      deep_sig = 1;
    }
  ++ml.line;
  mesg_init();
  tp = gdlparse_string(&ml, s);
  
  mp = mesg_retrieve();
  if (mp && list_len(mp))
    {
      if (error_stdout)
	{
	  mesg_print2(stdout, mp);
	  fflush(stdout);
	}
      else
	mesg_print2(stderr, mp);
    }

  const char *sig = gdlsig(tp);
  if (backward)
    fprintf(stdout, "%s => %s\n", s, sig); /* tlitsig.plx format */
  else if (tabbed)
    fprintf(stdout, "%s\t%s\n", s, sig);
  else
    fprintf(stdout, "%s\n", sig);
  fflush(stdout);
  gdlparse_reset();
  deep_sig = saved_deep;
  /*tree_term(tp);*/ /* don't do this per-tree any more */
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
      while ((s = fgets(buf, 1024, fp)))
	{
	  if (s)
	    {
	      if (0x04 == *s) /* exit on ^D */
		exit(0);
	      else
		do_one(s);
	    }
	  else
	    exit(1);
	}
    }
  else
    fprintf(stderr, "tlitsig: file %s can't be read\n", fn);
}

static void
helper_mode(void)
{
  error_stdout = 1;
  mesg_no_loc = 1;
  mesg_prefix(">>");
  gvl_sans_report = 1;
}

int
main(int argc, char **argv)
{
  gdl_flex_debug = gdldebug = 0;
  
  options(argc, argv, "bcdf:hinoprstvw");

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
    {
      helper_mode();
      do_many("-");
    }

  gdlparse_term();
  return 0;
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'b':
      backward = 1; /* use ba => o0000113 output format */
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'd':
      deep_sig = 1;
      break;
    case 'f':
      fname = optarg;
      break;
    case 'h':
      helper_mode();
      break;
    case 'i':
      identity_mode = 1;
      break;
    case 'n':
      ns_output = 1;
      break;
    case 'o':
      gdl_orig_mode = 1;
      break;
    case 'p':
      gvl_strict = pedantic = 1;
      break;
    case 'r':
      gvl_sans_report = 1;
      break;
    case 's':
      sortsigs = 1;
      break;
    case 't':
      tabbed = 1;
      break;
    case 'u': /* uptranslate to canonicalized version */
      gdl_c10e_mode = 1;
      break;
    case 'v':
      validate = 1;
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
  fprintf(stderr, "tlitsig: give grapheme on command line or use -c / -i and read lines from stdin\n");
}

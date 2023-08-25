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
#include <gsort.h>

/* test harness for gvl/gdl libraries */

static Mloc ml;

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int rnvtrace;

extern int gdl_flex_debug, gdldebug, gdl_orig_mode;

int bare_mode = 0;
int check_mode = 0;
extern int gdlsig_depth_mode;
int error_stdout = 0;
const char *fname = NULL;
int gdl_c10e_mode = 1;
int gsort_mode = 0;
int identity_mode = 0;
int ns_output = 0;
int pedantic = 0;
int signatures = 0;
int trace_mode = 0;
int validate = 0;
int wrapper = 0;

FILE *fp;
List *gslp;

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
  static int saved_deep = -1;

  if (-1 == saved_deep)
    saved_deep = gdlsig_depth_mode;
  
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  if (s[strlen(s)-1] == '+')
    {
      s[strlen(s)-1] = '\0';
      saved_deep = gdlsig_depth_mode;
      gdlsig_depth_mode = 1;
    }
  ++ml.line;
  mesg_init();
  tp = gdlparse_string(&ml, s);
  
  mp = mesg_retrieve();
  if (mp && list_len(mp))
    {
      List *tmp = list_create(LIST_SINGLE);
      unsigned const char *msg = NULL;
      for (msg = list_first(mp); msg; msg = list_next(mp))
	if (pedantic || (!strstr((ccp)msg, "qualified") && !strstr((ccp)msg, "unknown")))
	  list_add(tmp, (void*)msg);
      if (list_len(tmp))
	{
	  if (error_stdout)
	    {
	      mesg_print2(stdout, tmp);
	      fflush(stdout);
	    }
	  else
	    mesg_print2(stderr, tmp);
	}
    }

  if (identity_mode)
    test_identity(s, tp);
  else if (gsort_mode)
    {
      GS_head *ghp;
      list_add(gslp, (ghp = gsort_prep(tp)));
      gsort_show(ghp);
    }
  else if (!check_mode)
    {
      if (signatures)
	{
	  const char *sig = gdlsig(tp);
	  if (bare_mode)
	    fprintf(stdout, "%s\n", sig);
	  else
	    fprintf(stdout, "%s\t%s\n", s, sig);
	  fflush(stdout);
	}
      else if (ns_output)
	tree_xml_rnv(stdout, tp, &gdl_data, "gdl");
      else
	tree_xml(stdout, tp);
    }
  gdlparse_reset();
  gdlsig_depth_mode = saved_deep;
  /*tree_term(tp);*/
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
  
  options(argc, argv, "bcdef:gilnopstvw");

  gdl_flex_debug = gdldebug = trace_mode;
  
  gdlxml_setup();
  gvl_setup("ogsl", "ogsl");
  gdlparse_init();

  if (gsort_mode)
    {
      gsort_init();
      gslp = list_create(LIST_SINGLE);
    }
  
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

  if (gsort_mode)
    {
      GS_head ** ghp = (GS_head**)list2array(gslp);
      int n = list_len(gslp), i;
      qsort(ghp, n, sizeof(GS_head*), gsort_cmp);
      for (i = 0; i < n; ++i)
	fprintf(stdout, "%s\n", ghp[i]->s);
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
      bare_mode = 1;
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'd':
      gdlsig_depth_mode = 1;
      break;
    case 'e':
      error_stdout = 1;
      mesg_prefix(">>");
      gvl_sans_report = 1;
      break;
    case 'f':
      fname = optarg;
      break;
    case 'g':
      gsort_mode = 1;
      break;
    case 'i':
      identity_mode = 1;
      break;
    case 'l':
      gdl_legacy = 1;
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
    case 's':
      signatures = 1;
      break;
    case 't':
      trace_mode = 1;
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
  fprintf(stderr, "gdlx: give grapheme on command line or use -c / -i and read lines from stdin\n");
}

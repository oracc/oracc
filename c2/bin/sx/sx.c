#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <asl.h>
#include <gdl.h>
#include <oraccsys.h>
#include <oracclocale.h>

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int asltrace,rnvtrace;

extern int asl_raw_tokens; /* ask asl to produce list of @sign/@form/@v tokens */

int check_mode = 0;
int trace_mode = 0;
extern int asl_flex_debug;

int
main(int argc, char * const*argv)
{
  Tree *tp = NULL;
  const char *l = NULL;

  asl_flex_debug = 0;
  
  if (!(l = setlocale(LC_ALL,ORACC_LOCALE)))
    if (!(l = setlocale(LC_ALL, "en_US.UTF-8")))
      if (!(l = setlocale(LC_ALL, "C")))
        fprintf(stderr, "gvl_setup: failed to setlocale to '%s', 'UTF-8', or 'C'\n", ORACC_LOCALE);
  
  options(argc, argv, "crt");
  fprintf(stderr, "sx trace_mode = %d\n", trace_mode);
  aslflextrace = asltrace = asl_flex_debug = trace_mode;

  mesg_init();
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  asl_init();
  tp = aslyacc();
  mesg_print(stderr);
  if (tp)
    tree_xml(NULL, tp);
  asl_term();
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'c':
      check_mode = 1;
      break;
    case 'r':
      asl_raw_tokens = 1;
      break;
    case 't':
      trace_mode = 1;
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
  fprintf(stderr, "sx: read input from stdin; use -c to check only\n");
}

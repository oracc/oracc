#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <../../nss/asl/asl.h>
#include <gdl.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include "sx.h"

Mloc *xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int asltrace,rnvtrace;

int asl_output = 0;
int tree_output = 0;
int xml_output = 0;

extern int asl_raw_tokens; /* ask asl to produce list of @sign/@form/@v tokens */

int check_mode = 0;
int trace_mode = 0;
extern int asl_flex_debug, gdl_flex_debug;

int
main(int argc, char * const*argv)
{
  struct sl_signlist *sl;

  xo_loc = malloc(sizeof(Mloc));
  mesg_init();
  asl_flex_debug = gdl_flex_debug = 0;

  /* Initialize a dummy gvl with an empty hash instead of a signlist */
  (void)gvl_setup(NULL, NULL);
  
  options(argc, argv, "acrtTx");
  asltrace = asl_flex_debug = trace_mode;

  if (argv[optind])
    {
      file = argv[optind];
      if (!freopen(file, "r", stdin))
	{
	  fprintf(stderr, "sx: unable to read from %s\n", file);
	  exit(1);
	}
    }
  
  mesg_init();
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  gdl_init();
  asl_init();
  sl = aslyacc(file);
  mesg_print(stderr);

  if (sl)
    {
      sx_marshall(sl);
      
      if (asl_output)
	{
	  struct sl_functions *f = sx_asl_init(stdout, "-");
	  f->sll(f,sl);
	}

      if (xml_output)
	{
#if 0
	  struct sl_functions *f = sx_asl_init(stdout, "-");
	  f->sll(f, sl);
#else
	  sx_xml(sl);
#endif
	}
    }

  gdl_term();
  asl_term();
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'a':
      asl_output = 1;
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'r':
      asl_raw_tokens = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'T':
      tree_output = 1;
      break;
    case 'x':
      xml_output = 1;
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

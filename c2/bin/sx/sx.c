#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include <tree.h>
#include <xml.h>
#include <asl.h>
#include <gdl.h>
#include <oraccsys.h>

Mloc xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int status;
int asltrace,rnvtrace;

int check_mode = 0;
int trace_mode = 0;

int
main(int argc, char * const*argv)
{
  Tree *tp = NULL;

  options(argc, argv, "ct");
  aslflextrace = asltrace = trace_mode;

  mesg_init();
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  asl_init();
  tp = aslyacc();
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

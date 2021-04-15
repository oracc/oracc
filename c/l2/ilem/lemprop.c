#include <stdio.h>
#include <psd_base.h>
#include "options.h"
#include "warning.h"
#include "ilem_props.h"

extern int ilem_props_status;
extern int ilem_props_verbose;
int check = 0;
int test = 0;

int
main(int argc, char **argv)
{
  warning_init();
  options(argc,argv,"ctv");
  ilem_props_init();
  if (check)
    {
      if (ilem_props_status)
	fprintf(stderr, "errors in lemprops file\n");
      else
	fprintf(stderr, "lemprops file syntax OK\n");
    }
}

const char *prog = "prop";
const char *usage_string = "prop -cvt";
int major_version = 1;
int minor_version = 0;
void
help(void)
{
  printf("\t-c = check 00lib/lemprops.txt\n\t-t = test lookup routine\n\t-v = verbose messages\n");
}
int
opts(int och,char *oarg)
{
  switch (och)
    {
    case 'c':
      check = 1;
      break;
    case 't':
      test = 1;
      break;
    case 'v':
      ilem_props_verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

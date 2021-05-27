#include <stdio.h>
#include <psd_base.h>
#include "options.h"
#include "warning.h"
#include "ilem_props.h"

const char *project;

extern int ilem_props_status;
extern int ilem_props_verbose;
int check = 0;
int test = 0;

void
testf(const char *t)
{
  struct keypair *kp = NULL;
  fprintf(stderr, "lemprop testing '%s'\n", t);
  kp = ilem_props_look((const unsigned char *)t);
  if (kp->key)
    fprintf(stderr, "%s returned key=%s; val=%s\n", t, kp->key, kp->val);
}

int
main(int argc, char **argv)
{
  warning_init();
  options(argc,argv,"ctvp:");
  if (!project)
    {
      fprintf(stderr, "must give project with -p option\n");
      exit(1);
    }
  ilem_props_init();
  if (check)
    {
      if (ilem_props_status)
	fprintf(stderr, "errors in lemprops file\n");
      else
	fprintf(stderr, "lemprops file syntax OK\n");
    }
  if (test)
    {
      testf("gender=f");
      testf("m");		/* do abbreviated propspecs work? */
      testf("gender=banana"); 	/* banana not a valid val */
      testf("sealer");
      testf("sealer=Dada"); 	/* sealer is boolean so no val */
      testf("seller");
      testf("seller=xyz"); 	/* seller is a value not a prop */
      testf("x"); 		/* x is an ambiguous val */
      testf("banana");		/* banana not valid prop or val */
      testf("prof=merchant");	/* prof has * val */
      testf("pro=xyz"); 	/* pro is not a property */
      testf("fatherOf=@1"); 	/* must take ref */
      testf("fatherOf=Dada"); 	/* must take ref */
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

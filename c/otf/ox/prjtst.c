#include <stdio.h>
#include "warning.h"
#include "xpd2.h"
#include "run.h"
#include "proj_context.h"

int saa_mode = 1;

void
showopts(unsigned char *key, void *val)
{
  fprintf(stderr,"%s = %s\n", key, (char*)val);
}

int
main(int argc, char **argv)
{
  struct run_context *rp = run_init();
  verbose = 1;
  if (!argv[1])
    {
      fputs("prj: must give project name as argument\n",stderr);
      return 1;
    }
  proj_init(rp,argv[1]);
  hash_exec2(rp->proj->xpd->opts,showopts);
  fprintf(stderr,"cbd-mode = %s\n", xpd_option(rp->proj->xpd,"cbd-mode"));
  run_term(rp);
  return 0;
}

const char *prog = "prj";
const char *usage_string = "prj [PROJECT]";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }

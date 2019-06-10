#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "warning.h"
#include "runexpat.h"

int optind = 0;

void
rn_sH(void *userData, const char *name, const char **atts)
{
  if (strcmp(name,"rp-wrap"))
    {
      fputs(name, stdout);
      exit(0);
    }
}

void
rn_eH(void *userData, const char *name)
{
}

void
root_node_from_file(const char *fname)
{
  char const *fnlist[2];
  fnlist[0] = fname;
  fnlist[1] = NULL;
  runexpat(i_list, fnlist, rn_sH, rn_eH);
}

int
main(int argc, char **argv)
{
  if (argv[1])
    root_node_from_file(argv[1]);
  return 0;
}

int verbose = 0;
const char *prog = "rootnode";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [XML_FILE]";
int opts(int argc, char *arg) {return 0;}
void help() {}

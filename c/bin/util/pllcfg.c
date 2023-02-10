#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "warning.h"
#include "runexpat.h"

extern int options(int, char**,const char*);
extern int optind;
const char *listname = NULL;

void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"list"))
    {
      const char *f = findAttr(atts, "file");
      if (f && *f)
	{
	  if (!strcmp(f,listname))
	    {
	      f = findAttr(atts, "cfg");
	      if (f && *f)
		{
		  fputs(f,stdout);
		  exit(0);
		}
	    }
	}
    }
}

void
eH(void *userData, const char *name)
{
}

int
main(int argc, char **argv)
{
  const char *fname[2];

  fname[0] = argv[1];
  fname[1] = NULL;
  listname = argv[2];
  
  runexpat(i_list, fname, sH, eH);
  return 1;
}

int verbose = 0;
const char *prog = "pllcfg";
int major_version = 1;
int minor_version = 0;
const char *usage_string = " [-c -h -H -s] [-p PROJECT] [XML_FILE] [XML_ID]";
int
opts(int argc, char *arg)
{
  return 0;
}
void help() {}

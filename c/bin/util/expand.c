#include <stdio.h>
#include <stdlib.h>
#include "../include/psd_base.h"
#include "../include/fname.h"

/*static char nameframe[_MAX_PATH], linebuf[_MAX_PATH], *line, *cp1,*cp2,*cp3;*/

#define _MAX_LINE 2048
static char *line, linebuf[_MAX_LINE];

int
main(int argc, char**argv)
{
  if (argc < 3)
    {
      fprintf(stderr,"expand: must give project and extension\n");
      exit(1);
    }
  while ((line = fgets(linebuf,_MAX_PATH,stdin)))
    {
      if (line[strlen(line)-1] == '\n')
	{
	  line[strlen(line)-1] = '\0';
	  puts(expand(argv[1],line,argv[2]));
	}
      else
	{
	  fprintf(stderr,"expand: input line too long\n");
	  exit(1);
	}
    }
  return 0;
}

const char *prog = "expand";
int major_version = 1, minor_version = 0;
const char *usage_string = "expand PROJECT EXTENSION";
void help () { }
int opts(int arg,char*str){ return 0; }

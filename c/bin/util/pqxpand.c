#include <stdio.h>
#include <stdlib.h>
#include "../include/psd_base.h"
#include "../include/fname.h"

#define _MAX_LINE 2048
static char *line, linebuf[_MAX_LINE];

int
main(int argc, char**argv)
{
  setvbuf(stdout, NULL, _IONBF, 0);
  if (argc == 3) 
    {
      puts(l2_expand(NULL,argv[2],argv[1]));
    }
  else if (argc != 2)
    {
      fprintf(stderr,"pqxpand: give extension as first argument, optional QID as second\n");
      exit(1);
    }
  else
    {
      while ((line = fgets(linebuf,_MAX_LINE,stdin)))
	{
	  if (line[strlen(line)-1] == '\n' || feof(stdin))
	    {
	      line[strlen(line)-1] = '\0';
	      if (strchr(line, ':'))
		{
		  char *at = strchr(line, '@');
		  if (at)
		    *at = '\0';
		  puts(l2_expand(NULL,line,argv[1]));
		}
	      else
		fprintf(stderr,"pqxpand only works on qualified IDs, ignoring %s\n", line);
	    }
	  else
	    {
	      fprintf(stderr,"expand: input line too long\n");
	      exit(1);
	    }
	}
    }
  return 0;
}

const char *prog = "pqxpand";
int major_version = 1, minor_version = 0;
const char *usage_string = "pqxpand EXTENSION";
void help () { }
int opts(int arg,char*str){ return 0; }

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <oraccsys.h>

void
options (int argc, char *const *argv, const char *optstr)
{
  int c;

  while (EOF != (c = getopt (argc, argv, optstr)))
    {
      if ('?' == c)
	{
	  fprintf(stderr, "%s: command line help:\n", argv[0]);
	  help();
	  exit(1);
	}
      else if (opts (c, optarg))
	{
	  fprintf(stderr, "%s: unrecognized option %c\n", argv[0], c);
	  help();
	  exit(1);
	}
    }
}

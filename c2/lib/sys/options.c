#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <oraccsys.h>

int banner_done = 0;

static const char *copyright_string = "(this program is in the public domain)";
static const char *prog = NULL;
static const char *usage_string = NULL;

static int major_version = 1;
static int minor_version = 0;
static int show_command_line = 1;

static char *const *_internal_argv = NULL;
static int _internal_argc = 0;

void
program_values(const char *name, int major, int minor, const char *usage, const char *copyright)
{
  prog = name;
  major_version = major;
  minor_version = minor;
  usage_string = usage;
  if (copyright)
    copyright_string = copyright;
}

void
options (int argc, char *const *argv, const char *optstr)
{
  int c;

  _internal_argc = argc;
  _internal_argv = argv;
  
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

void
banner ()
{
  if (banner_done)
    return;
  else
    banner_done = 1;
   
  fprintf(stderr, "This is %-10s %2d.%02d %50s", 
	   prog, major_version, minor_version, copyright_string);
  if (show_command_line)
    {
      int i;
      fprintf(stderr, "\nCommand line: %s", _internal_argv[0]);
      for (i = 1; i < _internal_argc; ++i)
	fprintf(stderr, " %s", _internal_argv[i]);
    }
  fprintf(stderr, "\n");
}

void
usage ()
{
  banner ();
  fprintf(stderr, "Usage:\n      %s %s\n\n", prog, usage_string);
  help ();
  exit (1);
}

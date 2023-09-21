#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <loadfile.h>
#include <roco.h>

#define REPS_BUF_MAX_ 4196

const char *cfg = NULL;
const char *infile = NULL;

int
main(int argc, char *const *argv)
{
  Roco *r = NULL;
  
  options(argc, argv, "c:i:?");

  if (cfg)
    {
      r = roco_load(cfg, 0, NULL, NULL, NULL);
      Hash *h = roco_hash(r);
      unsigned char *s = NULL;
      if (infile)	
	s = loadfile((uccp)infile, NULL);
      else
	s = loadstdin(NULL);
      strrep_f_h((ccp)s, stdout, h);
      free(s);
    }
  else
    {
      fprintf(stderr, "repx: must give configuration file on command-line. Stop.\n");
      exit(1);
    }
}

const char *prog = "rocox";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "";

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'c':
      cfg = arg;
      break;
    case 'i':
      infile = arg;
      break;
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void help (void)
{
}

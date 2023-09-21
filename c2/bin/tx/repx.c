#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oraccsys.h>
#include <pool.h>
#include <xpd.h>
#include <xsystem.h>
#include <loadfile.h>
#include <roco.h>

#define REPS_BUF_MAX_ 4196

const char *cfg = NULL;
const char *infile = NULL;
const char *outfile = NULL;
const char *project = NULL;

int
main(int argc, char *const *argv)
{
  options(argc, argv, "c:i:p:?");
  Hash *h = NULL;
  Pool *p = pool_init();
  struct xpd *xpd = NULL;
  if (project)
    {
      xpd = xpd_init(project, p);
      h = xpd->opts;
    }
  else
    h = hash_create(10);
  if (cfg)
    {
      roco_hash_hash(h, roco_load1(cfg));
      unsigned char *s = NULL;
      FILE *outfp = stdout;
      
      if (!infile)
	infile = hash_find(h, (uccp)"infile");
      
      if (infile)
	s = loadfile((uccp)infile, NULL);
      else
	s = loadstdin(NULL);

      if (!outfile)
	outfile = hash_find(h, (uccp)"outfile");

      if (outfile)
	outfp = xfopen(outfile, "w");

      strrep_f_h((ccp)s, outfp, h);

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
    case 'o':
      outfile = arg;
      break;
    case 'p':
      project = arg;
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

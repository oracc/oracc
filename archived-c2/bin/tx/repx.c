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
int env = 0;
const char *infile = NULL;
const char *outfile = NULL;
const char *project = NULL;

int
main(int argc, char *const *argv)
{
  options(argc, argv, "c:ei:p:?");
  Hash *h = NULL;
  Pool *p = pool_init();
  struct xpd *xpd = NULL;
  int status = 1;
  
  if (project)
    {
      xpd = xpd_init(project, p);
      h = xpd->opts;
    }
  else
    h = hash_create(10);
  if (cfg)   
    roco_hash_hash(h, roco_load1(cfg));
  else if (!env)
    fprintf(stderr, "repx: must give -c [CONFIG_FILE] or -e command-line. Stop.\n");
  else
    {
      unsigned char *s = NULL;
      FILE *outfp = stdout;

      status = 0;
      
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
  return status;
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
    case 'e':
      env = 1;
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

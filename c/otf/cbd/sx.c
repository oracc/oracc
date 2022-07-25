#include <stdio.h>
#include <unistd.h>
#include <psd_base.h>
#include "options.h"
#include "runexpat.h"
#include "xmlutil.h"
#include "atf.h"
#include "cdf.h"
#include "gdl.h"
#include "lang.h"
#include "pool.h"
#include "npool.h"
#include "gx.h"
#include "sx.h"

extern struct sig_context *global_scp;

const char *errmsg_fn = NULL;

const char *outfile = NULL;
FILE *f_out = NULL;

int filter = 0;

/* sig action ops have sig_ prefixes; functions don't have the _ */
int sig_cgp = 0;
int sig_group = 0;
int sig_merge = 0;
int sig_sort = 0;
int sig_stats = 0;

extern int math_mode;
extern int cbd(const char *fname);

int
main(int argc, char **argv)
{
  struct sigfile *sigfile = NULL;
  options(argc,argv,"cgmno:sv");

  if (outfile)
    f_out = xfopen(outfile,"w");
  else
    f_out = stdout;

  f_log = stderr;
  file = argv[optind];

  if (sig_merge)
    {
      sigmerge(file);
    }
  else if (sig_stats)
    {
      atf_init();
      sigstats(file);
      atf_term();
    }
  else
    {
      math_mode = no_pi = do_cuneify = use_unicode = 1;
      galloc_init();
      pool_init();
      tree_init();
      gdl_init();
      curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
      /*cbds = hash_create(1);*/
      with_textid = 0;
      
      sigfile = sigload(file);
      
      if (sigfile)
	{
	  if (sig_sort)
	    {
	      sigindex(sigfile);
	      sigdump(sigfile);
	    }
	}  
      lang_term();
      gdl_term();
      pool_term();
      tree_term(1);
      galloc_term();
    }

  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "sx";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'b':
      break;
    case 'c':
      sig_cgp = 1;
      break;
    case 'd':
      break;
    case 'f':
      filter = 1;
      break;
    case 'g':
      sig_group = 1;
      break;
    case 'i':
      break;
    case 'm':
      sig_merge = 1;
      break;
    case 'n':
      sig_stats = 1;
      break;
    case 'o':
      outfile = optarg;
      break;
    case 'p':
      break;
    case 's':
      sig_sort = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

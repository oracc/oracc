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
#include "globals.h"

extern unsigned char *gdl_signnames(unsigned char *atf, int frag_ok);

const char *outfile = NULL;
FILE *outfp = NULL;
extern int backslash_is_formvar;
extern int cbd_rules;
extern int do_signnames;
extern int math_mode;
extern int use_legacy;
extern int use_unicode;
const char *project;
static int deep = 0;
static int stdin_input = 0;

char *gdl_file = NULL;
int gdl_file_len = 0;
int gdl_line = 0;

struct npool *gdl_pool = NULL;
struct xpd *xpd = NULL;

int
main(int argc, char **argv)
{
  const char *fname[2];
  unsigned char *sig;

  options(argc,argv,"bcdgno:p:sux");

  if (!stdin_input)
    {
      fname[0] = file = argv[optind];
      fname[1] = NULL;
    }

  if (outfile)
    f_xml = outfp = xfopen(outfile,"w");
  else
    f_xml = stdout;
  f_log = stderr;
  
  gdl_pool = npool_init();
  galloc_init();
  pool_init();
  tree_init();
  gdl_init();
#ifdef GVL_MODE
  gvl_setup("ogsl", "ogsl", 1); /* 1=tsv */
#endif
  cuneify_init(xpd);
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);

  with_textid = 0;
  math_mode = no_pi = do_cuneify = use_unicode = 1;

  if (argv[optind])
    {
      sig = gdl_signnames((unsigned char*)argv[optind],1);
      printf("%s\t%s\n", argv[optind], sig);
    }
  else
    {
      char buf[128];
      while (fgets(buf,128,stdin))
	{
	  buf[strlen(buf)-1] = '\0';
	  sig = gdl_signnames((unsigned const char *)buf,1);
	  printf("%s\t%s\n", buf, sig);
	}
    }
  
  npool_term(gdl_pool);
  lang_term();
  gdl_term();
  pool_term();
  tree_term(1);
  cuneify_term();
  galloc_term();
  if (gdl_file)
    free(gdl_file);
  if (outfp)
    xfclose(outfile,outfp);
  return 1;
}

int major_version = 1; int minor_version = 0;
const char *prog = "gdlme2";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'b':
      gdl_bootstrap = 1;
      break;
    case 'c':
      cbd_rules = 1;
      backslash_is_formvar = 1;
      break;
    case 'd':
      deep = 1;
      break;
    case 'g':
      backslash_is_formvar = 0;
      break;
    case 'n':
      do_signnames = 1;
      break;
    case 'o':
      outfile = oarg;
      break;
    case 'p':
      project = oarg;
      break;
    case 's':
      stdin_input = 1;
      break;
    case 'u':
      use_unicode = 1;
      break;
    case 'x':
      no_gdl_xids = 0;
      break;
    default:
      return 1;
    }
  return 0;
}

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

const char *errmsg_fn = NULL;

int flex_scanner = 0;
int identity_output = 0;
int stdin_input = 0;

extern int yydebug;
extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

int
main(int argc, char **argv)
{
  options(argc,argv,"cdefisv");

#if 1
  file = argv[optind];
#else
  const char *fname[2];

  /* no way to set stdin_input atm */
  if (!stdin_input)
    {
      fname[0] = file = argv[optind];
      fname[1] = NULL;
    }

  if (outfile)
    f_xml = outfp = xfopen(outfile,"w");
  else
    f_xml = stdout;
#endif

  f_log = stderr;
  math_mode = no_pi = do_cuneify = use_unicode = 1;
  
  common_init();
  galloc_init();
  pool_init();
  tree_init();
  gdl_init();
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  cbds = hash_create(1);
  with_textid = 0;

  if (flex_scanner)
    flex(file);
  else
    cbd(file);

  if (identity_output)
    identity(curr_cbd);
  
  lang_term();
  gdl_term();
  pool_term();
  tree_term(1);
  galloc_term();
  common_term();
  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "gx";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'b':
      break;
    case 'c':
      check = 1;
      break;
    case 'd':
      yydebug = 1;
      break;
    case 'e':
      entries = 1;
      break;
    case 'f':
      flex_scanner = 1;
      break;
    case 'g':
      break;
    case 'i':
      identity_output = 1;
      break;
    case 'n':
      break;
    case 'o':
      break;
    case 'p':
      break;
    case 's':
      sigs = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

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

int stdin_input = 0;
Hash_table *cbds = NULL;

extern int cbd(const char *fname);

int
main(int argc, char **argv)
{
  const char *fname[2];

  options(argc,argv,"bcdgno:p:suxv");

  if (!stdin_input)
    {
      fname[0] = file = argv[optind];
      fname[1] = NULL;
    }

#if 0
  if (outfile)
    f_xml = outfp = xfopen(outfile,"w");
  else
    f_xml = stdout;
#endif

  f_log = stderr;
  
  galloc_init();
  pool_init();
  tree_init();
  gdl_init();
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  cbds = hash_create(1);
  with_textid = 0;
  cbd(file);
  
  /*current_state = set_state(s_global,s_text);*/
#if 0
  if (!project)
    {
      project = "cdli";
      load_lang_in_project(current_state.lang);
      charset_init_lang(curr_data->this);
      curr_data->cset = curr_data->this->cset[current_state.mode];
    }

  math_mode = no_pi = do_cuneify = use_unicode = 1;
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n",f_xml);
  if (stdin_input)
    {
      runexpat(i_stdin,NULL,gdlme_sH,gdlme_eH);
    }
  else
    {
      if (!access(fname[0],R_OK))
	runexpat(i_list,fname,gdlme_sH,gdlme_eH);
      else
	fprintf(f_log,"gdlme: can't open %s for input\n",fname[0]);
    }

  (void)cbd_strip_backslash(NULL);
#endif

  lang_term();
  gdl_term();
  pool_term();
  tree_term(1);
  galloc_term();
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
      break;
    case 'd':
      break;
    case 'g':
      break;
    case 'n':
      break;
    case 'o':
      break;
    case 'p':
      break;
    case 's':
      stdin_input = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

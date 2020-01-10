#include <stdlib.h>
#include <stdio.h>
#include <psd_base.h>
#include <options.h>
#include "lang.h"
#include "charsets.h"
#include "warning.h"
#include "ngram.h"
#include "pool.h"

FILE *f_log, *f_nlx;
const char *nlx_file = "ngram.nlx";
const char *xcl_file = NULL;

struct NL_context*nlcp = NULL;
struct xcl_context *xc;

struct lang_context *curr_lang;

extern int ng_debug;
int cbd_lem_sigs = 0;
int fuzzy_aliasing = 0;
int bootstrap_mode, lem_autolem, verbose, lem_standalone, shadow_lem,
  ignore_plus, slow_lem_utf8, lem_dynalem;
const char *textid, *project, *lang;
int pretty = 0, psu = 0, use_unicode = 0;
int
main(int argc, char **argv)
{
  struct sigset *sp = NULL;
  extern int psus_sig_check, links_standalone;

  links_standalone = 1;
  psus_sig_check = 0;
  
  (void)sig_context_init();
  sp = sig_new_context_free_sigset();

  pool_init();
  
  f_log = stderr;
  nl_init();
  options(argc, argv, "dl:n:p:x:v");
  nlcp = nl_load_file(sp,NULL,nl_file,nlcp_action_psu,lang);
  f_nlx = fopen(nlx_file,"w");
  if (f_nlx)
    nl_dump(f_nlx,nlcp);
  else
    fprintf(stderr,"ngram: can't open %s to dump NLX\n",nlx_file);

  project = "epsd2";
#if 1
  curr_lang = NULL;
#else
  load_lang_in_project(current_state.lang);
  charset_init_lang(curr_data->this);
  curr_data->cset = curr_data->this->cset[current_state.mode];
#endif

  if (xcl_file)
    {
      ngramify_init();
      xc = xcl_load(xcl_file,0);
      xc->user = nlcp;
      xcl_map(xc,ngramify,NULL,NULL,NULL);
      /* x2_serialize(xc,stdout,1); */
      links_serialize(stdout, xc->linkbase, 1);
      ngramify_term();
    }

  return 0;
}

int
opts (int o, char *c)
{
  int ret = 0;
  switch (o)
    {
    case 'a':
      fprintf(stderr, "ngrammer: -a argument (set action) not yet supported\n");
      break;
    case 'd':
      ++ng_debug;
      break;
    case 'l':
      lang = c;
      break;
    case 'n':
      nl_file = c;
      break;
    case 'p':
      project = c;
      break;
    case 'x':
      xcl_file = c;
      break;
    case 'v':
      verbose = 2;
      break;
    default:
      ret = 1;
      break;
    }
  return ret;
}
const char *prog = "ngram";
const char *usage_string = "ngram";
int major_version = 0;
int minor_version = 1;
void help() {}

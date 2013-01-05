#include <stdlib.h>
#include <stdio.h>
#include <psd_base.h>
#include <options.h>
#include "lang.h"
#include "charsets.h"
#include "warning.h"
#include "ngram.h"

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
  ignore_plus, slow_lem_utf8;
const char *textid, *project, *lang;
int pretty = 0, psu = 0, use_unicode = 0;

int
main(int argc, char **argv)
{
  f_log = stderr;
  nl_init();
  options(argc, argv, "dl:n:p:x:");
  f_nlx = fopen(nlx_file,"w");
  if (f_nlx)
    nl_dump(f_nlx,nlcp);
  else
    fprintf(stderr,"ngram: can't open %s to dump NLX\n",nlx_file);

  project = "cdli";
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
      x2_serialize(xc,stdout,1);
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
    case 'd':
      ++ng_debug;
      break;
    case 'l':
      lang = c;
      break;
    case 'n':
      nlcp = nl_load_file(NULL,nlcp,nl_file = c,nlcp_action_rewrite,lang);
      break;
    case 'p':
      project = c;
      break;
    case 'x':
      xcl_file = c;
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

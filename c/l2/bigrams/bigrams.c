#include <stdlib.h>
#include <stdio.h>
#include "ilem_form.h"
#include "xcl.h"
#include "ngram.h"

static struct NL_context *bigrams_nlcp = NULL;
extern const char *project;
extern int verbose;

void
bigrams_init(struct sigset *sp)
{
  static char buf[128];
  sprintf(buf,"/usr/local/oracc/pub/%s/bgrm-%s.ngm", sp->project, sp->lang);
  if (!xaccess(buf,R_OK,0))
    {
      bigrams_nlcp = nl_load_file(sp, bigrams_nlcp, buf, nlcp_action_rewrite, sp->lang);
      if (verbose)
	fprintf(stderr,"loaded bigrams %s\n", buf);
    }
  if (bigrams_nlcp)
    {
      bigrams_nlcp->action = nlcp_action_rewrite;
      bigrams_nlcp->active_hash = bigrams_nlcp->posNgrams;
      bigrams_nlcp->func = nlcp_rewrite;
      ngramify_init();
    }
  sp->bigrams = bigrams_nlcp;
}

void
bigrams_term()
{
  ngramify_term();
}

void
bigrams(struct xcl_context *xc)
{
  if (bigrams_nlcp)
    {
      extern int ngramify_disambiguating;
      ngramify_disambiguating = ngramify_per_lang = 1;
      xc->user = (void*)(uintptr_t)NGRAMIFY_USE_BIGRAMS;
      xcl_map(xc,ngramify,NULL,NULL,NULL);
    }
}

#include <stdlib.h>
#include <stdio.h>
#include "memblock.h"
#include "xcl.h"
#include "sigs.h"
#include "npool.h"
#include "f2.h"
#include "ngram.h"

static int have_psus = 0;
#if 0
static struct NL_context **per_lang_nlcp = NULL;
#endif
extern const char *project;
extern int verbose;

static void
psu_sub_init(struct NL_context *nlcp)
{
  nlcp->action = nlcp_action_psu;
  nlcp->func = links_psu;
  ngramify_init();
  ++have_psus;
}

static void
parse_psu(struct sigset *sp, struct NLE *nlep)
{
  nlep->psu_form = mb_new(sp->owner->mb_f2s);
  f2_parse((unsigned char*)nlep->owner->file,
	   nlep->lnum,
	   npool_copy((unsigned char *)nlep->psu,sp->owner->pool),
	   nlep->psu_form, NULL);
  nlep->psu_form->file = (unsigned char *)nlep->owner->file;
  nlep->psu_form->lnum = nlep->lnum;
  nlep->psu_form->lang = sp->lang;
}

struct NL_context *
xpsus2_init(struct sigset *sp)
{
  static char buf[128];
  struct NL_context *nlcp = NULL;

#if 0
  int lcode = langtok(lang,strlen(lang))->code;
  const char *sysprojp = sysproj(lang);
  if (!per_lang_nlcp)
    per_lang_nlcp = calloc(c_count, sizeof(struct NL_context *));
#endif

  if ('.' == *sp->project)
    {
      sprintf(buf,"%s/%s/glossary.psu",sp->project,sp->lang);
    }
  else
    {
      sprintf(buf,"/usr/local/oracc/bld/%s/%s/glossary.psu",sp->project,sp->lang);
    }
  if (!xaccess(buf,R_OK,0))
    nlcp = nl_load(nlcp,buf,nlcp_action_psu, (char*)sp->lang);
  if (nlcp)
    {
      int i;
      struct NL *nlp;
      for (nlp = nlcp->nlp; nlp; nlp = nlp->next)
	for (i = 0; i < nlp->nngrams; ++i)
	  parse_psu(sp, &nlp->parsed_ngrams[i]);
      if (verbose)
	fprintf(stderr,"psu: loaded psu file %s\n",buf);      
      psu_sub_init(nlcp);
    }

  return nlcp;
}

void
xpsus2_term()
{
  ngramify_term();
}

void
xpsus2(struct xcl_context *xc)
{
  xcl_map(xc,ngramify,NULL,NULL,NULL);
}

#include <stdio.h>
#include "ngram.h"

static FILE *dump_fp;

static void
dump_cf_tts(struct CF *cfp)
{
  int i;
  fprintf(dump_fp,"<cf bad=\"%d\" neg=\"%d\" wild=\"%d\">%s</cf>",
	  cfp->bad, cfp->neg, cfp->wild,cfp->cf);
  for (i = 0; cfp->preds[i]; ++i)
    fprintf(dump_fp,"<pred neg=\"%d\" attr=\"%s\" value=\"%s\"/>",
	    cfp->preds[i]->neg, cfp->preds[i]->key, cfp->preds[i]->value);
}

static void
dump_meta(struct MD*mdp)
{
  const char **mvp;
  fprintf(dump_fp,"<meta field=\"%s\"\n>",mdp->field);
  for (mvp = mdp->values; *mvp; ++mvp)
    fprintf(dump_fp,"<value>%s</value\n>",*mvp);
  fprintf(dump_fp,"</meta\n>");
}

static void
dump_nle(struct NLE*nlep)
{
  fprintf(dump_fp,"<nle priority=\"%d\"\n>",nlep->priority);
  if (nlep->meta)
    dump_meta(nlep->meta);
  if (nlep->ncfs)
    {
      int i;
      fputs("<cts>",dump_fp);
      for (i = 0; nlep->tts[i]; ++i)
	{
	  fputs("<ct>",dump_fp);
	  dump_cf_tts(nlep->cfs[i]);
	  fputs("</ct>",dump_fp);
	}
      fputs("</cts>",dump_fp);
      if (!nlep->implicit_tts)
	{
	  fputs("<tts>",dump_fp);
	  for (i = 0; i < nlep->ncfs; ++i)
	    {
	      fputs("<tt>",dump_fp);
	      dump_cf_tts(nlep->cfs[i]);
	      fputs("</tt>",dump_fp);
	    }
	  fputs("</tts>",dump_fp);
	}
      if (nlep->psu)
	fprintf(dump_fp,"<psu>%s</psu>", nlep->psu);
    }
  fputs("</nle\n>",dump_fp);
}

void
nl_dump(FILE *fp, struct NL_context*nlcp)
{
  static const char *ns = "http://oracc.org/ns/ngram/1.0";
  struct NL *nlp;
  size_t i;

  if (nlcp)
    {
      dump_fp = fp;
      fprintf(fp,"<nl xmlns=\"%s\" xmlns:ngram=\"%s\"\n>",ns,ns);
      for (nlp = nlcp->nlp; nlp; nlp = nlp->next)
	for (i = 0; i < nlp->nngrams; ++i)
	  dump_nle(&nlp->parsed_ngrams[i]);
      fputs("</nl>",fp);
    }
}

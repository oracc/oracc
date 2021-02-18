#include <stdlib.h>
#include <stdio.h>
#include "memblock.h"
#include "lang.h"
#include "xcl.h"
#include "sigs.h"
#include "npool.h"
#include "f2.h"
#include "ngram.h"

extern const char *project;
extern int verbose;

struct NL_context *
psus2_init(struct sigset *sp)
{
  return NULL;
}

void
psus2_term()
{
  ngramify_term();
}

void
psus2(struct xcl_context *xc)
{
  int per_lang = ngramify_per_lang;
  ngramify_per_lang = 1;
  xcl_map(xc,ngramify,NULL,NULL,NULL);
  ngramify_per_lang = per_lang;
}

/*
 * In ATF you can say:
 * 
 * #psu: [%langswitch] NGRAM => PSU_SIG
 *
 * If %langswitch is not given, the current language 
 * context (the default for the text) is used.  For PSUs
 * headed by a NN the context language should be used: 
 * the %qpn will be inferred from the POS of the head.
 */
struct f2 *
psu2_register(const char *file, size_t lnum, 
	      struct sigset *sp, const char *lang,
	      const unsigned char *ngram_line,
	      List *component_sigs)
{
  struct NL *nlp = NULL;

  if (!sp || !lang || !ngram_line)
    return NULL;

  nl_set_location(file,lnum+1);

  if (!sp->psus)
    sp->psus = nl_new_context(sp, nlcp_action_psu, lang);

  if (!sp->psus->nlp)
    nlp = nl_new_nl(sp->psus);
  else
    nlp = sp->psus->nlp;

  nlp->name = "psu";

  if (verbose > 2)
    fprintf(stderr, "psu: registering psu %s from %s:%d\n", ngram_line, file, (int)lnum);

  return nl_process_one_line(nlp, (char *)ngram_line, component_sigs);
}

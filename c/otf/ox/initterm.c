#include "ox.h"
#include "atf.h"
#include "tree.h"
#include "props.h"
#include "nsa.h"
#include "xcl.h"
#include "xmlutil.h"
#include "cdt.h"
#include "lemline.h"
#include "initterm.h"
#include "text.h"
#include "pool.h"
#include "ngram.h"
#include "f2.h"

void
ox_init(void)
{
  pool_init();
  tree_init();
  gdl_init();
  text_vec_init();
  /*lemline_init();*/
  tree_functions(x2_serialize);
  ncname_init();
  catvec_init();
  load_labels_init();
  global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  with_noform = 1;

  if (need_lemm)
    {
      f2_init();
      props_auto_init();
      lem_init();
      nl_init();
      ngramify_init();
      (void)sig_context_init();
    }

  rnvif_init();

  if (dollar_fifo)
    dollar_init();

  if (perform_nsa)
    {
      const char *nsa_data = "/usr/local/oracc/lib/data/nm.xml";
      if (!(global_nsa_context = nsa_init(nsa_data)))
	{
	  fprintf(stderr,"atf2xtf: NSA init failed on `%s'\n",nsa_data);
	  exit(1);
	}
      xlink_results = 1;
    }
  
  if (named_ents)
    {
#if 0
      if (!(global_psa_context = psa_init()))
	{
	  fprintf(stderr,"atf2xtf: PSA init failed\n");
	  exit(1);
	}
#endif
      xlink_results = named_ents > 1;
    }
}

void
ox_term(void)
{
  catvec_term();
  gdl_term();
  pool_term();
  tree_term();
  cdfrnc_term();
  /*  lemline_term(); */
  label_term();
  load_labels_term();
  text_term();
  block_term();
  inline_term();
  rnvif_term();
  if (dollar_fifo)
    dollar_term();
  (void)xmlify((const unsigned char *)0);

  if (perform_nsa)
    nsa_term();

  if (need_lemm)
    {
      f2_term();
      sig_context_term();
      nl_term();
      ngramify_term();
      props_auto_term();
      lem_term();
      xcl_final_term();
    }
}

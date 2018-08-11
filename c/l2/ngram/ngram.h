#ifndef _NGRAM_H
#define _NGRAM_H
#include "hash.h"
#include "xcl.h"
#include "npool.h"
#include "f2.h"
/*
  Implement a translation of the old Perl Ngram grammar into C.
  PV = Perl version; CV = C version.
 */
struct sigset;
enum nlcp_actions { nlcp_action_rewrite , nlcp_action_psu , 
		    nlcp_action_psa, nlcp_action_props };

#define NGRAMIFY_USE_PSU       	0x01
#define NGRAMIFY_USE_BIGRAMS	0x02
#define NGRAMIFY_USE_COLLOS	0x04

struct ML;

struct NL_context
{
  enum nlcp_actions action;
  int nngrams;
  Hash_table *posNgrams;
  Hash_table *psuNgrams;
  Hash_table *active_hash;
  struct npool *pool;
  void (*func)(struct xcl_context *,struct ML *);
  struct NL*nlp;
  const char *lang;
  struct sigset *owner;
};

/*
  NL is an Ngram list consisting of zero or more Ngram List
  Expressions (NLE).  The NL hosts indices of the NLEs by citation
  form (CF); between the two hashes all the loaded NLEs are
  accessible via CFs.
 */
struct NL
{
  const char *file;
  const char *name;
  int initial_line; /* subtract this from NLE# to get 
		       line number of match */
  char **ngram_lines;
  unsigned char *fmem;
  struct NLE *parsed_ngrams;
  struct NL *next;
  int nngrams;
  struct NL_context *owner;
  struct NLE *last_parsed_ngram;
};

/*
  NLE   := EC* CF+ ( '=>' TT+ )?

  EC = Environment Constraint

  In PV the NLE was an anonymous array with the following
  elements:

  0 \@env
  1 \@ct_steps
  2 \@tt_steps
  3 $priority
  4 $nle_count
  5 $dsu

  In CV \@env is replaced by the standard four form MD properties.
  
  In PV ct_steps and tt_steps were stored as arrays of strings; in CV
  we parse them into structures.

  Priority is a computed importance for this NLE; NLEs with higher
  priority are applied in preference to NLEs with lower priority.

  The nle_count member is simply the list order of the NLE on input;
  this allows comparison of NLEs by order of input.

  The psu is a Phrasal Semantic Unit structure (this was named
  DSU (Discontinous Semantic Unit) in PV; the name change reflects the
  fact that PSUs are only occasionally discontinuous--which they
  continue to be permitted to be.
 */
struct NLE
{
  const char *line;
  char *chop; /* this one gets chopped up */
  struct MD  *meta;
  struct CF  **cfs;
  struct CF  **tts;
  int ncfs;
  /* condition tokens start at cfs[0] */
  int priority;
  int nle_count;
  int implicit_tts;
  const char *psu;
  struct f2 *psu_form;
  const char *file;
  int lnum;
  void *user;
  struct NL *owner;
  int deleted;
  struct NLE *next;
};

struct NLE_set
{
  struct NLE **pp;
  int pp_used;
  int pp_alloced;
};

struct MD
{
  const char *field;
  const char **values;
};

/* 
  CF    := '!'? ([A-Za-z']+ | '*' , [0-9]*) , PRED*

  PRED  := '[' #ADV | @<POS> | GW | SENSE | GW//SENSE ']'

  POS   := [A-Z]+
 
  ADV   := '#adv'
 */

struct CF
{
  int bad;
  int neg;
  int wild;
  const char *cf;
  struct f2 *f2;
  struct PRED **preds;
  struct NLE *owner;
};

struct PRED
{
  const char *key;
  const char *value;
  int neg;
  struct CF *owner;
};

/*
  PSU := CFGW | '#u'[0-9]+

  CFGW is as above; the notation #u0001 etc., refers to usages, i.e.,
  dictionary subheadings of the form 'black cow'.
 */
struct PSU
{
  const char *cfgw;
  const char *usage;
};

/*
  In PV a match list was a list of anonymous arrays with the following
  elements: 

  a) For a non-wild match:

    0 = \@matches -- the list of 'l' nodes in the DOM that match;
      it can have more than one entry because the matcher may have
      tested all the entries in an 'll' node (i.e., all the
      possibilities in an ambiguous lemmatization)
    1 = the TT that corresponds to this step
    2 = the DSU (if any) that corresponds to this step

  b) For a wild match:

    0 = cl_item -- the 'l' node in the XCL DOM
    1 = '*' -- a literal asterisk string

  In CV there is a struct ML for the matching process; each matched
  step in the NLE has its matches stored in the matches member of the
  struct ML.

 */
struct ML
{
  int nmatches;
  int matches_alloced;
  int matches_used;
  struct match *matches;
  /*struct ML *next;*/
};

struct match
{
  struct xcl_l*lp;
  struct f2 **matching_f2s;
  int nmatches;
  int wild;
  struct CF *tt;
  /*int ntts;*/
  const char  *psu;
  struct f2 *psu_form;
  const unsigned char **psu_finds;
  int psu_nfinds;
  void *user;
};

extern void nlcp_rewrite(struct xcl_context *xcp, struct ML *mp);

extern const char *nl_file;
extern FILE *f_log;
extern int ng_debug, nl_lnum, ngramify_per_lang;
extern const char *file;
struct NL_context *nl_load(struct NL_context*nlcp, const char *fname,
			   enum nlcp_actions act, const char *lang);
void nl_set_location(const char *fn,int ln);
extern void nl_dump(FILE *fp,struct NL_context*nlcp);
extern struct NL *nl_setup(struct NL_context**nlcp, enum nlcp_actions act, const char *lang);

extern struct f2* parse_ngram_line(struct NL*nlp, const char *line, int ngram_index, 
				   struct NL_context*nlcp, List *components,
				   void *user);

extern void ngramify(struct xcl_context *xcp, struct xcl_c*cp);
extern void ngramify_init(void);
extern void ngramify_reset(void);
extern void ngramify_term(void);
extern void ngdebug(const char *mess,...);

extern int get_ngram_id(void);

extern void nl_init(void);
extern void nl_term(void);

extern struct NL_context *nl_new_context(struct sigset *sp, 
					 enum nlcp_actions act, 
					 const char *lang);
extern void nl_free_context(struct NL_context *nlcp);

extern struct NL*nl_new_nl(struct NL_context *nlcp);
extern struct f2* nl_process_one_line(struct NL *nlp, const char *lp, List *components);
extern struct NL_context *nl_load_file(struct sigset *sp,
				       struct NL_context*nlcp, 
				       const char *fname, 
				       enum nlcp_actions act, 
				       const char *lang);

extern int nl_parse_cts(char *line, char *end, struct NLE *nlep, int tts_mode);
extern struct PRED *new_pred(List *lp);
extern struct CF *new_cf(List *lp);

#endif /*_NGRAM_H*/

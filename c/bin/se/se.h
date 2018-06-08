#ifndef _SE_H
#define _SE_H
#include "types.h"

struct lookup
{
  const unsigned char *term;
  const char *index;
};

enum op_type 
  {
    o_Lookup , o_Relation , o_Search
  };

enum relation
  {
    e_AND , e_OR, e_NOT, e_TOP
  };

union op
{ 
  enum op_type type;
  struct lookup *l;
  enum relation *r;
  struct search *s;
};

struct search
{
  union op *ops;
  struct Datum *res;
  enum result_granularity res_gran;
  enum result_id_domain   res_id;
};

/* The search engine handles results from the text search
   in Location format; from the record/field search (used by
   the catalog and one day the bibliography) and the instance
   mapper */

struct Datum
{
  enum datum_type type;
  union lNp l;	/* left end of range */
  union lNp r;	/* right end of range */
  void *ldata;
  void *rdata;
  Unsigned32 count; /* number of elements in arrays */
  Unsigned32 stripped; /* TRUE if strip_txt_flags has been run on
		    these locations */
  Unsigned32 data_size;/* size of one data element */
  const Uchar *key;
 int expr_id;
};

struct token
{
 enum se_toks type;
 int flag;
 int expr_id;
 const void *data;
 const unsigned char *mangled;
 struct expr_rules *rules; /* rules to use when mangling this token */
};

struct se_tok_tab
{
 const char *name;
 enum se_toks tok;
};

enum near_dirs { n_before , n_after , n_either , n_bad_dir };

struct near
{
 enum near_dirs dir;
 enum near_levs lev;
 Signed32 plus;
 Signed32 minus;
 Signed32 range;
 Signed32 redup;
};

extern int any_index;
extern int show_tokens;
extern int verbose;

extern void showtoks(struct token*toks,int ntoks, int phase);
extern struct token *tokenize(const char **argv, int *ntoks);
extern void tokinit(void);
extern struct se_tok_tab *setoks(register const char *str, register unsigned int len);
extern struct lm_tab *langmask(register const char *str, register unsigned int len);
extern struct Datum *merge_data_pair (struct Datum *d1, struct Datum *d2);
extern void binop_init(void);
extern void binop24_init(void);
extern void se_vids_init(const char *index);
extern const char ** xmldir_toks(const char *xmldir);
extern void xmldir_results(const char *xmldir, int count);

extern void debug_location8(struct location8 *l8p, struct location8 *r8p);
extern void debug_location16(struct location16 *l16p, struct location16 *r16p);
extern void debug_location24(struct location24 *l24p, struct location24 *r24p);

#include "selib.h"

#endif /*_SE_H*/

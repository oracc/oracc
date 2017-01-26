#ifndef TYPES_H_
#define TYPES_H_ 1
#include <limits.h>
#include <stdint.h>
#include "psdtypes.h"
typedef unsigned char  Char;
typedef uint32_t       Four_bytes;
typedef uint16_t       Two_bytes;
typedef int16_t        Two_bytes_s;
typedef unsigned char  One_byte;
typedef signed char    One_byte_s;

#define TWO_BYTES_MAX 65535
#define TWO_BYTES_s_MAX 32767
#define TEXT_ID_MAX	    	PQ_MAX
#define UNIT_ID_MAX		TWO_BYTES_MAX
#define WORD_ID_MAX	      	TWO_BYTES_MAX
#define START_COLUMN_MAX      	TWO_BYTES_MAX
#define END_COLUMN_MAX        	TWO_BYTES_MAX
#define BRANCH_ID_MAX	      	TWO_BYTES_s_MAX
#define FOUR_BYTES_MAX		USHORT_MAX

#define PQ_MAX 		0x00200000
#define PQ_BIT 		(1<<31)
#define setQ(x)		((x)|=PQ_BIT)
#define isQ(t)		((t)&PQ_BIT)

#define IF_GUIDE	0x1000
#define IF_LEMMA	0x2000
#define IF_MORPH	0x4000
#define IF_MORPH2	0x8000
#define mask_sc(sc)	((sc)&~(IF_GUIDE|IF_LEMMA|IF_MORPH|IF_MORPH2))
#define prop_sc(sc)	((sc)&(IF_GUIDE|IF_LEMMA|IF_MORPH|IF_MORPH2))

#define G_LOGO		0x1000
#define logo_mask(x)	((x)&~(G_LOGO))
#define is_logo(x)      ((x)&G_LOGO)

#define LM_NONE		0x00000000
#define LM_MISC		0x01000000
#define LM_SUX 		0x02000000
#define LM_SUX_ES 	0x04000000
#define LM_AKK 		0x08000000
#define LM_QPC 		0x10000000
#define LM_QPE 		0x20000000
#define LM_NOT_4_USE    0x40000000
#define LM_MASK 	0x7f000000

enum langmask
  {
    lm_none    = LM_NONE,
    lm_misc    = LM_MISC,
    lm_sux     = LM_SUX,
    lm_sux_es  = LM_SUX_ES,
    lm_akk     = LM_AKK,
    lm_qpc     = LM_QPC,
    lm_qpe     = LM_QPE,
    lm_any     = lm_misc|lm_sux|lm_sux_es|lm_akk|lm_qpc|lm_qpe
  };

struct lm_tab
{
  const char *name;
  enum langmask lm;
};

#define TEXTID_MASK 	0xffffff
#define PQVal(t)	((t)&PQ_BIT)
#define idVal(t)	((int)((t)&TEXTID_MASK))
#define lmVal(t)	((t)&LM_MASK)

#define X_FACTOR	2000000u /* X-numbers are P-numbers of 2 million and above */
#define XIFY_ID(x)     	((x)+X_FACTOR)
#define UNEX_ID(x)	((idVal(x))-X_FACTOR)

#define loc8 \
  Four_bytes text_id; \
  Two_bytes  unit_id; \
  Two_bytes  word_id; \

struct location8
{
  loc8
};

struct location16
{
  loc8
  Two_bytes_s branch_id;
  Two_bytes   start_column;
  Two_bytes   end_column;
  Two_bytes   properties;
};

struct location24
{
  loc8
  Two_bytes_s branch_id;
  Two_bytes   start_column;
  Two_bytes   end_column;
  Two_bytes   properties;
  Two_bytes   sentence_id;
  Two_bytes   clause_id;
  Two_bytes   phrase_id;
  Two_bytes   lemma_id;
};

#undef loc8

enum result_granularity 
  {
    g_text , g_record , g_field , g_word , g_grapheme , g_not_set
  };

enum result_id_domain
  {
    i_text , i_composite , i_biblio
  };

#define C(x) x,

enum se_toks
  {
    C(se_notused)
    C(se_and)C(se_or)C(se_not)C(se_groupo)C(se_groupc)
    C(se_near)C(se_sans)C(se_logo)C(se_sign)
    C(se_filter)C(se_filter1)C(se_lang)C(se_index)C(se_expr)C(se_rexp)
    C(se_hyphen)C(se_space)C(se_tilde)C(se_xspace)C(se_project)
    se_top
  };

enum near_levs { n_record = se_top+1, n_field , n_word , n_grapheme , n_bad_lev };

/* These must be ordered so that earlier items have larger granularity */
enum datum_type { d_cat , d_cbd , d_tra , d_lem , d_l24 , d_txt , d_uni , d_NONE };

struct expr_rules
{
  enum datum_type type;
  const char *sepchars;
  const char *stripchars;
  const char *default_near_scope;
  const char *default_bool_scope;
#if 1
  int ix_manglerules;
  int se_manglerules;
#else
  int lowercase;
  int unfd;
  int atf2utf8;
#endif
  enum result_granularity granularity;
  const char pos_id_prefix;
  const char neg_id_prefix;
};
extern struct expr_rules rulestab[];
extern const char * const datum_type_names[];

union lNp 
{ 
  struct location8 **l8p; 
  struct location16 **l16p; 
  struct location24 **l24p;
};

#endif /*TYPES_H_*/

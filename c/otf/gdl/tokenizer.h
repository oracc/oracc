#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include "xmlnames.h"

/* User Flags Implementation 2019-02-03- 
   =====================================

   User flags were implemented because a few extra flags were needed
   for RINAP2.

   To keep it quick (and dirty) to implement, a second set of four
   flags are defined which have the same token names etc. as the basic
   four flags, but with a 'u' prepended to them.

   The user flags are bound to the following symbols:

     dagger            U+2020
     double-dagger     U+2021
     bullet            U+2022
     triangular bullet U+2023

   They must be input in UTF-8.     
   
 */

#define UFLAG1_STR "\xe2\x80\xa0" /* dagger */
#define UFLAG2_STR "\xe2\x80\xa1" /* double-dagger */
#define UFLAG3_STR "\xe2\x80\xa2" /* bullet */
#define UFLAG4_STR "\xe2\x80\xa3" /* triangular bullet */

#define C(x) x,

#define T_MISC   C(notoken)C(cell)C(cellspan)C(field)C(ftype)C(nong)C(flag)C(uflag)
#define T_SHIFT  C(percent)C(sforce)C(lforce)
#define T_O      C(deto)C(glosso)C(damago)C(hdamago)C(supplo)C(implo)C(smetao)C(maybeo)C(exciso)C(uscoreo)C(agroupo)C(surro)C(eraso)C(normo)C(someo)
#define T_C      C(detc)C(glossc)C(damagc)C(hdamagc)C(supplc)C(implc)C(smetac)C(maybec)C(excisc)C(uscorec)C(agroupc)C(surrc)C(erasc)C(normc)C(somec)
#define T_BOUND  C(space)C(hyphen)C(slash)C(colon)C(ellipsis)C(linebreak)C(newline)C(icmt)C(plus)C(period)C(ilig)C(zspace)C(zhyphen)
#define T_GRAPH  C(g_c)C(g_v)C(g_n)C(g_s)C(g_q)C(g_corr)C(g_g)C(g_p)C(wm_absent)C(wm_broken)C(wm_linecont)C(wm_linefrom)C(g_t)
#define T_MODS   C(g_b)C(g_m)C(g_a)C(g_f)
#define T_DISAMB C(g_disamb)
#define T_NORM   C(norm)
#define T_PROX   C(prox)C(surro_mark)
#define T_OL     C(sol)C(eol)
#define T_VAR    C(varo)C(varc)
#define T_UB	 C(ub_plus)C(ub_minus)
#define T_NMARK  C(notemark)
#define T_NOOP	 C(noop)

#define SPLIT_BIT (1<<16)

enum t_type
  {
    T_MISC T_SHIFT T_O T_C T_BOUND T_GRAPH T_MODS T_DISAMB
    T_NORM T_PROX T_OL T_VAR T_UB T_NMARK T_NOOP
    type_top
  };

#define T_CLASSES C(text)C(bound)C(meta)
enum t_class
  {
    T_CLASSES class_top
  };

/* h=hash q=query b=bang s=star */
#define F0 C(f_none)
#define F1 C(f_h)C(f_q)C(f_b)C(f_s)
#define F2 C(f_hq)C(f_hb)C(f_hs)C(f_qb)C(f_qs)C(f_bs)
#define F3 C(f_hqb)C(f_hbs)C(f_hqs)C(f_qbs)
#define F4 C(f_hqbs)

enum f_type { F0 F1 F2 F3 F4 f_type_top };

/* h=uflag1 q=uflag2 b=uflag3 s=uflag4 */
#define UF0 C(uf_none)
#define UF1 C(uf_h)C(uf_q)C(uf_b)C(uf_s)
#define UF2 C(uf_hq)C(uf_hb)C(uf_hs)C(uf_qb)C(uf_qs)C(uf_bs)
#define UF3 C(uf_hqb)C(uf_hbs)C(uf_hqs)C(uf_qbs)
#define UF4 C(uf_hqbs)

enum uf_type { UF0 UF1 UF2 UF3 UF4 uf_type_top };

#undef C

struct fattr
{
  enum a_type a;
  const char *s;
};

struct flags
{
  enum f_type type;
  int h;
  int q;
  int b;
  int s;
  int nattr;
  struct token *t;
  const char *atf;
  struct fattr a[4];
};

struct uflags
{
  enum uf_type type;
  int h;
  int q;
  int b;
  int s;
  int nattr;
  struct token *t;
  const char *atf;
  struct fattr a[4];
};

struct token
{
  enum t_class class;
  enum t_type  type;
  const unsigned char *text;
  const void *data;
  struct lang_context *lang;
  const char *altlang;
  const char *fwhost;
  const void *grapheme;
  int lnum;
};

#define MEDIAL_OPEN  0x01 /* lug[al */
#define MEDIAL_HASH  0x02 /* l[ug]al */
#define MEDIAL_CLOSE 0x04 /* lu]gal */
#define FINAL_CLOSE  0x08 /* lugal]{ki} */

#define WORD_MATRIX_ABSENT_CHAR 	0x2014 /* EM DASH */
#define WORD_MATRIX_BROKEN_CHAR 	0x00B1 /* PLUS-MINUS */
#define WORD_MATRIX_LINEFROM_CHAR 	0x2190 /* LEFT ARROW */
#define WORD_MATRIX_LINECONT_CHAR 	0x2192 /* RIGHT ARROW */

struct medial_info
{
  unsigned char *raw;
  int status;
};

extern int is_flag[];
extern struct flags flag_info[];
extern const char *const type_data[];
extern const char *const type_names[];
extern struct token *tokens[];
extern int last_token;
enum f_type parse_flags(unsigned char *fptr, int *nflags);
enum uf_type parse_uflags(unsigned char *fptr, int *unflags);
int is_uflag(unsigned char *p);
void print_token(struct token *tp);
void showtoks(void);
void tokcheck_init(void);
void tokenize_init(void);
void tokenize_term(void);
void tokenize_reinit(void);
void tokenize(register unsigned char *l, unsigned char *e);

#endif

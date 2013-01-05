#ifndef _NSA_H
#define _NSA_H

#ifndef strdup
char *strdup(const char*);
#endif

#include "../include/hash.h"
#include "../include/list.h"
#include "../include/npool.h"

enum nsa_sex_axis { nsa_sex_disz_axis, nsa_sex_asz_axis,  nsa_sex_no_axis };
enum nsa_sex_form { nsa_sex_curviform, nsa_sex_cuneiform, nsa_sex_no_form };
enum nsa_sex_toks
{
  nsa_sex_szargal, nsa_sex_szaru, nsa_sex_szar,
  nsa_sex_geszu,   nsa_sex_gesz,  nsa_sex_u,
  nsa_sex_disz,    nsa_sex_asz
};

struct nsa_sex_tab
{
  const char *name;
  enum nsa_sex_toks type;
  int value;
};

#include "f2.h"
#include "ilem_form.h"
#include "xcl.h"
#include "links.h"
enum nsa_ptypes { NSA_P_LEMM , NSA_P_LINK , NSA_P_LITERAL , NSA_P_STOP , NSA_P_NONE};

struct nsa_text_ref
{
  enum nsa_ptypes ptype;
  union textu
  {
    struct xcl_l*lemmptr;
    struct linkset *linkptr;
    const unsigned char *literal;
  } t;
};

enum nsa_ttypes 
{ 
  NSA_T_GRAPHEME , 
  NSA_T_COUNT , 
  NSA_T_UNIT , 
  NSA_T_QUANTITY , 
  NSA_T_MEASURE , 
  NSA_T_AMOUNT ,
  NSA_T_COMMODITY ,
  NSA_T_STOP ,
  NSA_T_UNKNOWN , 
  NSA_T_NONE
};

struct nsa_token
{
  enum nsa_ttypes type;
  union nsa_data
  {
    struct nsa_grapheme *g;
    struct nsa_count *   c;
    struct nsa_unit *    u;
    struct nsa_quantity *q;
    struct nsa_measure * m;
    struct nsa_commodity*o; /* c=count; o=commodity */
    struct nsa_amount *  a;
  } d;
  struct nsa_token **children;
};
#define grapheme(x) (x)->d.g
#define count(x)    (x)->d.c
#define unit(x)     (x)->d.u
#define quantity(x) (x)->d.q
#define measure(x)  (x)->d.m
#define amount(x)   (x)->d.a

struct nsa_grapheme
{
  char *num;
  char *unit;
  int overt;
  struct nsa_system *is_sysdet;
  struct nsa_text_ref *text;
};
#define grapheme_num(x)      (x)->d.g->num
#define grapheme_overt(x)    (x)->d.g->overt
#define grapheme_is_sysdet(x)(x)->d.g->is_sysdet
#define grapheme_text_ref(x) (x)->d.g->text
#define grapheme_unit(x)     (x)->d.g->unit

/* Integers are always stored with a denominator of 1 */
struct nsa_count
{
  long long num;
  int den;
  int base;
  enum nsa_sex_axis axis;
  enum nsa_sex_form form;
  int implicit;
};
#define count_num(x)  (x)->d.c->num
#define count_den(x)  (x)->d.c->den
#define count_axis(x) (x)->d.c->axis
#define count_form(x) (x)->d.c->form
#define count_base(x) (x)->d.c->base

struct nsa_unit
{
  char *name;
  struct nsa_step *step; /* the determinate step, if system is unambiguous */
  List *cands; /* the candidate steps, while we're processing a measure */
  int implicit;
};
#define unit_name(x)  (x)->d.u->name
#define unit_cands(x) (x)->d.u->cands
#define unit_implicit(x) (x)->d.u->implicit
#define unit_step(x)  (x)->d.u->step

struct nsa_quantity
{
  struct nsa_token* count;
  struct nsa_token* unit;
  struct nsa_count* aev;
};
#define quantity_aev(x)   (x)->d.q->aev
#define quantity_count(x) (x)->d.q->count
#define quantity_unit(x)  (x)->d.q->unit
#define quantity_cands(x) unit_cands(quantity_unit(x))

struct nsa_measure
{
  struct nsa_system*system;
  List *cands;
  struct nsa_count *aev;
  struct nsa_token *sysdet; /* gur etc. */
  struct nsa_count *mev;
  char *rendered_mev;
};
#define measure_aev(x)          (x)->d.m->aev
#define measure_cands(x)        (x)->d.m->cands
#define measure_mev(x)          (x)->d.m->mev
#define measure_rendered_mev(x) (x)->d.m->rendered_mev
#define measure_sysdet(x)       (x)->d.m->sysdet
#define measure_system(x)       (x)->d.m->system

struct nsa_xcl_data
{
  const char *sysname;
  struct nsa_count *aev;
  const char *aeu; /*NB: NOT HANDLED YET*/
  struct nsa_count *mev;
  const char *meu;
  const char *rendered_mev;
  const char *comh;
  const char *sysdet;
};

struct nsa_commodity
{
  const char *head; /* note that this token is also stored in 
		       children[0] of the parent token structure */
  List *cands;
};
#define commodity_head(x)  (x)->d.o->head
#define commodity_cands(x) (x)->d.o->cands

struct nsa_amount
{
  struct nsa_token *measure;
  struct nsa_token *commodity;
};

#define amount_measure(x)   (x)->d.a->measure
#define amount_commodity(x) (x)->d.a->commodity

struct nsa_system
{
  const char *n;
  List *steps;
  struct nsa_count *mev;
  const char *meu;
};

struct nsa_context
{
  Hash_table *systems;
  Hash_table *step_index; /* index of all the step-names;
			     the value of each entry is a list 
			     of struct nsa_step *'s. */
  Hash_table *sysdets;
  Hash_table *syskeys;
  Hash_table *comheads;
  Hash_table *gal2_tokens;
  Hash_table *igigal_keys;
  Hash_table *la2_tokens;
  Hash_table *morph_suffixes;
  Hash_table *sexfracs;
  void *user;
  struct npool *cpool;
};

struct nsa_hash_data
{
  List *cands;
  Hash_table *continuations;
};

struct nsa_step
{
  const char *atf; /* CDLI transliteration in ATF, e.g., gin2 */
  const char *utf; /* CDLI transliteration in utf8 */
  const char *ecf; /* ePSD CITATION FORM, e.g., giŋ */
  const char *etr; /* ePSD transliteration, e.g., giŋ₄ */
  enum nsa_sex_axis sex; /* does this step take asz or disz? */
  struct nsa_system *sys;
  struct nsa_count *aev;
  int level;
  List_node *node; /* allow navigation from step to sequence */
};

enum nsa_rtypes { NSA_R_PARSED , NSA_R_UNPARSED };
struct nsa_result_chunk
{
  enum nsa_rtypes type;
  struct nsa_amount *amount;
  struct nsa_text_ref **refs;
  struct linkset *xlink;
};

struct nsa_result
{
  int success;
  const char *label;
  struct nsa_result_chunk **results;
  int ambigs;
  struct nsa_parser *parser;
};

struct nsa_parser
{
  List *toks;
  struct nsa_context *context;
  struct npool *pool;
};

extern const char const *sex_axis_str[];
extern const char const *sex_form_str[];

extern int label;
extern int printres;
extern int verbose;
extern int xlink_results;
extern int lnum;
extern struct nsa_system *sex_system;
extern struct nsa_context * nsa_init(const char *file);
extern void nsa_term(void);
extern struct nsa_result *nsa_parse_line(struct nsa_context *cp, const char *lp);

extern struct nsa_parser *nsa_parse_init(struct nsa_context *cp);
extern void nsa_parse_term(struct nsa_result *rp);
extern void nsa_token(struct nsa_parser *p, enum nsa_ptypes type, void *ref, const char *t);
extern void nsa_parse(struct nsa_parser *p);

extern void nsa_print(struct nsa_result *resp, FILE*fp);
extern void nsa_mem_die(void);
extern void nsa_show_toks(List *toks);
extern const unsigned char *nsa_grapheme_text(struct nsa_token *tx);
extern struct nsa_sex_tab *sexnum(register const char *,register unsigned int);
extern void nsa_debug_delimiter(const char *msg);
extern void nsa_set_debug_fp(FILE *fp);
extern char *nsa_modern(struct nsa_count *mev, const char *meu);
extern char *nsa_decimal(struct nsa_count *c);
extern struct nsa_result *nsa_create_result(struct nsa_parser *p);
extern List *get_text_refs(struct nsa_token *,List *,enum nsa_ttypes);
extern unsigned char *ref_text(struct nsa_text_ref*text);
extern void show_sys_list(List *lp, FILE *fp);
extern const char *show_sys_list_str(List *lp);
extern void find_refs(struct nsa_token *t, List *g, const char *role);
extern void nsa_text_input(struct nsa_context *cp, FILE *in);
extern void nsa_xml_input(struct nsa_context *cp, const char *infile);
extern void nsa_xcl_input(struct xcl_context *xcp, struct nsa_context *cp);
extern void nsa_xcl_warning(struct xcl_context *xcp, struct xcl_l*head, const char *fmt,...);
extern void nsa_xcl_dump(FILE *,struct nsa_xcl_data *);
extern struct nsa_xcl_data *nsa_xcl_info(struct nsa_amount *a);

extern struct nsa_token ** new_children(int n);
extern struct nsa_sex_tab *new_sex_tab(void);
extern struct nsa_text_ref *new_text_ref(void);
extern struct nsa_token *new_token(void);
extern struct nsa_grapheme *new_grapheme(void);
extern struct nsa_unit *new_unit(void);
extern struct nsa_count *new_count(void);
extern struct nsa_quantity *new_quantity(void);
extern struct nsa_measure *new_measure(void);
extern struct nsa_xcl_data *new_xcl_data(void);
extern struct nsa_commodity *new_commodity(void);
extern struct nsa_amount *new_amount(void);
extern struct nsa_system *new_system(void);
extern struct nsa_context *new_context(void);
extern struct nsa_hash_data *new_hash_data(void);
extern struct nsa_step *new_step(void);
extern struct nsa_result_chunk *new_result_chunk(void);
extern struct nsa_result *new_result(void);
extern struct nsa_parser *new_parser(void);

extern void nsa_del_token(struct nsa_token *);
extern void nsa_del_system(struct nsa_system *);
extern void nsa_del_context(struct nsa_context *);
extern void nsa_del_result(struct nsa_result *);
extern void nsa_del_parser(struct nsa_parser *);
extern void nsa_del_xcl_data(struct nsa_xcl_data *p);

extern void render_token(struct nsa_token *t, FILE *fp);
extern void nsa_show_tokens(struct nsa_parser *p, FILE *fp);
extern void nsa_divider(const char *mess, FILE *fp);

extern void nsa_sexnum(struct nsa_parser *p);
extern void nsa_ucount(struct nsa_parser *p);
extern void nsa_quantity(struct nsa_parser *p);
extern void nsa_measure(struct nsa_parser *p);
extern void nsa_measure_wrapup(struct nsa_token *m, struct nsa_parser *p);

extern void nsa_amount(struct nsa_parser *p);
extern struct nsa_token *create_unit(struct nsa_parser *p, 
				     const char *n,
				     struct nsa_token *t);

extern void nsa_add_frac(struct nsa_count *n1, struct nsa_count *n2);
extern void nsa_mul_frac(struct nsa_count *n1, struct nsa_count *n2);
extern struct nsa_count *nsa_parse_count(const char *n, int sign);
extern const char *nsa_trim_morph(struct nsa_context *c,const char *s);
extern struct nsa_token *nsa_last_grapheme(List *toks);
extern struct nsa_token *nsa_last_unit(List *toks);
extern struct nsa_token *nsa_next_grapheme(List *toks);
extern void nsa_append_child(struct nsa_token *parent, struct nsa_token *child);

extern void nsa_system_override_set(const char *sys);
extern const char *nsa_system_override_get(void);

#endif/*_NSA_H*/

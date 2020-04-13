#ifndef _PROPS_H
#define _PROPS_H
/*#include "langtok.h"*/
#include "xcl.h"

struct props_prop
{
  int maxlem;
  int where;
  int headshift;
  const unsigned char *name;
  const unsigned char *group;
  const unsigned char *value;
};

struct props_term
{
  unsigned char *ngram;
  struct props_prop **props;
  int props_used;
#if 0
  int props_alloced;
#endif
};

struct props_lang_data
{
  struct props_term **term;
  struct langcore *lang;
  int terms_used;
#if 0
  int terms_alloced;
#endif
};

struct props_context
{
  const char *name;
  Hash_table *heads;
  Hash_table *todo;
  struct NL_context **     nlcps;
  struct props_lang_data **langs;
  struct mb *mb_props_terms;
  struct mb *mb_props_props;
  struct mb *mb_props_lang_datas;
  struct npool *pool;
};

struct props_head
{
  struct xcl_context *xc;
  struct xcl_l *lp;
  struct props_prop *pp;
};

struct prop_ref_cache
{
  struct xcl_l *prevs[3];
  struct xcl_l *nexts[3];
};

struct prop
{
  const unsigned char *group;
  const unsigned char *name;
  const unsigned char *value;
  int ngram_id;
  const char *ref;
  const char *p;
  const char *xml_id;   /* set for props by cbd2xff */
  int freq;		/* frequency of prop in corpus */
  struct prop *next;
};

enum pd_type { pd_type_flag , pd_type_ref , pd_type_string , pd_type_token };

struct propdef
{
  const unsigned char *name;
  const unsigned char *group;
  enum pd_type type;
  int multi;
};

typedef void props_analyzer(struct xcl_context*xc,struct props_context*pcp);

extern void props_exec(struct xcl_context *xc, const char *name, props_analyzer analyzer);
extern struct props_context *props_init(struct xcl_context *xc, const char *name);
extern void props_term(struct props_context *pcp);
extern void props_decorate(struct xcl_context *xc, struct props_context *pcp);
extern void props_ngram_match(struct xcl_context *xc, struct ML *mlp);
extern struct xcl_l *props_find_node(struct prop_ref_cache *cachep, 
				     struct xcl_l *start, int where, int maxlem);
extern void props_head_cue(struct xcl_context *xc, 
		    struct props_context *pcp, struct xcl_l *lp, struct props_prop *pp);
extern void props_head_reg(struct props_context *pcp, struct xcl_l *lp);
extern void props_heads(struct xcl_context *xc,struct xcl_l *lp);
extern void props_link(struct xcl_c *cp, int begin, int end, const char *name, const char *role);

extern int props_add_prop(struct ilem_form *f, const unsigned char *group,
			  const unsigned char *name, const unsigned char *value, 
			  const char *ref, const char *xml_id, const char *pref,
			  int ngram_id);
extern void props_dump_props(struct ilem_form *f, FILE *fp);
extern struct prop*props_find_prop(struct ilem_form *f, const unsigned char *name, const unsigned char *value);
extern struct prop*props_find_prop_group(struct ilem_form *f, const unsigned char *group);

extern struct prop*props_add_prop_sub(struct prop *p, const unsigned char *group,
			  const unsigned char *name, const unsigned char *value, 
			  const char *ref, const char *xml_id, const char *pref,
			  int ngram_id);
extern void props_dump_props_sub(struct prop *p, FILE *fp);
extern struct prop*props_find_prop_sub(struct prop *p, const unsigned char *name, const unsigned char *value);
extern struct prop*props_find_prop_group_sub(struct prop *p, const unsigned char *group);

void props_auto_init(void);
void props_auto_term(void);
struct propdef *props_auto_name(const unsigned char *value);

void xcl_props_init(void);
void xcl_props_term(void);

extern struct prop_ref_cache prop_cache;

#endif/*_PROPS_H*/

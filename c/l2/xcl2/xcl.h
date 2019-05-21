#ifndef _XCL_H
#define _XCL_H
#include <hash.h>

struct xcl_c;
struct xcl_l;
struct xcl_context;

#include "links.h"

enum xcl_load_sigs { XCL_LOAD_SANS_SIGSETS , 
		     XCL_LOAD_WITH_SIGSETS };

enum xcl_node_types { xcl_node_c , xcl_node_d , xcl_node_l };

enum xcl_c_types { xcl_c_text , xcl_c_discourse , xcl_c_chunk , 
		   xcl_c_sentence , xcl_c_clause , xcl_c_phrase ,
		   xcl_c_line_var , xcl_c_word_var ,
		   xcl_c_top };
enum xcl_d_types { xcl_d_line_start , xcl_d_break , 
		   xcl_d_cell_start , xcl_d_cell_end ,
		   xcl_d_field_start , xcl_d_field_end ,
		   xcl_d_gloss_start , xcl_d_gloss_end ,
		   xcl_d_punct ,
		   xcl_d_object , xcl_d_surface , xcl_d_column ,
		   xcl_d_nonw , xcl_d_nonx ,
		   xcl_d_top };

enum sb_status	 { no_sb , sb };

enum ll_types    { ll_none , ll_ambig , ll_finds , ll_bad , ll_top };

struct xcl_c
{
  enum xcl_node_types node_type;
  enum xcl_c_types type;
  const char *subclass;
  const char *classtype;
  const char *subtype;
  const char *id;
  const char *ref;
  const char *label;
  struct xcl_c *parent;
  struct xcl_context *xc;
  Hash_table *meta;
  Hash_table *ancestor_meta;
  int lnum;
  int nchildren;
  int children_alloced;
  int level;
  int bracketing_level;
  union xcl_u*children;
};

struct xcl_d
{
  enum xcl_node_types node_type;
  enum xcl_d_types type;
  const char *subtype;
  struct xcl_context *xc;
  enum sb_status sb;
  const char *xml_id;
  int lnum;
  const char *ref;
  const char *label;
};

#include "f2.h"

struct xcl_l
{
  enum xcl_node_types node_type;
  const char *xml_id;
  int lnum;
  const char *ref;
  const char *inst;
  const char *sig;
  struct xcl_c *parent;
  struct xcl_context *xc;
  struct ilem_form *f;
  struct xcl_l *cof_head;
  List *cof_tails;
  int nth; 		/* index into parent's child array */
  int ro; 		/* used by the ngram processor */
  void *user;		/* caller can attach arbitrary data to lemma node */
  char *psurefs;        /* normally a singleton idref to a PSU linkset */

#if 0  
  int nfinds;
  const unsigned char **sigs_found;
#endif

  struct ilem_para *ante_para;
  struct ilem_para *post_para;
};

union xcl_u
{
  struct xcl_c *c;
  struct xcl_d *d;
  struct xcl_l *l;
};

struct NL_context;
struct props_context;
struct xcl_context
{
  struct xcl_c *root;
  struct xcl_c *curr;
  struct run_context *run;
  struct sig_context *sigs;
  struct props_context *props;
  const char *project;
  const char *langs;
  const char *textid;
  const char *file;
  struct linkbase *linkbase;
  struct npool *pool;
  Hash_table *psus;
  void *user;
};

struct xcl_tok_tab
{
  const char *name;
  enum xcl_node_types node_type;
  int subtype; /* generic b/c each node_type has its own enum */
};

struct xcl_chunk_spec 
{
  struct linkset *lsp;
  const char *from_id;
  const char *to_id;
  const char *sort_from;
  const char *sort_to;
  const char *type;
  const char *subtype;
  const char *ref;
};

typedef void xcl_c_fnc(struct xcl_context *,struct xcl_c*);
typedef void xcl_d_fnc(struct xcl_context *,struct xcl_d*);
typedef void xcl_l_fnc(struct xcl_context *,struct xcl_l*);

extern int sparse_lem, sparse_skipping;
extern int xcl_is_sparse_field(const char *f);
extern void xcl_set_sparse_fields(const char *f);
extern void xcl_set_sparse_skipping(const char *f);

extern struct xcl_context *text_xc;
extern const char *xcl_project, *system_project;
extern void xcl_anno(struct xcl_context *xc);
extern struct sigset *xcl_cache(void);
extern void xcl_chunk(struct xcl_context *xc, const char *xml_id, enum xcl_c_types t);
extern void xcl_chunk_end(struct xcl_context *xc);
extern const char *xcl_chunk_id(const char *idbase, enum xcl_c_types t, struct xcl_context *xc);
extern void xcl_chunk_insert(struct xcl_c *curr_c, const char *xml_id, const char *ref,
			     enum xcl_c_types t, const char *subtype,
			     int first_child, int ncopy);
extern void xcl_chunkify(struct xcl_context *xc);
extern void xcl_chunkify_part(struct xcl_context *xc, 
			      Hash_table *titles, Hash_table *roles, Hash_table *lnodes);
extern struct xcl_context *xcl_create(void);
extern Hash_table *xcl_create_meta(struct xcl_context *xc,const char *xml_id);
extern void xcl_destroy(struct xcl_context **xc);
extern void xcl_discontinuity(struct xcl_context *xc, const char *ref, enum xcl_d_types t, const char *st);
extern struct xcl_l* xcl_lemma(struct xcl_context *xc, const char *xml_id, 
			       const char *ref, struct ilem_form *fp, void *user,
			       enum ll_types t);
extern struct xcl_context *xcl_load(const char *xcl_fn, int setup_formsets);
extern void xcl_map(struct xcl_context *, xcl_c_fnc, xcl_c_fnc, xcl_d_fnc, xcl_l_fnc);
extern struct xcl_tok_tab *xcltok(register const char *str,register unsigned int len);

/*extern void xcl_serialize(struct xcl_context *xc, FILE *fp);*/
extern void x2_serialize(struct xcl_context *xc, FILE *f_xcl, int with_xcl_decl);
extern void xcl_serialize_form(struct ilem_form *f);

extern void xcl_set_project_forms(struct sigset *f);
extern void xcl_set_system_forms(const char *lang, struct sigset *forms);
extern void xcl_set_project_pns(struct sigset *forms);
extern void xcl_set_system_pns(struct sigset *forms);

extern void xcl_init(void);
extern void xcl_term(void);
extern void xcl_final_term(void);
extern struct xcl_context *xcl_get_global_context(void);
extern void xcl_unsentence(struct xcl_context *xc);
extern Hash_table *xcl_hash_lemm_meta(const char *const*lmeta, 
				      const char *xml_id, struct xcl_context *xc);

extern void xcl_load_langs(struct xcl_context *xc);
extern void xcl_clear_cache(void);

extern const char *sysproj(const char *lang);

extern struct xcl_l *xcl_head(union xcl_u *xu);

extern void xcl_insert_ub(struct xcl_context *xc, int nlem, enum xcl_c_types t, int level);
extern void xcl_fix_context(struct xcl_context *xc, const unsigned char *discourse);
extern void xcl_add_discourse(struct xcl_context *xc, const char *discourse);
extern void xcl_add_child(struct xcl_c*p, void *c, enum xcl_node_types type);

extern void sigs_dump_sigs(struct xcl_context *xcp, struct xcl_l *lp);

#endif /*_XCL_H*/

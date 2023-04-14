#ifndef GDL_H_
#define GDL_H_

#include <pool.h>
#include <tree.h>
#include "gvl.h"

#ifndef uccp
#define uccp unsigned const char *
#endif

#ifndef ccp
#define ccp const char *
#endif

#if 0
enum bracket {
  e_L_squ, e_R_squ,
  e_L_cur, e_R_cur,
  e_L_par, e_R_par,
  e_L_ang, e_R_ang,
  e_L_dbl_ang, e_R_dbl_ang,
  e_L_dbl_cur, e_R_dbl_cur,
  e_L_ang_par, e_R_ang_par,
  e_L_uhs , e_R_uhs,
  e_L_lhs , e_R_lhs,
  e_L_inl_dol , e_R_inl_dol,
  e_L_inl_cmt , e_R_inl_cmt
};
#endif

enum gdlpropvals { GP_ATTRIBUTE, GP_IMPLICIT, GP_TRACKING };

#if 0
/* This may not be necessary given the emerging GDL/GVL architecture; see gvl_g in gvl.h */
struct gdl_g {
  gvl_g g;		/* constant data from GVL */
  const char *flags;	/* instance data */
};
#endif

extern int gdl_corrq;
extern int curr_lang;

extern Pool *gdlpool;

extern void gdl_init(void);
extern void gdl_term(void);

extern void gdl_lex_init(const char *file, int line);
extern void gdl_setup_buffer(char *buf);
extern void gdl_set_tree(Tree *tp);
extern Tree *gdlparse_string(Mloc *m, char *s);
extern void gdlparse_init(void);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);

extern void gdlxml_setup(void);
extern void gdl_xml_handler(Node *np, void *xhp);

extern Node *gdl_pop(Tree *ytp, const char *s);
extern Node *gdl_push(Tree *ytp, const char *s);
extern void gdl_prop(Node *ynp, int p, int g, const char *k, const char *v);
extern void gdl_remove_q_error(Mloc m, Node *ynp);

extern Node *gdl_nongraph(Tree *ytp, const char *data);
extern Node *gdl_state(Tree *ytp, const char *data);
extern void gdl_cell(Tree *ytp, const char *span);
extern Node *gdl_delim(Tree *ytp, const char *data);
extern void gdl_field(Tree *ytp, const char *ftype);
extern Node *gdl_graph(Tree *ytp, const char *data);
extern Node *gdl_lang(Tree *ytp, const char *data);
extern const char *gdl_lexfld_name(const char *lftok);
extern void gdl_lexfld_init(void);
extern void gdl_mod(Tree *ytp, const char *data);
extern Node *gdl_punct(Tree *ytp, const char *data);
extern Node *gdl_listnum(Tree *ytp, const char *data);
extern Node *gdl_number(Tree *ytp, const char *data);
extern Node *gdl_barenum(Tree *ytp, const char *data);

extern void gdl_incr_qin(void);
extern void gdl_decr_qin(void);

#endif /*GDL_H_*/

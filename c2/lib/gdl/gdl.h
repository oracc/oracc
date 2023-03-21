#ifndef GDL_H_
#define GDL_H_

#include <pool.h>
#include <tree.h>

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

enum gprop { GP_BREAK , GP_FLAGS , GP_STATE };

struct gdl_prop {
  enum gprop type;
  const char *data;
  struct gdl_prop *next;
};

struct gdl_g {
  unsigned const char *c10e; /* canonicalized grapheme */
  unsigned const char *orig; /* original grapheme */
  unsigned const char *accn; /* canonicalized grapheme mapped through accnum */
  const char *type;
  const char *flags;
};

extern Pool *gdlpool;

extern void gdl_init(void);
extern void gdl_term(void);

extern void gdl_setup_buffer(char *buf);
extern void gdl_set_tree(Tree *tp);
extern Tree *gdlparse_string(char *s);
extern void gdlparse_init(void);
extern void gdlparse_reset(void);
extern void gdlparse_term(void);
extern void gdl_xml(FILE *fp, Tree *tp);

extern void gdl_append(Node *ynp, const char *s);
extern Node *gdl_delim(Tree *ytp, const char *data);
extern Node *gdl_graph(Tree *ytp, const char *data);
extern void gdl_prop(Node *ynp, enum gprop type, const char *s);

extern void gdl_pop(Tree *ytp, const char *s);
extern void gdl_push(Tree *ytp, const char *s);

#endif /*GDL_H_*/

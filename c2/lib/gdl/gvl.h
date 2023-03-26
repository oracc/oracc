#ifndef _GVL_H
#define _GVL_H

#include <tree.h>
#include "sll.h"

struct gvl_g {
  const char *type;
  unsigned const char *orig; /* grapheme as passed for validation; for
				a vq this is the vq as passed */
  unsigned const char *c10e; /* canonicalized grapheme; for use in
				orthographic forms (note: does not
				include aliasing) */
  unsigned const char *accn; /* grapheme passed through accnum--lazily done */
  const char *oid;  	     /* OID for sign */
  unsigned const char *sign; /* sign name for OID; for a vq this is the canonical vq */
  unsigned const char *utf8; /* UTF8 value for OID */
  unsigned const char *uhex; /* HEX value for OID */
  unsigned const char *mess; /* NULL if OK; if bad this is the error message for the grapheme */
  int status; 		     /* 0 for exactly right; 1 for required canonicalization; -1 on error */
};

typedef struct gvl_g gvl_g;

struct gvl_i {
  const char *n;	/* name of SL DB */
  Hash *sl;		/* in-memory signlist hash */
  Hash *h;		/* Hash of items validated */
  Memo *m;		/* Memory blocks for gvl_g nodes */
  Pool *p;		/* General purpose string pool */
  struct gvl_i *prev;
  struct gvl_i *next;
};

typedef struct gvl_i gvl_i;

#define QFIX (q_fixed ? (ccp)q_fixed : "")

extern gvl_i *curr_sl;

extern unsigned const char *gvl_lookup(unsigned const char *key);

extern gvl_i *gvl_setup(const char *project, const char *name);
extern void gvl_wrapup(const char *name);

extern gvl_i *gvl_i_find(const char *name);

extern gvl_g *gvl_validate(unsigned const char *g);

extern int gvl_is_sname(unsigned const char *g);
extern int gvl_is_value(unsigned const char *g);
extern unsigned const char *gvl_get_id(unsigned const char *g);
extern unsigned const char *gvl_get_sname(unsigned const char *g);
extern int gvl_looks_like_sname(unsigned const char *g);

extern unsigned const char *gvl_cuneify(unsigned const char *g);
extern unsigned const char *gvl_cuneify_gv(gvl_g *gp);
extern unsigned const char *gvl_ucode(gvl_g *gp);

extern unsigned char *gvl_s_c10e(const unsigned char *g, int *err);

extern wchar_t *g_wlc(wchar_t *w);
extern wchar_t *g_wuc(wchar_t *w);
extern unsigned char *g_lc(unsigned const char *g);
extern unsigned char *g_uc(unsigned const char *g);
extern unsigned char *base_of(const unsigned char *v);
extern const char *sub_of(int i);
extern wchar_t subdig_of(wchar_t w);
extern wchar_t vowel_of(wchar_t w);

extern int gvl_v_isupper(unsigned const char *v);

extern void gvl_iterator_pre_fnc(Node *np, void *user);
extern void gvl_iterator_post_fnc(Node *np, void *user);

extern void gvl_c(gvl_g *cp);
unsigned char *gvl_c_orig(Node *ynp);
unsigned char *gvl_c_c10e(Node *ynp);

extern gvl_g *gvl_s(Node *ynp);

extern int gvl_q(gvl_g *vp, gvl_g *qp, gvl_g *vq);

extern unsigned char *gvl_vmess(char *s, ...);

extern void gvl_compound(Node *ynp);
extern void gvl_simplexg(Node *ynp);
extern void gvl_valuqual(Node *ynp);

#endif/*_GVL_H*/

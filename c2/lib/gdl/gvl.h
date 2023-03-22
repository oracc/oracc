#ifndef _GVL_H
#define _GVL_H

#include <tree.h>
#include "sll.h"

struct gvl_g {
  unsigned const char *text; /* grapheme as passed for validation; for a vq this is the vq as passed */
  unsigned const char *accn; /* grapheme mapped through accnum */
  const char *type;
  const char *oid;  /* OID for sign */
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

extern unsigned const char *(*gvl_lookup)(unsigned const char *key);

extern gvl_i *gvl_setup(const char *project, const char *name);
extern void gvl_wrapup(const char *name);

extern gvl_i *gvl_i_find(const char *name);

extern gvl_g *gvl_validate(unsigned const char *g);

extern int gvl_is_sname(unsigned const char *g);
extern int gvl_is_value(unsigned const char *g);
extern unsigned char *gvl_tmp_key(unsigned const char *key, const char *field);
extern unsigned const char *gvl_get_id(unsigned const char *g);
extern unsigned const char *gvl_get_sname(unsigned const char *g);
extern int gvl_looks_like_sname(unsigned const char *g);

extern unsigned const char *gvl_cuneify(unsigned const char *g);
extern unsigned const char *gvl_cuneify_gv(gvl_g *gp);
extern unsigned const char *gvl_ucode(gvl_g *gp);

extern unsigned char *g_c10e(const unsigned char *g, int *err);
extern wchar_t *g_wlc(wchar_t *w);
extern wchar_t *g_wuc(wchar_t *w);
extern unsigned char *g_lc(unsigned const char *g);
extern unsigned char *g_uc(unsigned const char *g);
extern unsigned char *base_of(const unsigned char *v);
const char *sub_of(int i);

extern void gvl_iterator_pre_fnc(Node *np, void *user);
extern void gvl_iterator_post_fnc(Node *np, void *user);

extern int gvl_vq_c10e(gvl_g *gp, unsigned char **mess);
extern unsigned char *gvl_vmess(char *s, ...);

#endif/*_GVL_H*/

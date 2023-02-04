#ifndef _GVL_H
#define _GVL_H

#include <dbi.h>
#include <sllib.h>

struct gvl_g {
  unsigned const char *text; /* grapheme as passed for validation */
  const char *type;
  const char *oid;  /* OID for sign */
  unsigned const char *sign; /* sign name for OID */
  unsigned const char *utf8; /* UTF8 value for OID */
  unsigned const char *mess; /* NULL if OK; if bad this is the error message for the grapheme */
};

typedef struct gvl_g gvl_g;

struct gvl_i {
  int tsv;		/* 1 if tsv mode; 0 if dbi mode */
  const char *n;	/* name of SL DB */
  union {		/* look up data union */
    Dbi_index *d;	/* SL DB index */
    Hash_table *h;	/* in-memory hash table */
  } u;
  Hash_table *h;	/* Hash of items validated */
  struct mb *m;		/* Memory blocks for gvl_g nodes */
  struct npool *p;	/* General purpose string pool */
  struct gvl_i *prev;
  struct gvl_i *next;
};

typedef struct gvl_i gvl_i;

extern unsigned const char *(*gvl_lookup)(unsigned const char *key);

extern gvl_i *gvl_setup(const char *project, const char *name, int arg_tsv);
extern void gvl_wrapup(const char *name);

extern gvl_i *gvl_i_find(const char *name);

extern gvl_g *gvl_validate(unsigned const char *g);

extern int gvl_is_sname(unsigned const char *g);
extern int gvl_is_value(unsigned const char *g);
extern unsigned char *gvl_tmp_key(unsigned const char *key, const char *field);
extern unsigned const char *gvl_get_id(unsigned const char *g);
extern unsigned const char *gvl_get_sname(unsigned const char *g);

extern unsigned const char *gvl_cuneify(unsigned const char *g);
extern unsigned const char *gvl_cuneify_gv(gvl_g *gp);


#endif/*_GVL_H*/

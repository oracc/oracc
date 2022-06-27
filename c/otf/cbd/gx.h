#ifndef __GX_H__
#define __GX_H__ 1

#undef warning
#include "warning.h"
#include "atf.h"
#include "cuneify.h"
#include "npool.h"
#include "globals.h"
#include "list.h"
#include "hash.h"
#include "xpd2.h"
#include "f2.h"

#undef ucc
#define ccp const char *
#define ucc unsigned const char *
#define ucp unsigned char *

extern Hash_table *cbds;

struct cbd {
  unsigned const char *project;
  unsigned const char *lang;
  unsigned const char *name;  /* this is the name as given in @name */
  unsigned const char *trans;
  unsigned const char *iname; /* this is the internal name used as the hash key in cbds */
  struct npool *pool;
  struct xpd *xpd;
  List *letters;
  List *entries;
  Hash_table *simple;
  Hash_table *cofs;
  Hash_table *psus;
};

struct entry {
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
  unsigned const char *eid;
  unsigned const char *lang;
  Hash_table *b_pri;
  Hash_table *b_alt;
  Hash_table *b_sig;
  Hash_table *dcfs;
  List *forms;
  List *senses;
  List *aliases;
  List *bffs;
  List *bib;
  List *isslp;
  int plus;
  int usage;
  int compound;
  struct cbd *owner;
};

struct sense {
  unsigned const char *num;
  unsigned const char *sgw;
  unsigned const char *pos;
  unsigned const char *lng;
  unsigned const char *mng;
  unsigned const char *sid;
  struct entry *owner;
};

struct isslp {
  unsigned char *year;
  unsigned char *text;
  struct entry *owner;
};

struct cbdpos {
  const char *name;
  int val;
};

struct cbdtag {
  const char *name;
  void (*parser)(struct entry *e, unsigned char *s);
};

extern const char *errmsg_fn;

extern struct cbdtag *cbdtags(const char *str, size_t len);
extern unsigned char *tok(unsigned char *s, unsigned char *end);
extern unsigned char *form_sig(struct entry *e, struct f2 *f2p);

extern int parse_dcf(struct entry *e, unsigned char *s);
extern unsigned char **parse_header(struct cbd *c, unsigned char **ll);
extern unsigned char **parse_entry(struct cbd *c, unsigned char **ll);
void parse_cgp(struct entry *c, unsigned char *s);
void parse_bases(struct entry *e, unsigned char *s);
void parse_form(struct entry *e, unsigned char *s);
void parse_sense(struct entry *e, unsigned char *s);
void parse_alias(struct entry *e, unsigned char *s);
void parse_allow(struct entry *e, unsigned char *s);
void parse_inote(struct entry *e, unsigned char *s);
void parse_isslp(struct entry *e, unsigned char *s);
void parse_bff(struct entry *e, unsigned char *s);
void parse_parts(struct entry *e, unsigned char *s);
/*
void parse_(unsigned char *s);
*/
#endif

#ifndef __GX_H__
#define __GX_H__ 1

#undef warning
#include "warning.h"
#include "atf.h"
#include "gdl.h"
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

typedef struct {
  const char *file;
  int line;
} locator;

extern Hash_table *cbds;

struct cbd {
  locator l;
  unsigned const char *project;
  unsigned const char *lang;
  unsigned const char *name;  /* this is the name as given in @name */
  unsigned const char *trans;
  unsigned const char *iname; /* this is the internal name used as the hash key in cbds */
  struct npool *pool;
  struct xpd *xpd;
  List *letters;
  List *entries;
  List *edits;
  Hash_table *hentries;
  Hash_table *haliases;
  Hash_table *simple;
  Hash_table *cofs;
  Hash_table *psus;
};

struct cgp {
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
};

enum edit_t { ADD_E, ADD_S, DEL_E, DEL_S, REN_E, REN_S, MRG_E, MRG_S, TOP };

struct edit {
  locator *lp;
  enum edit_t type;
  struct cgp target;
  struct sense *sp;
  int force;
  void *owner;
};

struct entry {
  locator l;
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
  unsigned const char *eid;
  unsigned const char *lang;
  unsigned const char *spread;
  unsigned const char *closed;
  Hash_table *b_pri;
  Hash_table *b_alt;
  Hash_table *b_sig;
  Hash_table *b_allow;
  Hash_table *dcfs;
  List *forms;
  List *senses;
  List *aliases;
  List *bffs;
  List *bib;
  List *isslp;  
  int bang;
  int usage;
  int compound;
  struct cbd *owner;
  struct edit *ed;
};

struct alias {
  locator l;
  struct cgp *c;
  unsigned const char *spread;
  unsigned const char *closed;
};

struct sense {
  locator l;
  unsigned const char *num;
  unsigned const char *sgw;
  unsigned const char *pos;
  unsigned const char *lng;
  unsigned const char *mng;
  unsigned const char *sid;
  int bang;
  struct entry *owner;
  struct edit *ed;
};

struct isslp {
  locator l;
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
  void (*parser)(struct entry *e, unsigned char *s, locator *lp);
};

extern int check;
extern int entries;
extern int sigs;

extern const char *errmsg_fn;

extern struct cbdpos *cbdpos(const char *str, size_t len);
extern struct cbdtag *cbdtags(const char *str, size_t len);
extern unsigned char *tok(unsigned char *s, unsigned char *end);
extern unsigned char *form_sig(struct entry *e, struct f2 *f2p);
extern void untab(unsigned char *s);

extern unsigned char *cgp_cgp_str(struct cgp *c, int spread);
extern unsigned char *cgp_entry_str(struct entry *e, int spread);
extern unsigned char *cgp_str(unsigned const char *cf,
			      unsigned const char *gw,
			      unsigned const char *pos, int spread);
extern void cgp_entry(struct cgp *c, struct entry *e);
extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsize);
extern void cgp_parse(struct cgp *c, unsigned char *s, locator *lp);
extern unsigned char *check_bom(unsigned char *s);
extern unsigned char **setup_lines(unsigned char *ftext);

extern int edit_add(unsigned char **ll, struct entry *e);
extern int edit_check(struct cbd *c);

extern struct sense *parse_sense_sub(unsigned char *s, locator*lp);
extern int parse_dcf(struct entry *e, unsigned char *s);
extern unsigned char **parse_header(struct cbd *c, unsigned char **ll);
extern unsigned char **parse_entry(struct cbd *c, unsigned char **ll);

extern void parse_alias(struct entry *e, unsigned char *s, locator *lp);
extern void parse_allow(struct entry *e, unsigned char *s, locator *lp);
extern void parse_bases(struct entry *e, unsigned char *s, locator *lp);
extern void parse_bff(struct entry *e, unsigned char *s, locator *lp);
extern void parse_bib(struct entry *e, unsigned char *s, locator *lp);
extern void parse_disc(struct entry *e, unsigned char *s, locator *lp);
extern void parse_file(struct entry *e, unsigned char *s, locator *lp);
extern void parse_form(struct entry *e, unsigned char *s, locator *lp);
extern void parse_inote(struct entry *e, unsigned char *s, locator *lp);
extern void parse_isslp(struct entry *e, unsigned char *s, locator *lp);
extern void parse_note(struct entry *e, unsigned char *s, locator *lp);
extern void parse_parts(struct entry *e, unsigned char *s, locator *lp);
extern void parse_sense(struct entry *e, unsigned char *s, locator *lp);
extern void parse_collo(struct entry *e, unsigned char *s, locator *lp);
extern void parse_root(struct entry *e, unsigned char *s, locator *lp);
extern void parse_pl_id(struct entry *e, unsigned char *s, locator *lp);
extern void parse_pl_uid(struct entry *e, unsigned char *s, locator *lp);
extern void parse_pl_coord(struct entry *e, unsigned char *s, locator *lp);
extern void parse_prop(struct entry *e, unsigned char *s, locator *lp);
extern void parse_proplist(struct entry *e, unsigned char *s, locator *lp);
extern void parse_oid(struct entry *e, unsigned char *s, locator *lp);
extern void parse_phon(struct entry *e, unsigned char *s, locator *lp);
extern void parse_equiv(struct entry *e, unsigned char *s, locator *lp);
extern void parse_stems(struct entry *e, unsigned char *s, locator *lp);

/*
extern void parse_(unsigned char *s, locator *lp);
*/
#endif

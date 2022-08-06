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
#include "memblock.h"
#include "grammar.h"
#include "grammar.tab.h"

#undef ucc
#define ccp const char *
#define ucc unsigned const char *
#define ucp unsigned char *

typedef void (*iterator_fnc)(void*);

#if 1
typedef struct YYLTYPE locator;
#else
typedef struct {
  const char *file;
  int line;
  int col_beg;
  int col_end;
} locator;
#endif

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
  List *proplists;
  Hash_table *hentries;
  Hash_table *haliases;
  Hash_table *simple;
  Hash_table *cofs;
  Hash_table *psus;
  struct mb *aliasmem;
  struct mb *allowmem;
  struct mb *cgpmem;
  struct mb *editmem;
  struct mb *equivmem;
  struct mb *formsmem;
  struct mb *loctokmem;
  struct mb *metamem;
  struct mb *metaordermem;
  struct mb *partsmem;
  struct mb *pleiadesmem;
  struct mb *sensesmem;
  struct mb *tagmem;
};

struct loctok {
  locator l;
  unsigned char *tok;
  unsigned char *lang;
};

struct cgp {
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
  unsigned const char *tight;
  unsigned const char *loose;
};

enum edit_t { ADD_E, ADD_S, DEL_E, DEL_S, REN_E, REN_S, MRG_E, MRG_S, TOP };

struct edit {
  locator *lp;
  enum edit_t type;
  struct cgp *target;
  unsigned char *why;
  struct sense *sp;
  int force;
  void *owner;
};

struct entry {
  locator l;
  struct cgp *cgp;
  unsigned const char *eid;
  unsigned const char *lang;
  Hash_table *b_pri;
  Hash_table *b_alt;
  Hash_table *b_sig;
  Hash_table *b_allow;
  List *allows; /* struct allow * */
  List *aliases;
  List *dcfs;
  Hash_table *hdcfs;
  struct parts *parts;
  List *bases; /* list of base components in @bases; list data is another list, first element is pri, rest are alt */
  List *forms;
  List *senses;
  Hash_table *hsenses; /* needed for building cbd from sigs */
  struct tag *phon;
  struct tag *root;
  List *stems;
  struct tag *disc;
  int bang;
  int beginsenses;
  int usage;
  int compound;
  struct cbd *owner;
  struct edit *ed;
  struct meta *meta;
};

struct allow {
  locator l;
  unsigned char *lhs;
  unsigned char *rhs;
  unsigned char *lsig;
  unsigned char *rsig;
};

struct meta {
  List *order;
  List *bib;
  List *collo;
  List *equiv;
  List *inote;
  List *isslp;  
  List *note;
  List *oid;
  List *prop;
  List *pleiades;
  List *rel;
};

struct metaorder {
  locator l;
  int tok;
  const char *name;
  void *val;
};

struct alias {
  locator l;
  struct cgp *cgp;
};

struct parts {
  locator l;
  List *cgps; /* list_free(cgps,NULL) when freeing cbd */
};

struct sense {
  locator l;
  unsigned const char *num;
  unsigned const char *sgw;
  unsigned const char *pos;
  unsigned const char *lng;
  unsigned const char *mng;
  unsigned const char *sid;
  struct tag *disc;
  int bang;
  struct entry *owner;
  struct edit *ed;
  struct meta *meta;
};

struct equiv {
  locator l;
  unsigned char *lang;
  unsigned char *text;
};

struct isslp {
  locator l;
  unsigned char *year;
  unsigned char *text;
  struct entry *owner;
};

struct pleiades {
  locator l_coord;
  locator l_id;
  locator l_uid;
  unsigned char *coord;
  unsigned char *id;
  unsigned char *uid;
};

struct cbdpos {
  const char *name;
  int val;
};

struct tag {
  locator l;
  const char *name;
  unsigned char *val;
};

struct cbdtag {
  const char *name;
  void (*parser)(struct entry *e, unsigned char *s, locator *lp);
};

extern int check;
extern int entries;
extern int keepgoing;
extern int sigs;

extern struct cbd *curr_cbd;
extern struct entry *curr_entry;
extern struct alias *curr_alias;
extern struct parts *curr_parts;

extern const char *errmsg_fn;

struct cbd * cbd_init(void);
void cbd_setup(struct cbd*c);

struct entry * entry_init(YYLTYPE l, struct cbd* c);

void common_init(void);
void common_term(void);

extern struct cbdpos *cbdpos(const char *str, size_t len);
extern struct cbdtag *cbdtags(const char *str, size_t len);
extern unsigned char *tok(unsigned char *s, unsigned char *end);
extern unsigned char *form_sig(struct entry *e, struct f2 *f2p);
extern void untab(unsigned char *s);

extern void cgp_init(struct cgp *c, unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern unsigned const char *cgp_entry_str(struct entry *e, int spread);
extern const unsigned char *cgp_str(struct cgp *cp, int spread);
extern void cgp_entry(struct cgp *c, struct entry *e);
extern unsigned char *slurp(const char *caller, const char *fname, ssize_t *fsize);
extern void cgp_parse(struct cgp *c, unsigned char *s, locator *lp);
extern unsigned char *check_bom(unsigned char *s);
extern unsigned char **setup_lines(unsigned char *ftext);

extern int edit_add(unsigned char **ll, struct entry *e);
extern int edit_check(struct cbd *c);
extern int edit_script(struct cbd *c);

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

extern List *cgp_get_all(void);
extern struct cgp*cgp_get_one(void);
extern void cgp_save(unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern void entry_edit(struct entry *e, char type);
extern struct sense *sense_edit(struct entry *e, char type);
extern void edit_save(struct entry *e, char ctxt, char type);
extern void edit_why(struct entry *e, char *why);
extern struct alias *alias_init(YYLTYPE l, struct entry *e);
extern void dcf_init(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg);
extern struct parts *parts_init(YYLTYPE l, struct entry *e);
extern struct tag *tag_init(YYLTYPE l, struct entry *e, const char *name, unsigned char *val);

extern void identity(struct cbd*c);
extern void validator(struct cbd*c);

extern void proplist_add(struct cbd *c, char *text);
extern void allow_init(YYLTYPE l, struct entry *e, unsigned char *lhs, unsigned char *rhs);
extern void stem_init(YYLTYPE l, struct entry *e, unsigned char *stem);
extern struct sense *sense_init(YYLTYPE l, struct entry *e);

extern void bases_pri_save(YYLTYPE l, struct entry *e, unsigned char *lang, unsigned char *p);
extern void bases_alt_save(YYLTYPE l, struct entry *e, unsigned char *p);
extern struct loctok *loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok);

extern struct f2 *form_init(YYLTYPE l, struct entry *e);

extern struct meta *meta_init(struct entry *e);
extern void meta_add(YYLTYPE l, struct entry *e, struct meta *mp, int tok, const char *name, void *val);

extern struct equiv *equiv_init(struct entry *e, unsigned char *lang, unsigned char *text);
extern struct pleiades *pleiades_init(struct entry *e,
				      unsigned char *coord, unsigned char *id, unsigned char *uid);

extern void msglist_init(void);
extern void msglist_print(FILE *fp);
extern void msglist_term(void);
extern char *msglist_loc(YYLTYPE *locp);
extern void msglist_err(YYLTYPE *locp, char *s);
extern void msglist_verr(YYLTYPE *locp, char *s, ...);
extern void msglist_averr(YYLTYPE *locp, char *s, va_list ap);
extern void msglist_print(FILE *fp);

#endif

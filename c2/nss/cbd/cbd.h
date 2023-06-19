#ifndef CBD_H_
#define CBD_H_ 1

#include "hash.h"
#include "memo.h"
#include "pool.h"
#include "mesg.h"
#include "form.h"

/* Common grammar header file for cbd.l and cbd.y */
#include "cbdyacc.h"

#undef ucc
#define ccp const char *
#define ucc unsigned const char *

#ifndef ucp
#define ucp unsigned char *
#endif

typedef void (*iterator_fnc)(void*);
extern Hash *cbds;

struct cbd {
  Mloc l;
  unsigned const char *project;
  unsigned const char *lang;
  unsigned const char *name;  /* this is the name as given in @name */
  unsigned const char *trans;
  unsigned const char *version;
  unsigned const char *iname; /* this is the internal name used as the hash key in cbds */
  Pool *pool;
  struct xpd *xpd;
  List *letters;
  List *entries;
  List *edits;
  List *propss;
  List *reldefs;
  Hash *hentries;
  Hash *haliases;
  Hash *simple;
  Hash *cofs;
  Hash *psus;
  char *i18nstr; 	/* content of the @i18n header tag */
  List *i18n;		/* list of i18n langs from @i18n tag in header */
  Memo *aliasmem;
  Memo *allowmem;
  Memo *cgpmem;
  Memo *editmem;
  Memo *entrymem;
  Memo *equivmem;
  Memo *formsmem;
  Memo *i18nmem;
  Memo *loctokmem;
  Memo *locatormem;
  Memo *metamem;
  Memo *metaordermem;
  Memo *partsmem;
  Memo *pleiadesmem;
  Memo *sensesmem;
  Memo *tagmem;
  Memo *taglmem;
};

/* Store translated data */
struct i18n {
  Mloc l;
  const char *lang;
  void *data;
};

struct loctok {
  Mloc l;
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
  Mloc *lp;
  enum edit_t type;
  struct cgp *target;
  unsigned char *why;
  struct sense *sp;
  int force;
  void *owner;
};

struct entry {
  Mloc l;
  struct cgp *cgp;
  unsigned const char *eid;
  unsigned const char *lang;
  Hash *b_pri;
  Hash *b_alt;
  Hash *b_sig;
  Hash *b_allow;
  List *allows; /* struct allow * */
  List *aliases;
  List *dcfs;
  Hash *hdcfs;
  List *parts; /* list of struct parts * */
  List *bases; /* list of base components in @bases; list data is
		  another list, first element is pri, rest are alt
		  pri/alt data are struct loctok
	        */
  List *forms;
  List *senses;
  Mloc *begin_senses;
  Mloc *end_senses;
  Mloc *end_entry;
  Hash *hsenses; /* needed for building cbd from sigs */
  struct tag *phon;
  struct tag *root;
  List *stems;
  struct tag *disc;
  int bang;
  int usage;
  int compound;
  struct cbd *owner;
  struct edit *ed;
  struct meta *meta;
  List *gwls; /* list of struct i18n with (char*) data */
  List *discls; /* list of struct i18n with (char*) data; one day this will be OTF inline tree */
};

struct allow {
  Mloc l;
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
  Mloc l;
  int tok;
  const char *name;
  void *val;
};

struct alias {
  Mloc l;
  struct cgp *cgp;
};

struct parts {
  Mloc l;
  List *cgps; /* list_free(cgps,NULL) when freeing cbd */
};

struct sense {
  Mloc l;
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
  List *sensels; /* list of translated senses with (struct sense *) data--struct has l and lng so i18n not needed */
};

struct equiv {
  Mloc l;
  unsigned char *lang;
  unsigned char *text;
};

struct isslp {
  Mloc l;
  unsigned char *year;
  unsigned char *text;
  struct entry *owner;
};

struct pleiades {
  Mloc l_id;
  Mloc l_coord;
  unsigned char *id;
  unsigned char *coord;
  List *pl_aliases; /* list of struct loctok * so pl_aliases could use %lang */
};

struct cbdpos {
  const char *name;
  int val;
};

struct cbdrws {
  const char *name;
  const char *lang;
};

struct tag {
  Mloc l;
  const char *name;
  unsigned char *val;
};

struct tagl {
  Mloc l;
  unsigned char *data;
  List *i18n; /* List of i18ns translating this tag; i18n->data is char* */
};

struct cbdtag {
  const char *name;
  int tok;
  int eol_sp;
  int ss;
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

extern void o_tg2(struct cbd*c);
extern void o_xc2(struct cbd*c);
extern void validator(struct cbd*c);

extern struct cbdpos *cbdpos(const char *str, size_t len);
extern struct cbdrws *cbdrws(const char *str, size_t len);
extern int edit_script(struct cbd *c);
extern void cgp_parse(struct cgp *c, unsigned char *s, Mloc *lp);
extern unsigned char *cbd_form_sig(struct entry *e, Form *f2p);

extern struct cbdtag *cbdtags(const char *str, size_t len);
extern List *cgp_get_all(void);
extern struct cgp*cgp_get_one(void);
extern void cgp_save(unsigned char *cf, unsigned char *gw, unsigned char *pos);
extern struct loctok *loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok);

extern struct alias *bld_alias(YYLTYPE l, struct entry *e);
extern void bld_allow(YYLTYPE l, struct entry *e, unsigned char *lhs, unsigned char *rhs);
extern void bld_bases_pri(YYLTYPE l, struct entry *e, unsigned char *lang, unsigned char *p);
extern void bld_bases_alt(YYLTYPE l, struct entry *e, unsigned char *a);
extern struct cbd *bld_cbd(void);
extern void bld_cbd_setup(struct cbd *c);
extern void bld_cbd_term(struct cbd *c);
extern List *bld_cmt_append(List *to, List *from);
extern void bld_cmt_queue(Mloc *lp, unsigned char *cmt);
extern void bld_dcf(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg);
extern void bld_discl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text, int e_or_s);
extern void bld_edit(struct entry *e, char ctxt, char type);
extern void bld_edit_entry(struct entry *e, char type);
extern struct sense *bld_edit_sense(struct entry *e, char type);
extern void bld_edit_why(struct entry *e, char *why);
extern struct entry *bld_entry(YYLTYPE l, struct cbd *c);
extern void bld_entry_cgp(struct entry *e);
extern struct equiv *bld_equiv(struct entry *e, unsigned char *lang, unsigned char *text);
extern Form *bld_form(YYLTYPE l, struct entry *e);
extern void bld_form_setup(struct entry *e, Form *fp);
extern void bld_gwl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text);
extern Mloc *bld_locator(YYLTYPE lp);
extern struct loctok *bld_loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok);
extern struct meta *bld_meta_create(struct entry *e);
extern void bld_meta_add(YYLTYPE l, struct entry *e, struct meta *mp, int tok, const char *name, void *val);
extern void bld_note(YYLTYPE l, struct entry *e, struct meta *curr_meta, unsigned char *text);
extern void bld_notel(YYLTYPE l, struct entry *e, struct meta *curr_meta, const char *lang, unsigned char *text);
extern struct parts *bld_parts(YYLTYPE l, struct entry *e);
extern struct pleiades *bld_pl_id(YYLTYPE l, struct entry *e, unsigned char *id);
extern void bld_pl_coord(YYLTYPE l, struct pleiades *p, unsigned char *coord);
extern void bld_pl_alias(YYLTYPE l, struct pleiades *p, const char *lang, unsigned char *alias);
extern void bld_props(struct cbd *c, char *text);
extern void bld_reldef(struct cbd *c, char *text);
extern struct sense *bld_sense(YYLTYPE l, struct entry *e);
extern struct sense *bld_sensel(YYLTYPE l, struct entry *e);
extern void bld_stem(YYLTYPE l, struct entry *e, unsigned char *stem);
extern struct tag *bld_tag(YYLTYPE l, struct entry *e, const char *name, unsigned char *val);

#endif/*CBD_H_*/


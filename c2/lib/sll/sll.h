#ifndef SLL_H_
#define SLL_H_

#include <hash.h>
#include <pool.h>
#include <dbi.h>

typedef List *(sll_get_fnc)(const char *key);

enum sll_t { SLL_ANY, SLL_ID, SLL_SN, SLL_V };

struct sllext
{
  const char *name;
  const char *pre;
  const char *pst;
  enum sll_t type;
  sll_get_fnc *fnc;
};

#undef ccp
#define ccp const char *
#undef ucp
#define ucp unsigned char *
#undef uccp
#define uccp unsigned const char *

extern int sll_trace;
extern Hash *sll_sl;
extern const char *oracc;
extern Pool *sllpool;
extern int sll_raw_output;
extern int sll_trace;

extern const char *wcaller, *wextension, *wgrapheme, *wproject, *correctedg;

extern void sll_init(void);
extern void sll_term(void);

extern Hash *sll_init_t(const char *project, const char *name);
extern void sll_term_t(Hash *h);

extern Dbi_index *sll_init_d(const char *project, const char *name);
extern void sll_term_d(Dbi_index *d);

extern int sll_has_sign_indicator(unsigned const char *g);
extern void sll_init_si(void);

extern void sll_cli_handler(unsigned const char *key);
extern void sll_web_handler(const char *wcaller, const char *wproject, const char *wgrapheme, const char *wextension);
extern unsigned char *sll_strip_pp(unsigned const char *g);
extern unsigned char *sll_snames_of(unsigned const char *oids);
extern unsigned const char *sll_lookup(unsigned const char *key);
extern unsigned const char *sll_lookup_t(unsigned const char *key);
extern unsigned const char *sll_lookup_d(Dbi_index *dbi, unsigned const char *key);
extern void sll_set_sl(Hash *sl);
extern unsigned char *sll_try_h(const char *oid, unsigned const char *g);
extern unsigned char *sll_v_from_h(const unsigned char *b, const unsigned char *qsub);
extern unsigned char *sll_tmp_key(unsigned const char *key, const char *field);

extern const char *sll_get_name(unsigned char *k);
extern const char *sll_get_oid(unsigned char *k);
extern List *sll_get_cpd(const char *k);
extern List *sll_get_forms(const char *k);
extern List *sll_get_h(const char *k);
extern List *sll_get_one(const char *k);
extern List *sll_get_one_n(const char *k);
extern List *sll_get_one_o(const char *k);

extern struct sllext *sllext (register const char *str, register size_t len);

extern unsigned const char *sll_ext_check(unsigned const char *k, enum sll_t t);
extern List *sll_resolve(unsigned const char *g, const char *e, struct sllext *ep);

#endif /*SLL_H_*/

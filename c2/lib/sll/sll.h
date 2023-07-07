#ifndef SLL_H_
#define SLL_H_

#include <hash.h>
#include <dbi.h>

typedef List *(sll_get_fnc)(const char *key);

struct sllext
{
  const char *name;
  sll_get_fnc *fnc;
};

extern int sll_raw_output;
extern int sll_trace;

extern Hash *sll_init_t(const char *project, const char *name);
extern void sll_term_t(Hash *h);

extern Dbi_index *sll_init_d(const char *project, const char *name);
extern void sll_term_d(Dbi_index *d);

extern int sll_has_sign_indicator(unsigned const char *g);
extern void sll_init_si(void);

extern void sll_handle(unsigned const char *key);
extern unsigned char *sll_strip_pp(unsigned const char *g);
extern unsigned char *sll_snames_of(unsigned const char *oids);
extern unsigned const char *sll_lookup(unsigned const char *key);
extern unsigned const char *sll_lookup_t(unsigned const char *key);
extern unsigned const char *sll_lookup_d(Dbi_index *dbi, unsigned const char *key);
extern void sll_set_sl(Hash *sl);
extern unsigned char *sll_try_h(const char *oid, unsigned const char *g);
extern unsigned char *sll_v_from_h(const unsigned char *b, const unsigned char *qsub);
extern unsigned char *sll_tmp_key(unsigned const char *key, const char *field);
extern List *sll_resolve(unsigned const char *key, unsigned const char *res);

extern const char *sll_get_name(unsigned char *k);
extern const char *sll_get_oid(unsigned char *k);
extern List *sll_get_list(const char *k);
extern List *sll_get_h(const char *k);

extern struct sllext *sllext (register const char *str, register size_t len);

#endif /*SLL_H_*/

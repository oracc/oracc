#ifndef SLL_H_
#define SLL_H_
#include <hash.h>

extern Hash * sll_init(const char *project, const char *name);
extern void sll_term(Hash *h);

extern int sll_has_sign_indicator(unsigned const char *g);
extern void sll_init_si(void);

extern unsigned char *sll_strip_pp(unsigned const char *g);
extern unsigned char *sll_snames_of(unsigned const char *oids);
extern unsigned const char *sll_lookup(unsigned const char *key);
extern void sll_set_sl(Hash *sl);
extern unsigned const char *sll_try_h(const char *oid, unsigned const char *g);
extern unsigned char *sll_v_from_h(const unsigned char *b, const unsigned char *qsub);

#endif /*SLL_H_*/

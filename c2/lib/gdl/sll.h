#ifndef SLL_H_
#define SLL_H_
#include <hash.h>

extern Hash * sl_init(const char *project, const char *name);
extern void sl_term(Hash *h);

extern int sl_has_sign_indicator(unsigned const char *g);
extern void sl_init_si(void);

extern unsigned char *sl_strip_pp(unsigned const char *g);

#endif /*SLL_H_*/

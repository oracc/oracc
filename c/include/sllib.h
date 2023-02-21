#ifndef _SLLIB_H
#define _SLLIB_H

#include <psd_base.h>

extern unsigned const char * sl_lookup_d(Dbi_index *dbi, unsigned const char *key);

extern Dbi_index * sl_init_d(const char *project, const char *name);
extern void sl_term_d(Dbi_index *dbi);

extern Hash_table * sl_init_h(const char *project, const char *name);
extern void sl_term_h(Hash_table *dbi);

extern int sl_has_sign_indicator(unsigned const char *g);
extern void sl_init_si(void);

extern unsigned char *sl_strip_pp(unsigned const char *g);

#endif /*_SLLIB_H*/

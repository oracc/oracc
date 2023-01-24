#ifndef _SLLIB_H
#define _SLLIB_H

extern unsigned char * sl_lookup(Dbi_index *dbi, char *key);
extern Dbi_index * sl_init(char *project, char *name);
extern void sl_term(Dbi_index *dbi);

#endif /*_SLLIB_H*/

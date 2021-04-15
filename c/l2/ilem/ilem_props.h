#ifndef ILEM_PROPS_
#define ILEM_PROPS_ 1

#include "key.h"

extern void ilem_props_init(void);
extern void ilem_props_term(void);
struct keypair * ilem_props_look(const unsigned char *kv);

#endif/*ILEM_PARA_*/

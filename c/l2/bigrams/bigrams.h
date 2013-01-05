#ifndef BIGRAMS_H_
#define BIGRAMS_H_
#include "../xcl2/xcl.h"
extern struct NL_context *bigrams_init(struct sigset *sp);
extern void bigrams(struct xcl_context *xc);
extern void bigrams_term(void);
#endif/*BIGRAMS_H_*/

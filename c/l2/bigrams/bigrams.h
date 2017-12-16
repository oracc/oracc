#ifndef BIGRAMS_H_
#define BIGRAMS_H_
#include "../xcl2/xcl.h"
extern struct NL_context *bigrams_init(struct sigset *sp);
extern void bigrams(struct xcl_context *xc);
extern void bigrams_term(void);

extern struct NL_context *collos_init(struct sigset *sp);
extern void collos(struct xcl_context *xc);
extern void collos_term(void);
#endif/*BIGRAMS_H_*/

#ifndef _CUNEIFY_H
#define _CUNEIFY_H
#include "xpd2.h"
extern int do_cuneify;
extern const unsigned char *de_allograph(const unsigned char *u,const unsigned char *tilde);
extern void cuneify_init(struct xpd *xp);
extern void cuneify_term(void);
extern const unsigned char *cuneify(const unsigned char *utf);
#endif /*_CUNEIFY_H*/

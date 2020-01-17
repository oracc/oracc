#ifndef ILEM_H_
#define ILEM_H_

#include "ilem_form.h"
#include "xcl.h"

extern int lem_do_wrapup;

#define is_proper(xfp) ((xfp)->f2.pos && (xfp)->f2.pos[1] == 'N' && !(xfp)->f2.pos[2])

typedef int select_func(struct ilem_form *, void *, void *);

extern void ilem_parse(struct xcl_context *xc, struct ilem_form *master_formp, int first_word);
extern void ilem_harvest_notices(struct xcl_context *xcp, struct xcl_l *lp);
extern void ilem_inherit(struct ilem_form*inheritor, struct ilem_form *from);
extern void ilem_lnode(struct xcl_context *,struct xcl_l*);
extern struct ilem_form **ilem_select(struct ilem_form **fpp, 
				     int fpp_len,
				     void *user_data,
				     void * (*init)(void*data), 
				     int (*test)(struct ilem_form *fp, 
						 void *user_data, 
						 void *user_setup),
				     void (*term)(void *data, void *setup),
				     int *nformsp);
extern void ilem_unlemm_warnings(struct xcl_context *xcp, struct xcl_l *lp);
extern void ilem_wrapup(struct xcl_context *xcp, struct xcl_l *lp);
extern void ilem_select_clear(void);

#endif/*ILEM_H_*/

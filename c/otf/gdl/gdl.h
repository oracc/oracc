#ifndef _GDL_H
#define _GDL_H
#include "atf.h"
#include "graphemes.h"
#include "inline.h"
#include "tokenizer.h"
#include "cuneify.h"
#define GDL_FRAG_OK 1
extern int gdl_fragment_ok;
extern int use_unicode;

#define c10e_compound 	c1_c10e_compound
#define gdl_init 	c1_gdl_init
#define gdl_sig		c1_gdl_sig
#define gdl_string	c1_gdl_string
#define gdl_term 	c1_gdl_term

extern void gdl_init(void);
extern void gdl_term(void);
extern struct node *gdl(unsigned char *gdlin, int frag_ok);
extern void gdl_string(unsigned char *gdlinput, int frag_ok);
extern unsigned char *gdl_sig(unsigned char *atf, int frag_ok, int deep);
extern unsigned char *c10e_compound(unsigned const char *g);

#endif /*_GDL_H*/

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
extern void gdl_init(void);
extern void gdl_term(void);
extern struct node *gdl(unsigned char *gdlin, int frag_ok);
extern void gdl_string(unsigned char *gdlinput, int frag_ok);
extern unsigned char *gdl_sig(unsigned char *atf, int frag_ok, int deep);
#endif /*_GDL_H*/

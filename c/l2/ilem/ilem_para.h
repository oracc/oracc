#ifndef ILEM_PARA_
#define ILEM_PARA_ 1

#include "xcl.h"

/* This module manages everything that can be attached
   before or after a lemma in inline lemmatization.  We
   parse these pre- and post-lemma attachments so we 
   can do some basic validation and classification, but
   store them as a list of items which can be handed off
   to other processing phases to do the heavy lifting.
 */

#undef C
#define C(x) LPC_##x,
#define LPC  C(none)C(boundary)C(property)C(pointer)C(syntax)C(linkset)C(nsa)
enum ilem_para_class 
  {
    LPC
    LPC_top
  };

enum ilem_para_pos { ilem_para_pos_ante , ilem_para_pos_post };

#undef C
#define C(x) LPT_##x ,
#define LPT \
  C(discourse)C(sentence)C(clause)C(phrase)		  \
  C(no_discourse)C(no_sentence)C(no_clause)C(no_phrase)	  \
  C(long_prop)C(short_prop)				  \
  C(pointer_anchor)C(pointer_ref)			  \
  C(and)C(no_and)C(prn)C(no_prn)C(or)C(no_or)		  \
  C(premod)C(no_premod)C(mod)C(no_mod)			  \
  C(brack_o)C(brack_c)C(label)				  \
  C(linkset_def)C(linkset_member)			  \
  C(nsa_system)C(nsa_stop)

enum ilem_para_type
  {
    LPT
    LPT_top
  };

struct ilem_para
{
  enum ilem_para_class class;
  enum ilem_para_type type;
  const unsigned char *text;
  const unsigned char *longval;
  int level;
  struct ilem_para *next;
};

extern void ilem_para_boundaries(struct xcl_l*lp, struct xcl_context*xc);
extern struct ilem_para *ilem_para_parse(struct xcl_context *xc, unsigned const char *s, unsigned char **end, 
					 int err_lnum, enum ilem_para_pos, struct xcl_l*lp);
extern void ilem_para_dump(FILE *fp, struct xcl_l *lp);
extern const unsigned char *ilem_para_head_label(struct xcl_c *c, int depth);
extern struct ilem_para *ilem_para_find(struct xcl_l *lp, enum ilem_para_class class, enum ilem_para_type type);

extern void ilem_para_dump_text_ante(FILE *fp, struct xcl_l *lp);
extern void ilem_para_dump_text_post(FILE *fp, struct xcl_l *lp);

#endif/*ILEM_PARA_*/

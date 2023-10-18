#ifndef _INLINE_H
#define _INLINE_H
#include "tokenizer.h"
#include "tree.h"

extern int ods_cols;
extern int ods_mode;

extern struct node **line_words;
extern int nline_words;
extern void reset_forms_insertp(void);
extern void forms_init(void);
extern void forms_term(void);
extern void inline_functions(void (*lemm_save_form_arg)(const char *,const char*,
							const char*,struct lang_context *,
							const char *field),
			     void (*lemm_unform_arg)(void),
			     void (*lemm_reset_form_arg)(const char *,const char *));
extern void inline_init(void);
extern void inline_term(void);
extern void max_cells_attr(struct node *tp);
extern void set_flags(struct node *np,struct flags*fp);
extern void set_uflags(struct node *np,struct uflags*ufp);
extern void tlit_parse_inline(unsigned char *line, unsigned char *end, struct node*lnode, 
			      int word_id_base, int with_word_list, unsigned char *line_id);
extern void tlit_reinit_inline(int with_word_list);
#endif /*_INLINE_H*/

#ifndef _XTLEM_H
#define _LEMLINE_H

struct xcl_ilem
{
  struct xcl_l *x;
  struct ilem_form *i;
};

struct lang_context;
struct xcl_c;
struct node;

extern int need_lemm;
extern void lem_reset_form(const char *ref, const char *form);
extern void lem_save_cont(unsigned char *lp);
extern void lem_save_line(unsigned char *lp);
extern void lem_save_form(const char *ref, const char *lang, const char *formstr,
			  struct lang_context *langcon,
			  const char *field);
extern void lem_save_form_dynalem(const char *ref, const char *lang, const char *formstr,
				  struct lang_context *langcon,
				  const char *field);
extern void lem_save_lemma(struct node *wp, const char *lemma);
extern void lem_text_init(const char *pq);
extern void lem_text_term(void);
extern const char *const* lem_text_meta(void);
extern const char *lem_meta_id(void);
extern void lem_unform(void);
extern void lem_serialize(FILE *fp);
extern void lem_ods_serialize(FILE *fp);
extern void xcl_sentence_labels(struct xcl_context *xc,struct xcl_c *c);
extern void lemline_init(void);
extern void lemline_term(void);
#endif /*_LEMLINE_H*/

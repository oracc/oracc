#ifndef _TRANSLATE_H
#define _TRANSLATE_H
#include "blocktok.h"
#include "xmlnames.h"
enum e_tu_types
{
  etu_trans , etu_note , 
  etu_h1, etu_h2, etu_h3 ,
  etu_label , etu_unit , etu_span,
  etu_dollar , 
  etu_interlinear , etu_parallel , etu_unitary , etu_labeled ,
  etu_top
};

struct translation
{
  const char    * type;
  const unsigned char* id;
  enum e_tu_types etype;
  const char    * lang;
  const char    * code;
  struct node   * tree;
};

struct trans_token
{
    const char*      name;
    enum  e_tu_types type;
    enum  e_type     elem;
};

extern struct translation **translations;
extern const char *project; 
extern struct node *curr_trans_tree(void);
extern int  has_trans(void);
extern void init_trans(void);
extern struct trans_token *transtok (register const char *str,register unsigned int len);
extern struct node *translation(unsigned char **lines,struct node*text,enum e_tu_types *transtype);
extern unsigned char **trans_block(unsigned char **lines,unsigned char *token,struct block_token*blocktokp, char save);
extern void trans_clear(void);
extern void trans_cols_attr(struct node *tp);
extern unsigned char ** trans_inter(unsigned char **lines);
extern void trans_mapdump(struct node *p);
extern unsigned char **trans_para(unsigned char **lines, unsigned char *s, 
				  struct node *p, int p_elem, int with_id,
				  unsigned const char *label,
				  int stop_at_lnum);
extern unsigned char **trans_dollar(unsigned char **lines);
extern void trans_finish_labels(struct node *text, struct translation *tp);
extern unsigned char *trans_inline(struct node*parent,unsigned char *text, const char *, int);

extern void trans_hash_init(void);
extern int trans_hash_add(const char *t);
extern void trans_hash_term(void);

#endif /*_TRANSLATE_H*/

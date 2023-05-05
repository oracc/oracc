#ifndef _NONX_H
#define _NONX_H
enum x_class { x_qual, x_extent, x_scope, x_state, x_rule , x_docket ,
	       x_impression , x_image_class , x_empty_class , x_ignore };
enum x_type { x_number, x_all, x_blank, x_broken, x_column, x_effaced, x_end, 
	      x_line, x_middle, x_object, x_rest, x_start, x_surface, x_dock ,
	      x_traces, x_illegible, x_single, x_double, x_triple, x_impress,
	      x_about, x_atleast, x_atmost , x_of, x_at , x_space, x_image,
	      x_empty  };

struct nonx_token
{
  const char *name;
  enum x_class class;
  enum x_type  type;
  const char *const str;
  int of_flag;
  int pl_flag;
};

struct nonx_link
{
  const char *pre;
  const char *url;
  const char *text;
  const char *post;
};

struct nonx
{
  int strict;
  unsigned char *number;
  unsigned char *ref;
  unsigned char *literal;
  unsigned char flags[4];
  struct nonx_token* qual;
  struct nonx_token* extent;
  struct nonx_token* scope;
  struct nonx_token* state;
  struct nonx_link * link;
};

extern struct nonx*parse_nonx(unsigned char *l);
struct nonx_token *nonxtok(register const char *str,register unsigned int len);

extern int dollar_fifo;
extern void dollar_init(void);
extern const char *dollar_add(const char *xid);
extern const char *dollar_get(void);
extern int dollar_clear(void);
extern void dollar_reset(void);
extern int dollar_term(void);
extern const char *dollar_peek(void);
#endif /*_NONX_H*/

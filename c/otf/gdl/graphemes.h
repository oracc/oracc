#ifndef _GRAPHEMES_H
#define _GRAPHEMES_H

#include "tokenizer.h"

#define C(x) x,
#define OPS C(beside)C(joining)C(repeated)C(containing)\
  C(above)C(crossing)C(opposing)C(reordered)

enum t_ops { OPS o_top };
#undef C

#define MAX_MOD_DATA 6

struct mods
{
  enum t_type type; 
  char data[MAX_MOD_DATA+1];
};

struct grapheme
{
  enum t_type type;
  unsigned char *atf;
  union
  {
    struct simple { unsigned const char *base; } s;
    struct number { unsigned char *r; struct grapheme *n; } n;
    struct punct  { struct grapheme *g; } p;
    struct qualed { struct grapheme *g; struct grapheme *q; } q;
  } g;
  struct mods *mods;
  struct node *xml;
  unsigned char *xtf;
  unsigned char *raw; /* in legacy mode this holds graphemes with medial square 
			 brackets */
  int gflags;
};

#define GFLAGS_BAD         0x001
#define GFLAGS_HAVE_SQUARE 0x002
#define GFLAGS_REDUNDANT   0x004
#define GFLAGS_HETH	   0x008
#define GFLAGS_DOTS	   0x010

extern struct node *pending_disamb;

extern void g_reinit(void);
extern struct grapheme *gparse(register unsigned char *g, enum t_type type);
extern void graphemes_init(void);
extern void graphemes_term(void);
extern void galloc_init(void);
extern void galloc_term(void);
extern enum t_type gtype(register unsigned char *g);
extern unsigned char *unicodify(register const unsigned char *s);
extern char cued_opener[];
extern void cue_opener(const char *o);
extern void appendCloser(struct node *gp, const char *c);
extern int is_signlist(register const unsigned char *s);
#define render_g(p,i,s) _render_g((p),(i),(s),__FILE__,__LINE__)
extern unsigned char *_render_g(struct node *np, unsigned char *insertp, unsigned char *startp,
				const char *FILE, int LINE);
extern unsigned char *render_g_text(struct node*tp, unsigned char *insertp, unsigned char *startp);
extern void grapheme_id_reset(void);
#endif

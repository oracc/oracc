#ifndef GSORT_H_
#define GSORT_H_

struct GS_item
{
  unsigned const char *g;	/* item grapheme */
  unsigned const char *b;	/* grapheme base */
  unsigned const char *k;	/* grapheme key via lib/collate */
  short x;			/* grapheme index */
  short r;			/* repeater for number graphemes, e.g., 3(diš) */
};

struct GS_head
{
  unsigned const char *s; 	/* original string */
  int n;		  	/* number of items */
  struct GS_item **i;		/* array of item ptrs */
};

typedef struct GS_head GS_head;
typedef struct GS_item GS_item;

extern void gsort_init(void);
extern void gsort_term(void);
extern GS_head *gsort_prep(Tree *tp);
extern void gsort_show(GS_head *gsp);
extern int gsort_cmp(const void *v1, const void *v2);


#endif/*GSORT_H_*/

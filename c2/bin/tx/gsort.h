#ifndef GSORT_H_
#define GSORT_H_

struct gsort_item
{
  unsigned const char *g;
  unsigned const char *b;
  unsigned const char *k;
  short x;
  short r;
};

struct gsort_header
{
  int n;
  struct gsort_item **g;
};

typedef struct gsort_header Gsort;
typedef struct gsort_item Gitem;

extern void gsort_init(void);
extern void gsort_show(Tree*tp);

#endif/*GSORT_H_*/

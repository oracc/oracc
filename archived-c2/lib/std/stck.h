#ifndef STCK_H_
#define STCK_H_

#include <stdint.h>

struct stck
{
  uintptr_t *stack;
  int alloced;
  int nalloc;
  int top;
};

typedef struct stck Stck;

extern uintptr_t stck_pop(Stck *s);
extern void stck_push(Stck *s, uintptr_t tok);
extern Stck *stck_init(int nalloc);
extern void stck_term(Stck *s);

#endif/*STCK_H*/

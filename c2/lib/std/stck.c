#include <stdlib.h>
#include "stck.h"

static void
stck_extend(Stck *s)
{
  int t;
  s->alloced *= 2;
  s->stack = realloc(s->stack, s->alloced * sizeof(uintptr_t));
  for (t = s->top; t < s->alloced; ++t)
    s->stack[t] = 0;
}

uintptr_t
stck_pop(Stck *s)
{
  if (s->top >= 0)
    return s->stack[s->top--];
  else
    return -1;
}

void
stck_push(Stck *s, uintptr_t tok)
{
  /* save curr_stream to stack */
  if (++s->top == s->alloced)
    stck_extend(s);
  s->stack[s->top] = tok;
}

Stck *
stck_init(int nalloc)
{
  Stck *s = calloc(1, sizeof(Stck));
  s->alloced = s->nalloc = nalloc;
  s->stack = calloc(s->nalloc, sizeof(uintptr_t));
  s->top = -1;
  return s;
}

void
stck_term(Stck *s)
{
  if (s->alloced)
    free(s->stack);
  free(s);
}


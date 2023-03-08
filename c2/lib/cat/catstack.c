#include <stdlib.h>
#include <stdio.h>
#include "cat.h"

#define CS_MAX 	16
static struct catstate cstack[CS_MAX];
static int cs_depth = 0;

void
catstack_reset(void)
{
  cs_depth = 0;
}

struct catstate *
catstack_push(struct catnode *n, struct catinfo *i)
{
  struct catstate *csp = NULL;
  if (cs_depth < CS_MAX)
    {
      csp = &cstack[cs_depth++];
      csp->cn = n;
      csp->cip = i;
      csp->end = i->end; /* csp->end is an editable version of 'end';
			    the one in csp->cip belongs to the
			    reference structure for the name */
    }
  else
    {
      fprintf(stderr, "catstack_push: nesting too deep\n");
    }
  return csp;
}

/* At depth 1 we are using stack entry 0 etc. */
struct catstate *
catstack_pop(void)
{
  struct catstate *csp = NULL;
  if (cs_depth)
    {
      --cs_depth;
      if (cs_depth)
	csp = &cstack[cs_depth - 1];
    }
  return csp;
}

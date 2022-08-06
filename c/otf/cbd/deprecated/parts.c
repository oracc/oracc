#include <ctype128.h>
#include "gx.h"

struct parts *curr_parts;

struct parts *
parts_init(YYLTYPE l, struct entry *e)
{
  struct parts *pp = mb_new(e->owner->partsmem);
  pp->l = l;
  return (e->parts = pp);
}

void
parse_parts(struct entry *e, unsigned char *s, locator *lp)
{
}

#include <ctype128.h>
#include "gx.h"

struct parts *curr_parts;

struct parts *
parts_init(struct entry *e)
{
  return  mb_new(e->owner->partsmem);
}

void
parse_parts(struct entry *e, unsigned char *s, locator *lp)
{
}

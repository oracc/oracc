#include <ctype128.h>
#include "gx.h"

struct pleiades *
pleiades_init(struct entry *e, unsigned char *coord, unsigned char *id, unsigned char *uid)
{
  struct pleiades *p = mb_new(e->owner->pleiadesmem);
  p->coord = coord;
  p->id = id;
  p->uid = uid;
  return p;
}

void
parse_pl_coord(struct entry *e, unsigned char *s, locator *lp)
{
}

void
parse_pl_id(struct entry *e, unsigned char *s, locator *lp)
{
}

void
parse_pl_uid(struct entry *e, unsigned char *s, locator *lp)
{
}

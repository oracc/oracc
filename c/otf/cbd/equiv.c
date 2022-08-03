#include <ctype128.h>
#include "gx.h"

struct equiv *
equiv_init(struct entry *e, unsigned char *lang, unsigned char *text)
{
  struct equiv *eq = mb_new(e->owner->equivmem);
  eq->lang = lang;
  eq->text = text;
  return eq;
}

void
parse_equiv(struct entry *e, unsigned char *s, locator *lp)
{
}

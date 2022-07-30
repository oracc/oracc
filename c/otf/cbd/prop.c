#include <ctype128.h>
#include "gx.h"

void
proplist_add(struct cbd *c, char *text)
{
  if (!c->proplists)
    c->proplists = list_create(LIST_SINGLE);
  list_add(c->proplists, text);
}

void
parse_prop(struct entry *e, unsigned char *s, locator *lp)
{
}

void
parse_proplist(struct entry *e, unsigned char *s, locator *lp)
{
}

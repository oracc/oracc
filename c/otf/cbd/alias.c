#include <ctype128.h>
#include "gx.h"

void
parse_alias(struct entry *e, unsigned char *s)
{
  struct alias *a = malloc(sizeof(struct alias));
  a->c = malloc(sizeof(struct cgp));
  cgp_parse(a->c, s);
  printf("alias => %s [ %s ] %s\n", a->c->cf, a->c->gw, a->c->pos);
  list_add(e->aliases, a);
  hash_add(e->owner->haliases, s, e);
}

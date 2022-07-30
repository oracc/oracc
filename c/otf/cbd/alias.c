#include <ctype128.h>
#include "gx.h"

struct alias *curr_alias;

struct alias *
alias_init(struct entry *e)
{
  struct alias *a = mb_new(e->owner->aliasmem);
  list_add(e->aliases, a);
  a->cgp = cgp_get_one();
  hash_add(e->owner->haliases, a->cgp->closed, e);
  return a;
}

void
parse_alias(struct entry *e, unsigned char *s, locator *lp)
{
  struct alias *a = malloc(sizeof(struct alias));
  a->l = e->l;
  cgp_parse(a->cgp, s, &a->l);
  /*printf("alias => %s [ %s ] %s\n", a->c->cf, a->c->gw, a->c->pos);*/
  list_add(e->aliases, a);
  hash_add(e->owner->haliases, s, e);
}

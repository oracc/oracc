#include <ctype128.h>
#include "gx.h"

struct alias *curr_alias;

struct alias *
alias_init(YYLTYPE l, struct entry *e)
{
  struct alias *a = mb_new(e->owner->aliasmem);
  a->l = l;
  list_add(e->aliases, a);
  a->cgp = cgp_get_one();
  hash_add(e->owner->haliases, a->cgp->tight, e);
  return a;
}

void
dcf_init(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg)
{
  struct tag *tp;
  if (!e->dcfs)
    e->dcfs = list_create(LIST_SINGLE);
  if (!e->hdcfs)
    e->hdcfs = hash_create(1024);
  tp = mb_new(e->owner->tagmem);
  tp->l = l;
  tp->name = (ccp)dcf;
  tp->val = dcfarg;
  list_add(e->dcfs, dcf);
  hash_add(e->hdcfs, dcf, dcfarg);
}

#if 0

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

#endif

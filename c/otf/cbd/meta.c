#include "gx.h"
#include "grammar.tab.h"

struct meta *
meta_init(struct entry *e)
{
  return mb_new(e->owner->metamem);
}

void
meta_add(struct entry *e, struct meta *mp, int tok, void *val)
{
  List *lp = NULL;
  struct metaorder *orderp;
  switch (tok)
    {
    case BIB:
      if (!mp->bib)
	mp->bib = list_create(LIST_SINGLE);
      lp = mp->bib;
      break;
    case COLLO:
      if (!mp->collo)
	mp->collo = list_create(LIST_SINGLE);
      lp = mp->collo;
      break;
    case EQUIV:
      if (!mp->equiv)
	mp->equiv = list_create(LIST_SINGLE);
      lp = mp->equiv;
      break;
    case INOTE:
      if (!mp->inote)
	mp->inote = list_create(LIST_SINGLE);
      lp = mp->inote;
      break;
    case ISSLP:
      if (!mp->isslp)
	mp->isslp = list_create(LIST_SINGLE);
      lp = mp->isslp;
      break;
    case NOTE:
      if (!mp->note)
	mp->note = list_create(LIST_SINGLE);
      lp = mp->note;
      break;
    case OID:
      if (!mp->oid)
	mp->oid = list_create(LIST_SINGLE);
      lp = mp->oid;
      break;
    case PROP:
      if (!mp->prop)
	mp->prop = list_create(LIST_SINGLE);
      lp = mp->prop;
      break;
    case PLEIADES:
      if (!mp->pleiades)
	mp->pleiades = list_create(LIST_SINGLE);
      lp = mp->pleiades;
      break;
#if 0
    case REL:
      lp = mp->rel;
      break;
#endif
    default:
      fprintf(stderr, "meta_add: unhandled token %d\n", tok);
      break;
    }
  if (lp)
    {
      list_add(lp, val);
      orderp = mb_new(e->owner->metaordermem);
      orderp->tok = tok;
      orderp->val = val;
      if (!mp->order)
	mp->order = list_create(LIST_SINGLE);
      list_add(mp->order, orderp);
    }
}

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "f2.h"
#include "gx.h"

extern int bang, star;
static int one = 1;
static List *curr_base_list = NULL;
struct parts *curr_parts;
List *cmt_queue = NULL;

#define cmts(l) (l) = cmt_queue , cmt_queue = NULL;

struct alias *
bld_alias(YYLTYPE l, struct entry *e)
{
  struct alias *a = mb_new(e->owner->aliasmem);
  a->l = l;
  cmts(a->l.cmt);
  list_add(e->aliases, a);
  a->cgp = cgp_get_one();
  hash_add(e->owner->haliases, a->cgp->tight, e);
  return a;
}

void
bld_allow(YYLTYPE l, struct entry *e, unsigned char *lhs, unsigned char *rhs)
{
  struct allow *ap = NULL;
  Hash_table *h_a, *h_b;

  if (!e->b_allow)
    e->b_allow = hash_create(1024);
  if (!e->allows)
    e->allows = list_create(LIST_SINGLE);

  ap = mb_new(e->owner->allowmem);
  ap->lhs = lhs;
  ap->rhs = rhs;
  ap->l = l;
  cmts(ap->l.cmt);
  list_add(e->allows, ap);

  h_a = hash_find(e->b_allow, lhs);
  h_b = hash_find(e->b_allow, rhs);
  if (!h_a)
    {
      h_a = hash_create(1);
      hash_add(h_a, rhs, ap);
      hash_add(e->b_allow, lhs, h_a);
    }
  if (!h_b)
    {
      h_b = hash_create(1);
      hash_add(h_b, lhs, ap);
      hash_add(e->b_allow, rhs, h_b);
    }
}

/* At parse time we just save the bases in a list of lists */
void
bld_bases_pri(YYLTYPE l, struct entry *e, unsigned char *lang, unsigned char *p)
{
  if (p && strlen((ccp)p))
    {
      struct loctok *ltp = bld_loctok(&l,e,p);
      ltp->lang = lang;
      if (!e->bases)
	e->bases = list_create(LIST_SINGLE);
      list_add(e->bases, (curr_base_list = list_create(LIST_SINGLE)));
      list_add(curr_base_list, ltp);
    }
  else
    {
      msglist_err(&l, "zero-length primary base (misplaced ';'?)");
    }
}

void
bld_bases_alt(YYLTYPE l, struct entry *e, unsigned char *a)
{
  if (a && strlen((ccp)a))
    {
      if (curr_base_list)
	list_add(curr_base_list, bld_loctok(&l,e,a));
    }
  else
    {
      msglist_err(&l, "zero-length alternate base (misplaced ','?)");
    }
}

struct cbd *
bld_cbd(void)
{
  struct cbd *c = NULL;
  c = malloc(sizeof(struct cbd));
  c->aliasmem = mb_init(sizeof(struct alias), 1024);
  c->allowmem = mb_init(sizeof(struct allow), 1024);
  c->cgpmem = mb_init(sizeof(struct cgp), 1024);
  c->editmem = mb_init(sizeof(struct edit), 1024);
  c->equivmem = mb_init(sizeof(struct equiv), 1024);
  c->entrymem = mb_init(sizeof(struct entry), 1024);
  c->formsmem = mb_init(sizeof(struct f2), 1024);
  c->i18nmem = mb_init(sizeof(struct i18n), 1024);
  c->metamem = mb_init(sizeof(struct meta), 1024);
  c->loctokmem = mb_init(sizeof(struct loctok), 1024);
  c->metaordermem = mb_init(sizeof(struct metaorder), 1024);
  c->partsmem = mb_init(sizeof(struct parts), 1024);
  c->pleiadesmem = mb_init(sizeof(struct pleiades), 1024);
  c->sensesmem = mb_init(sizeof(struct sense), 1024);
  c->tagmem = mb_init(sizeof(struct tag), 1024);
  c->taglmem = mb_init(sizeof(struct tagl), 1024);
  c->pool = npool_init();
  c->letters = list_create(LIST_SINGLE);
  c->entries = list_create(LIST_SINGLE);
  c->edits = list_create(LIST_SINGLE);
  c->proplists = list_create(LIST_SINGLE);
  c->haliases = hash_create(16);
  c->hentries = hash_create(1024);
  c->l.file = (char *)file;
  c->l.first_line = 1;
  return c;
}

void
bld_cbd_setup(struct cbd*c)
{
  c->iname = malloc(strlen((ccp)c->project) + strlen((ccp)c->lang) + 2);
  sprintf((char*)c->iname, "%s:%s", c->project, c->lang);
  hash_add(cbds, c->iname, c);
  /* xpdinit */
  /* cuneify_init(c->xpd); */
}

void
bld_cbd_term(struct cbd*c)
{
  npool_term(c->pool);
  hash_add(cbds, c->iname, NULL);
  cuneify_term();
  free(c);
}

List *
bld_cmt_append(List *to, List *from)
{
  if (!to)
    to = from;
  else if (to == from)
    return to;
  else
    {
      List_node *lp;
      for (lp = from->first; lp; lp = lp->next)
	list_add(to, lp->data);
      list_free(from, NULL);
    }
  return to;
}

void
bld_cmt_queue(unsigned char *cmt)
{
  if (curr_entry)
    {
      if (!cmt_queue)
	cmt_queue = list_create(LIST_SINGLE);
      list_add(cmt_queue, cmt);
    }
  else
    msglist_err(&yylloc, "#comments are only allowed inside entries");
}

void
bld_dcf(YYLTYPE l, struct entry *e, unsigned char *dcf, unsigned char *dcfarg)
{
  struct tag *tp;
  if (!e->dcfs)
    e->dcfs = list_create(LIST_SINGLE);
  if (!e->hdcfs)
    e->hdcfs = hash_create(1024);
  tp = mb_new(e->owner->tagmem);
  tp->l = l;
  cmts(tp->l.cmt);
  tp->name = (ccp)dcf;
  tp->val = dcfarg;
  list_add(e->dcfs, dcf);
  hash_add(e->hdcfs, dcf, dcfarg);
}

void
bld_discl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text, int e_or_s)
{
  if (e_or_s) /* apply to sense */
    {
      struct sense *sp = list_last(e->senses);
      struct sense *senselp = NULL;
      List_node *tmp;
      for (tmp = sp->sensels->first; tmp; tmp = tmp->next)
	{
	  if (!strcmp(lang, (const char*)((struct sense *)(tmp->data))->lng))
	    {
	      senselp = tmp->data;
	      break;
	    }
	}
      if (senselp)
	senselp->disc = bld_tag(l, e, "discl", text);
      else
	fprintf(stderr, "lang %s not found in sensels\n", lang);
    }
  else
    {
      struct i18n *i18 = mb_new(e->owner->i18nmem);
      i18->l = l;
      cmts(i18->l.cmt);
      i18->lang = lang;
      i18->data = text;
      if (!e->discls)
	e->discls = list_create(LIST_SINGLE);
      list_add(e->discls, i18);
    }
}

void
bld_edit(struct entry *e, char ctxt, char type)
{
  struct edit *ed = mb_new(e->owner->editmem);
  /*struct sense *snode = NULL;*/
  if (ctxt == 's')
    {
      struct sense *sp = list_last(e->senses);
      ed->owner = sp;
      sp->ed = ed;
      ed->lp = &sp->l;
    }
  else
    {
      ed->owner = e;
      e->ed = ed;
      ed->lp = &e->l;
    }
  switch (type)
    {
    case '+':
      ed->type = (ctxt == 'e' ? ADD_E : ADD_S);
      break;
    case '-':
      ed->type = (ctxt == 'e' ? DEL_E : DEL_S);
      break;
    case '>':
      ed->type = (ctxt == 'e' ? REN_E : REN_S);
      ed->target = cgp_get_one();
      break;
    case '|':
      ed->type = (ctxt == 'e' ? MRG_E : MRG_S);
      ed->target = cgp_get_one();
      break;
    default:
      break;
    }
}

void
bld_edit_entry(struct entry *e, char type)
{
  bld_edit(e, 'e', type);
}

struct sense *
bld_edit_sense(struct entry *e, char type)
{
  bld_edit(e, 's', type);
  return ((struct sense *)(list_last(e->senses)))->ed->sp = mb_new(e->owner->sensesmem);
}

void
bld_edit_why(struct entry *e, char *why)
{
  if (e->ed)
    e->ed->why = (ucp)why;
  /* should error if #why: doesn't follow an edit and valid it's only after -@entry */
}
  
struct entry *
bld_entry(YYLTYPE l, struct cbd* c)
{
  struct entry *e = mb_new(c->entrymem);
  e->aliases = list_create(LIST_SINGLE);
  e->forms = list_create(LIST_SINGLE);
  e->senses = list_create(LIST_SINGLE);
  e->owner = c;
  e->lang = c->lang;
  list_add(c->entries, e);
  e->meta = mb_new(c->metamem);
  e->l = l;
  cmts(e->l.cmt);
  if (bang)
    {
      bang = 0;
      e->bang = 1;
    }
  if (star)
    {
      star = 0;
      e->usage = 1;
    }
  return e;
}

void
bld_entry_cgp(struct entry *e)
{
  e->cgp = cgp_get_one();
  hash_add(e->owner->hentries, e->cgp->tight, &one);
}

#if 0
static void
bld_entry_term(struct entry *e)
{
  if (e->b_pri)
    {
      hash_free(e->b_pri, NULL);
      hash_free(e->b_alt, NULL);
      hash_free(e->b_sig, NULL);
      e->b_pri = e->b_alt = e->b_sig = e->b_allow = NULL;
    }
  if (e->b_allow)
    {
      hash_free(e->b_allow, NULL);
      e->b_allow = NULL;
    }
}
#endif

struct equiv *
bld_equiv(struct entry *e, unsigned char *lang, unsigned char *text)
{
  struct equiv *eq = mb_new(e->owner->equivmem);
  eq->lang = lang;
  eq->text = text;
  return eq;
}

struct f2 *
bld_form(YYLTYPE l, struct entry *e)
{
  static struct f2 *f2p;
  f2p = mb_new(e->owner->formsmem);
  f2p->file = (ucp)l.file;
  f2p->lnum = l.first_line;
  list_add(e->forms, f2p);
  if (bang)
    {
      bang = 0;
      f2p->rank = 1; /* check this is the right value for form! rank */
    }
  return f2p;
}

void
bld_form_setup(struct entry *e, struct f2* f2p)
{
  f2p->project = e->owner->project;
  if (!f2p->lang)
    f2p->lang = e->lang;
  f2p->core = langcore_of((ccp)f2p->lang);
  f2p->cf = e->cgp->cf;
  f2p->gw = e->cgp->gw;
  f2p->pos = e->cgp->pos;
}

void
bld_gwl(YYLTYPE l, struct entry *e, const char *lang, unsigned char *text)
{
  struct i18n *i18 = mb_new(e->owner->i18nmem);
  i18->l = l;
  cmts(i18->l.cmt);
  i18->lang = lang;
  i18->data = text;
  if (!e->gwls)
    e->gwls = list_create(LIST_SINGLE);
  list_add(e->gwls, i18);
}

struct loctok *
bld_loctok(YYLTYPE *lp, struct entry *e, unsigned char *tok)
{
  struct loctok *ltp = mb_new(e->owner->loctokmem);
  ltp->l = *lp;
  cmts(ltp->l.cmt);
  ltp->tok = tok;
  return ltp;
}

struct meta *
bld_meta_create(struct entry *e)
{
  return mb_new(e->owner->metamem);
}

void
bld_meta_add(YYLTYPE l, struct entry *e, struct meta *mp, int tok, const char *name, void *val)
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
      orderp->l = l;
      cmts(orderp->l.cmt);
      orderp->name = name;
      orderp->tok = tok;
      orderp->val = val;
      if (!mp->order)
	mp->order = list_create(LIST_SINGLE);
      list_add(mp->order, orderp);
    }
}

void
bld_note(YYLTYPE l, struct entry *e, struct meta *curr_meta, unsigned char *text)
{
  struct tagl *tlp = mb_new(e->owner->taglmem);
  tlp->l = l;
  cmts(tlp->l.cmt);
  tlp->data = text;
  if (!curr_meta->note)
    curr_meta->note = list_create(LIST_SINGLE);
  list_add(curr_meta->note, tlp);
}

void
bld_notel(YYLTYPE l, struct entry *e, struct meta *curr_meta, const char *lang, unsigned char *text)
{
  struct i18n *i18p = mb_new(e->owner->i18nmem);
  struct tagl *tp = list_last(curr_meta->note);
  i18p->l = l;
  cmts(i18p->l.cmt);
  i18p->data = text;
  list_add(tp->i18n, i18p);
}

struct parts *
bld_parts(YYLTYPE l, struct entry *e)
{
  struct parts *pp = mb_new(e->owner->partsmem);
  if (!e->parts)
    e->parts = list_create(LIST_SINGLE);
  list_add(e->parts, pp);
  pp->l = l;
  cmts(pp->l.cmt);
  return pp;
}

struct pleiades *
bld_pl_id(YYLTYPE l, struct entry *e, unsigned char *id)
{
  struct pleiades *p = mb_new(e->owner->pleiadesmem);
  p->l_id = l;
  cmts(p->l_id.cmt);
  p->id = id;
  return p;
}

void
bld_pl_coord(YYLTYPE l, struct pleiades *p, unsigned char *coord)
{
  p->l_coord = l;
  cmts(p->l_coord.cmt);
  p->coord = coord;
}

void
bld_pl_alias(YYLTYPE l, struct pleiades *p, const char *lang, unsigned char *alias)
{
  struct loctok *ltp = mb_new(curr_cbd->loctokmem);
  if (!p->pl_aliases)
    p->pl_aliases = list_create(LIST_SINGLE);
  list_add(p->pl_aliases, ltp);
  ltp->l = l;
  cmts(ltp->l.cmt);
  ltp->lang = (ucp)lang;
  ltp->tok = alias;
}

void
bld_proplist(struct cbd *c, char *text)
{
  if (!c->proplists)
    c->proplists = list_create(LIST_SINGLE);
  list_add(c->proplists, text);
}

void
bld_relations(struct cbd *c, char *text)
{
  if (!c->relations)
    c->relations = list_create(LIST_SINGLE);
  list_add(c->relations, text);
}

struct sense *
bld_sense(YYLTYPE l, struct entry *e)
{
  struct sense *sp = mb_new(e->owner->sensesmem);
  sp->l = l;
  cmts(sp->l.cmt);
  list_add(e->senses, sp);
  if (bang)
    {
      bang = 0;
      sp->bang = 1;
    }
  return sp;
}

struct sense *
bld_sensel(YYLTYPE l, struct entry *e)
{
  struct sense *sp = mb_new(e->owner->sensesmem);
  struct sense *curr_sense = list_last(e->senses);
  sp->l = l;
  cmts(sp->l.cmt);
  list_add(curr_sense->sensels, sp);
  return sp;
}

void
bld_stem(YYLTYPE l, struct entry *e, unsigned char *stem)
{
  if (!e->stems)
    e->stems = list_create(LIST_SINGLE);
  
  list_add(e->stems, stem);
}

struct tag *
bld_tag(YYLTYPE l, struct entry *e, const char *name, unsigned char *val)
{
  struct tag *tp;
  tp = mb_new(e->owner->tagmem);
  tp->l = l;
  cmts(tp->l.cmt);
  tp->name = name;
  tp->val = val;
  return tp;
}


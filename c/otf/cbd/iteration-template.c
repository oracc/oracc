#include <stdio.h>
#include "gx.h"
#include "grammar.tab.h"

#include "iterator_fncs.c"

extern void iterator(struct cbd *c, iterator_fnc fncs[]);

static void f_proplist(const char *p);

void
ITERATOR(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
f_alias(struct alias *a)
{
  (a->cgp->tight);
}

static void
f_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->b_allow, lhs);
      (lhs, rhs);
    }
}

static void
f_bases(struct entry *e)
{
  List_node *outer;
  int i;

  for (i = 0, outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      if (i++)
	/* ; */
      else
	/* */
      ((const char *)inner->data);
      if (list_len(bp) > 1)
	{
	  int j;
	  /* ( */;
	  for (j = 0, inner = inner->next; inner; inner = inner->next)
	    {
	      if (j++)
		/* , */;
	      ((const char *)inner->data);
	    }
	  /* ) */;
	}
    }
}

static void
f_cbd(struct cbd *c)
{
  c->project, c->lang, c->name;
  if (list_len(c->proplists))
    list_exec(c->proplists, (list_exec_func*)f_proplist);
}

static void
f_dcfs(struct entry *e)
{
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->hdcfs, lhs);
      (lhs, rhs);
    }
}

static void
f_entry(struct entry *e)
{
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case ADD_E:
	  /* + */;
	  break;
	case DEL_E:
	  /* - */;
	  break;
	default:
	  break;
	}
    }
  ( /* @entry */ e->cgp->loose);
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case DEL_E:
	  (/* #why */ e->ed->why);
	  break;
	case REN_E:
	  (/* > */ e->ed->target->tight);
	  break;
	case MRG_E:
	  (/* >> */ e->ed->target->tight);
	  break;
	default:
	  break;
	}
    }
  if (e->disc)
    (/* @disc */ e->disc);
}

static void
f_end_cbd(struct cbd *c)
{
  ;
}

static void
f_end_entry(struct entry *e)
{
  /* @end entry */
}

static void
f_forms(struct entry *e)
{
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  struct f2 *f2p = (struct f2*)(lp->data);
	  (/* @form */ f2p->form);
	  if (f2p->lang) /* careful: we only should only emit this if lang is explicit in form */
	    (/* %% */ f2p->lang);
	  if (f2p->base)
	    (/* /%s */ f2p->base);
	  if (f2p->stem)
	    (/* * */ f2p->stem);
	  if (f2p->cont)
	    (/* + */ f2p->cont);
	  if (f2p->morph)
	    (/* # */ f2p->morph);
	  if (f2p->morph2)
	    (/* ## */ f2p->morph2);
	  if (f2p->norm)
	    (/* $ */ f2p->norm);
	}
    }
}

static void
f_meta(struct entry *e)
{
  if (e->meta && e->meta->order)
    {
      List_node *lp;
      for (lp = e->meta->order->first; lp; lp = lp->next)
	{
	  struct metaorder *mo = ((struct metaorder*)(lp->data));
	  const char *at = NULL;
	  
	  switch (mo->tok)
	    {
	    case BIB:
	      at = "bib";
	      break;
	    case COLLO:
	      at = "collo";
	      break;
	    case INOTE:
	      at = "inote";
	      break;
	    case ISSLP:
	      at = "isslp";
	      break;
	    case NOTE:
	      at = "note";
	      break;
	    case OID:
	      at = "oid";
	      break;
	    case PROP:
	      at = "prop";	      
	      break;
	    case EQUIV:
	      {
		struct equiv *val = (struct equiv *)(mo->val);
		(/* @equiv % */ val->lang, val->text);
	      }
	      break;
	    case PLEIADES:
	      {
		struct pleiades *val = (struct pleiades*)(mo->val);
		(/* @pl_coord @pl_id @pl_uid */ val->coord, val->id, val->uid);
	      }
	      break;
#if 0
	    case REL:
	      lp = mp->rel;
	      break;
#endif
	    default:
	      fprintf(stderr, "unhandled tok %d\n", mo->tok);
	      break;
	    }
	  if (at)
	    (/* at */ (const char*)mo->val);
	}
    }  
}

static void
f_parts(struct entry *e)
{
  if (e->parts->cgps && list_len(e->parts->cgps))
    {
      /* @parts */;
      List_node *lp;
      for (lp = e->parts->cgps->first; lp; lp = lp->next)
	(((struct cgp*)(lp->data))->tight);
    }
}

static void
f_phon(struct entry *e)
{
  (/* @phon */ (ccp)e->phon);
}

static void
f_proplist(const char *p)
{
  (/* @proplist */ p);
}

static void
f_root(struct entry *e)
{
  (/* @root */ (ccp)e->root);
}

static void
f_senses(struct entry *e)
{
  if (e->beginsenses)
    (/* @senses */ );

  List_node *lp;
  for (lp = e->senses->first; lp; lp = lp->next)
    {
      struct sense *sp = (struct sense*)(lp->data);

      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case ADD_S:
	      putchar('+');
	      break;
	    case DEL_S:
	      putchar('-');
	      break;
	    default:
	      break;
	    }
	}
      
      (/* @sense */ );
      if (sp->lng)
	{ (/* % */ sp->lng); }
      if (sp->sid)
	(/* # */ sp->sid);
      if (sp->num)
	(/* . */ sp->num);
      if (sp->sgw)
	(/* [%s]", sp->sgw);
      if (sp->pos)
	(/*  */ sp->pos);
      if (sp->mng)
	(/*  */ sp->mng);

      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case REN_S:
	      (/* > %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    case MRG_S:
	      (/* >> %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    default:
	      break;
	    }
	}
      if (sp->disc)
	(/* @disc */ sp->disc);
    }
  
  if (e->beginsenses)
    (/* @end senses */ );
}

static void
f_stems(struct entry *e)
{
  List_node *lp;
  (/* @stems */ );
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = ((ucp)(lp->data));
      (stem);
    }
}

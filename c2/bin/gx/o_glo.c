#include <stdio.h>
#include <cbd.h>
#include "gx.h"

#include "o_glo_fncs.c"

extern void iterator(struct cbd *c, iterator_fnc fncs[]);

static void o_glo_cmt(List *c);
static void o_glo_props(const char *p);

void
o_glo(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
o_glo_aliases(struct entry *e)
{
  List_node *lp;
  for (lp = e->aliases->first; lp; lp = lp->next)
    {
      struct alias *ap = lp->data;
      o_glo_cmt(ap->l.user);
      printf("@alias %s\n", ap->cgp->tight);
    }
}

static void
o_glo_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      struct allow *ap = lp->data;
      o_glo_cmt(ap->l.user);
      printf("@allow %s = %s\n", ap->lhs, ap->rhs);
    }
}

static void
o_glo_bases(struct entry *e)
{
  List_node *outer;
  int i;
  o_glo_cmt(((struct loctok *)(e->bases->first->data))->l.user);
  printf("@bases");
  for (i = 0, outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      struct loctok *ltp = inner->data;
      if (i++)
	printf("; ");
      else
	printf(" ");

      if (ltp->lang)
	printf("%%%s ", ltp->lang);
      printf("%s", ltp->tok); 
      if (list_len(bp) > 1)
	{
	  int j;
	  printf(" (");
	  for (j = 0, inner = inner->next; inner; inner = inner->next)
	    {
	      if (j++)
		printf(", ");
	      printf("%s", ((struct loctok *)(inner->data))->tok);
	    }
	  printf(")");
	}
    }
  printf("\n");
}

static void
o_glo_cbd(struct cbd *c)
{
  printf("@project %s\n@lang %s\n@name %s\n", c->project, c->lang, c->name);
  if (list_len(c->propss))
    list_exec(c->propss, (list_exec_func*)o_glo_props);
  printf("\n");
}

static void
o_glo_cmt(List *c)
{
  if (c)
    {
      List_node *lp;
      for (lp = c->first; lp; lp = lp->next)
	printf("#%s\n", (char*)lp->data);
    }
}

static void
o_glo_dcfs(struct entry *e)
{
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      struct tag *tp = hash_find(e->hdcfs, lhs);
      o_glo_cmt(tp->l.user);
      printf("@%s %s\n", lhs, tp->val);
    }
}

static void
o_glo_entry(struct entry *e)
{
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case ADD_E:
	  putchar('+');
	  break;
	case DEL_E:
	  putchar('-');
	  break;
	default:
	  break;
	}
    }
  o_glo_cmt(e->l.user);
  printf("@entry %s\n", e->cgp->loose);
  if (e->ed)
    {
      switch (e->ed->type)
	{
	case DEL_E:
	  printf("#why: %s\n", e->ed->why);
	  break;
	case REN_E:
	  printf("> %s\n", e->ed->target->tight);
	  break;
	case MRG_E:
	  printf(">> %s\n", e->ed->target->tight);
	  break;
	default:
	  break;
	}
    }
  if (e->disc)
    {
      o_glo_cmt(e->disc->l.user);
      printf("@disc %s\n", e->disc->val);
    }
}

static void
o_glo_end_cbd(struct cbd *c)
{
  /* nothing to do in identity mode */
}

static void
o_glo_end_entry(struct entry *e)
{
  if (e->end_entry)
    o_glo_cmt(e->end_entry->user);
  printf("@end entry\n\n");
}

static void
o_glo_forms(struct entry *e)
{
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  Form *f2p = (Form*)(lp->data);
	  o_glo_cmt(f2p->user);
	  printf("@form %s", f2p->form);
	  if (f2p->lang) /* careful: we only should only emit this if lang is explicit in form */
	    printf(" %%%s", f2p->lang);
	  if (f2p->base)
	    printf(" /%s", f2p->base);
	  if (f2p->stem)
	    printf(" *%s", f2p->stem);
	  if (f2p->cont)
	    printf(" +%s", f2p->cont);
	  if (f2p->morph)
	    printf(" #%s", f2p->morph);
	  if (f2p->morph2)
	    printf(" ##%s", f2p->morph2);
	  if (f2p->norm)
	    printf(" $%s", f2p->norm);
	  printf("\n");
	}
    }
}

static void
o_glo_meta(struct entry *e)
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
		o_glo_cmt(mo->l.user);
		printf("@equiv %%%s %s\n", val->lang, val->text);
	      }
	      break;
	    case PL_ID:
	      {
		struct pleiades *val = (struct pleiades*)(mo->val);
		o_glo_cmt(val->l_coord.user);
		printf("@pl_coord %s\n", val->coord);
		o_glo_cmt(val->l_id.user);
		printf("@pl_id %s\n\n", val->id);
		/* still need to emit val->pl_aliases */
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
	    {
	      o_glo_cmt(mo->l.user);
	      printf("@%s %s\n", at, (const char*)mo->val);
	    }
	}  
    }
}

static void
o_glo_parts(struct entry *e)
{
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      struct parts *p = (struct parts*)lp->data;
      if (p->cgps && list_len(p->cgps))
	{
	  o_glo_cmt(p->l.user);
	  printf("@parts");
	  List_node *cp;
	  for (cp = p->cgps->first; cp; cp = cp->next)
	    printf(" %s", ((struct cgp*)(cp->data))->tight);
	  printf("\n");
	}
    }
}

static void
o_glo_phon(struct entry *e)
{
  o_glo_cmt(e->phon->l.user);
  printf("@phon %s\n", (ccp)e->phon->val);
}

static void
o_glo_props(const char *p)
{
  printf("@props %s\n", p);
}

static void
o_glo_root(struct entry *e)
{
  o_glo_cmt(e->root->l.user);
  printf("@root %s\n", (ccp)e->root->val);
}

static void
o_glo_senses(struct entry *e)
{
  if (e->begin_senses)
    {
      o_glo_cmt(e->begin_senses->user);
      printf("@senses\n");
    }

  List_node *lp;
  for (lp = e->senses->first; lp; lp = lp->next)
    {
      struct sense *sp = (struct sense*)(lp->data);

      o_glo_cmt(sp->l.user);

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
      
      printf("@sense");
      if (sp->lng)
	{ printf(" %%%s", sp->lng); }
      if (sp->sid)
	printf(" #%s", sp->sid);
      if (sp->num)
	printf(" .%s", sp->num);
      if (sp->sgw)
	printf(" [%s]", sp->sgw);
      if (sp->pos)
	printf(" %s", sp->pos);
      if (sp->mng)
	printf(" %s", sp->mng);
      printf("\n");
      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case REN_S:
	      printf("> %s %s\n", sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    case MRG_S:
	      printf(">> %s %s\n", sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    default:
	      break;
	    }
	}
      if (sp->disc)
	{
	  o_glo_cmt(sp->disc->l.user);
	  printf("@disc %s\n", sp->disc->val);
	}
    }
  
  if (e->begin_senses)
    {
      o_glo_cmt(e->end_senses->user);
      printf("@end senses\n");
    }
}

static void
o_glo_stems(struct entry *e)
{
  List_node *lp;

  o_glo_cmt(((struct loctok *)(e->stems->first->data))->l.user);
  printf("@stems");
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = ((ucp)(lp->data));
      printf(" %s", stem);
    }
  printf("\n");
}

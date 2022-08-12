#include <stdio.h>
#include "gx.h"
#include "grammar.tab.h"

#include "identity_fncs.c"

extern void iterator(struct cbd *c, iterator_fnc fncs[]);

static void i_cmt(List *c);
static void i_props(const char *p);

void
identity(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
i_aliases(struct entry *e)
{
  List_node *lp;
  for (lp = e->aliases->first; lp; lp = lp->next)
    {
      struct alias *ap = lp->data;
      i_cmt(ap->l.cmt);
      printf("@alias %s\n", ap->cgp->tight);
    }
}

static void
i_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      struct allow *ap = lp->data;
      i_cmt(ap->l.cmt);
      printf("@allow %s = %s\n", ap->lhs, ap->rhs);
    }
}

static void
i_bases(struct entry *e)
{
  List_node *outer;
  int i;
  i_cmt(((struct loctok *)(e->bases->first->data))->l.cmt);
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
i_cbd(struct cbd *c)
{
  printf("@project %s\n@lang %s\n@name %s\n", c->project, c->lang, c->name);
  if (list_len(c->propss))
    list_exec(c->propss, (list_exec_func*)i_props);
  printf("\n");
}

static void
i_cmt(List *c)
{
  if (c)
    {
      List_node *lp;
      for (lp = c->first; lp; lp = lp->next)
	printf("#%s\n", (char*)lp->data);
    }
}

static void
i_dcfs(struct entry *e)
{
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      struct tag *tp = hash_find(e->hdcfs, lhs);
      i_cmt(tp->l.cmt);
      printf("@%s %s\n", lhs, tp->val);
    }
}

static void
i_entry(struct entry *e)
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
  i_cmt(e->l.cmt);
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
      i_cmt(e->disc->l.cmt);
      printf("@disc %s\n", e->disc->val);
    }
}

static void
i_end_cbd(struct cbd *c)
{
  /* nothing to do in identity mode */
}

static void
i_end_entry(struct entry *e)
{
  i_cmt(e->end_entry->cmt);
  printf("@end entry\n\n");
}

static void
i_forms(struct entry *e)
{
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  struct f2 *f2p = (struct f2*)(lp->data);
	  i_cmt(f2p->user);
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
i_meta(struct entry *e)
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
		i_cmt(mo->l.cmt);
		printf("@equiv %%%s %s\n", val->lang, val->text);
	      }
	      break;
	    case PLEIADES:
	      {
		struct pleiades *val = (struct pleiades*)(mo->val);
		i_cmt(val->l_coord.cmt);
		printf("@pl_coord %s\n", val->coord);
		i_cmt(val->l_id.cmt);
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
	      i_cmt(mo->l.cmt);
	      printf("@%s %s\n", at, (const char*)mo->val);
	    }
	}  
    }
}

static void
i_parts(struct entry *e)
{
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      struct parts *p = (struct parts*)lp->data;
      if (p->cgps && list_len(p->cgps))
	{
	  i_cmt(p->l.cmt);
	  printf("@parts");
	  List_node *cp;
	  for (cp = p->cgps->first; cp; cp = cp->next)
	    printf(" %s", ((struct cgp*)(cp->data))->tight);
	  printf("\n");
	}
    }
}

static void
i_phon(struct entry *e)
{
  i_cmt(e->phon->l.cmt);
  printf("@phon %s\n", (ccp)e->phon->val);
}

static void
i_props(const char *p)
{
  printf("@props %s\n", p);
}

static void
i_root(struct entry *e)
{
  i_cmt(e->root->l.cmt);
  printf("@root %s\n", (ccp)e->root->val);
}

static void
i_senses(struct entry *e)
{
  if (e->begin_senses)
    {
      i_cmt(e->begin_senses->cmt);
      printf("@senses\n");
    }

  List_node *lp;
  for (lp = e->senses->first; lp; lp = lp->next)
    {
      struct sense *sp = (struct sense*)(lp->data);

      i_cmt(sp->l.cmt);

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
	  i_cmt(sp->disc->l.cmt);
	  printf("@disc %s\n", sp->disc->val);
	}
    }
  
  if (e->begin_senses)
    {
      i_cmt(e->end_senses->cmt);
      printf("@end senses\n");
    }
}

static void
i_stems(struct entry *e)
{
  List_node *lp;

  i_cmt(((struct loctok *)(e->stems->first->data))->l.cmt);
  printf("@stems");
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = ((ucp)(lp->data));
      printf(" %s", stem);
    }
  printf("\n");
}

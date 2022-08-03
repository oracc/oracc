#include <stdio.h>
#include "gx.h"

#include "iterator_fncs.c"

extern void iterator(struct cbd *c, iterator_fnc fncs[]);

static void f_proplist(const char *p);

void
identity(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  iterator(cbd,fncs);
  free(fncs);
}

static void
f_alias(struct alias *a)
{
  printf("@alias %s\n", a->cgp->tight);
}

static void
f_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->b_allow, lhs);
      printf("@allow %s = %s\n", lhs, rhs);
    }
}

static void
f_bases(struct entry *e)
{
  List_node *outer;
  int i;
  printf("@bases");
  for (i = 0, outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      if (i++)
	printf("; ");
      else
	printf(" ");
      printf("%s", (const char *)inner->data);
      if (list_len(bp) > 1)
	{
	  int j;
	  printf(" (");
	  for (j = 0, inner = inner->next; inner; inner = inner->next)
	    {
	      if (j++)
		printf(", ");
	      printf("%s", (const char *)inner->data);
	    }
	  printf(")");
	}
    }
  printf("\n");
}

static void
f_cbd(struct cbd *c)
{
  printf("@project %s\n@lang %s\n@name %s\n", c->project, c->lang, c->name);
  if (list_len(c->proplists))
    list_exec(c->proplists, (list_exec_func*)f_proplist);
  printf("\n");
}

static void
f_entry(struct entry *e)
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
    printf("@disc %s\n", e->disc);
}

static void
f_end_cbd(struct cbd *c)
{
  /* nothing to do in identity mode */
}

static void
f_end_entry(struct entry *e)
{
  printf("@end entry\n\n");
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
f_parts(struct entry *e)
{
  if (e->parts->cgps && list_len(e->parts->cgps))
    {
      printf("@parts");
      List_node *lp;
      for (lp = e->parts->cgps->first; lp; lp = lp->next)
	printf(" %s", ((struct cgp*)(lp->data))->tight);
      printf("\n");
    }
}

static void
f_phon(struct entry *e)
{
  printf("@phon %s\n", (ccp)e->phon);
}

static void
f_proplist(const char *p)
{
  printf("@proplist %s\n", p);
}

static void
f_root(struct entry *e)
{
  printf("@root %s\n", (ccp)e->root);
}

static void
f_senses(struct entry *e)
{
  if (e->beginsenses)
    printf("@senses\n");

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
	printf("@disc %s\n", sp->disc);
    }
  
  if (e->beginsenses)
    printf("@end senses\n");
}

static void
f_stems(struct entry *e)
{
  List_node *lp;
  printf("@stems");
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = ((ucp)(lp->data));
      printf(" %s", stem);
    }
  printf("\n");
}

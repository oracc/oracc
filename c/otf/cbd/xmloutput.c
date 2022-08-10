#include <stdio.h>
#include "rnvif.h"
#include "gx.h"
#include "grammar.tab.h"
#include "xmloutput_fncs.c"
extern void iterator(struct cbd *c, iterator_fnc fncs[]);
static void xo_proplist(const char *p);

#define f0()
#define f1(a)
#define f2(a,b)
#define f3(a,b,c)

static const char *cbd2ns = "http://oracc.org/ns/cbd/2.0";

/* XMLIFY */

static void
rnvxml_ch(const char *ch)
{
  rnv_characters(NULL, ch, strlen(ch));
  fputs(ch,f_xml);
}
static void
rnvxml_ea(const char *qname, ...)
{
  const char **atts = NULL, *arg;
  int nargs = 0;
  va_list ap;
  va_start(ap, qname);
  while (va_arg(ap, const char*))
    {
      if (NULL == arg)
	break;
      ++nargs;
    }
  va_end(ap);
  if (nargs)
    {
      atts = malloc((nargs+1) * sizeof(char*));
      nargs = 0;
      va_start(ap, qname);
      while ((arg = va_arg(ap, const char*)))
	{
	  if (NULL == arg)
	    break;
	  atts[nargs++] = arg;
	  atts[nargs++] = va_arg(ap, const char *);
	}
      atts[nargs] = NULL;
    }
  rnv_start_element(NULL,qname,atts);
  fprintf(f_xml, "<%s", qname);
  if (nargs > 1)
    {
      for (nargs = 0; atts[nargs]; nargs += 2)
	fprintf(f_xml, " %s=\"%s\"", atts[nargs], atts[nargs+1]);
    }
  fprintf(f_xml, ">");
  free(atts);
}
static void
rnvxml_ee(const char *qname)
{
  rnv_end_element(NULL,qname);
  fprintf(f_xml, "</%s>", qname);
}

void
xmloutput(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  f_xml = stdout;
  /*rnvxml_ea("rnvwrapper", "xmlns", cbd2ns, NULL);*/
  iterator(cbd,fncs);
  /*rnvxml_ee("rnvwrapper");*/
  free(fncs);
}

static void
xo_aliases(struct entry *e)
{
  List_node *lp;
  rnvxml_ea("aliases", (ccp)NULL);
  for (lp = e->aliases->first; lp; lp = lp->next)
    {
      rnvxml_ea("alias", "target", ((struct alias *)(lp->data))->cgp->tight, NULL);
      rnvxml_ee("alias");
    }
  rnvxml_ee("aliases");
}

static void
xo_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      struct allow *ap = lp->data;
      f2(ap->lhs, ap->rhs);
    }
}

static void
xo_bases(struct entry *e)
{
  List_node *outer;
  int i;

  for (i = 0, outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      if (i++)
	/* ; */;
      else
	/* */;
      f1((const char *)inner->data);
      if (list_len(bp) > 1)
	{
	  int j;
	  /* ( */;
	  for (j = 0, inner = inner->next; inner; inner = inner->next)
	    {
	      if (j++)
		/* , */;
	      f1((const char *)inner->data);
	    }
	  /* ) */;
	}
    }
}

static void
xo_cbd(struct cbd *c)
{
  rnvxml_ea("cbd",
	    "xmlns", cbd2ns,
	    "xmlns:c", cbd2ns,
	    "project", c->project,
	    "xml:lang", c->lang,
	    "name", c->name,
	    (ccp)NULL);
}

static void
xo_dcfs(struct entry *e)
{
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->hdcfs, lhs);
      f2(lhs, rhs);
    }
}

static void
xo_entry(struct entry *e)
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
	  f1(/* #why */ e->ed->why);
	  break;
	case REN_E:
	  f1(/* > */ e->ed->target->tight);
	  break;
	case MRG_E:
	  f1(/* >> */ e->ed->target->tight);
	  break;
	default:
	  break;
	}
    }
  rnvxml_ea("entry", NULL);
  if (e->disc)
    f1(/* @disc */ e->disc);
}

static void
xo_end_cbd(struct cbd *c)
{
#if 1
  rnvxml_ee("cbd");
#else
  fprintf(f_xml, "</cbd>");
#endif
}

static void
xo_end_entry(struct entry *e)
{
  rnvxml_ee("entry");
}

static void
xo_forms(struct entry *e)
{
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  struct f2 *f2p = (struct f2*)(lp->data);
	  f1(/* @form */ f2p->form);
	  if (f2p->lang) /* careful: we only should only emit this if lang is explicit in form */
	    f1(/* %% */ f2p->lang);
	  if (f2p->base)
	    f1(/* /%s */ f2p->base);
	  if (f2p->stem)
	    f1(/* * */ f2p->stem);
	  if (f2p->cont)
	    f1(/* + */ f2p->cont);
	  if (f2p->morph)
	    f1(/* # */ f2p->morph);
	  if (f2p->morph2)
	    f1(/* ## */ f2p->morph2);
	  if (f2p->norm)
	    f1(/* $ */ f2p->norm);
	}
    }
}

static void
xo_meta(struct entry *e)
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
		f2(/* @equiv % */ val->lang, val->text);
	      }
	      break;
	    case PLEIADES:
	      {
		struct pleiades *val = (struct pleiades*)(mo->val);
		f3(/* @pl_coord @pl_id @pl_uid */ val->coord, val->id, val->uid);
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
	    f1(/* at */ (const char*)mo->val);
	}
    }  
}

static void
xo_parts(struct entry *e)
{
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      struct parts *p = (struct parts*)lp->data;
      printf("<partss>");
      if (p->cgps && list_len(p->cgps))
	{
	  printf("<parts>");
	  List_node *cp;
	  for (cp = p->cgps->first; cp; cp = cp->next)
	    printf("<cgp>%s</cgp>", ((struct cgp*)(cp->data))->tight);
	  printf("</parts>");
	}
      printf("</partss>");
    }
}

static void
xo_phon(struct entry *e)
{
  f1(/* @phon */ (ccp)e->phon);
}

static void
xo_proplist(const char *p)
{
  f1(/* @proplist */ p);
}

static void
xo_root(struct entry *e)
{
  f1(/* @root */ (ccp)e->root);
}

static void
xo_senses(struct entry *e)
{
  if (e->begin_senses)
    f1(/* @senses */ );

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
      
      f0(/* @sense */ );
      if (sp->lng)
	{ f1(/* % */ sp->lng); }
      if (sp->sid)
	f1(/* # */ sp->sid);
      if (sp->num)
	f1(/* . */ sp->num);
      if (sp->sgw)
	f1(/* [%s] */ sp->sgw);
      if (sp->pos)
	f1(/*  */ sp->pos);
      if (sp->mng)
	f1(/*  */ sp->mng);

      if (sp->ed)
	{
	  switch (sp->ed->type)
	    {
	    case REN_S:
	      f2(/* > %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    case MRG_S:
	      f2(/* >> %s */ sp->ed->sp->pos, sp->ed->sp->mng);
	      break;
	    default:
	      break;
	    }
	}
      if (sp->disc)
	f1(/* @disc */ sp->disc);
    }
  
  if (e->begin_senses)
    f0(/* @end senses */ );
}

static void
xo_stems(struct entry *e)
{
  List_node *lp;
  f0(/* @stems */ );
  for (lp = e->stems->first; lp; lp = lp->next)
    {
      unsigned char *stem = /* (ucp)(lp->data) */"";
      f1(stem);
    }
}

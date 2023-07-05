#include <stdio.h>
#include <gdl.h>
#include "gx.h"
#include "validate_fncs.c"
extern void iterator(struct cbd *c, iterator_fnc fncs[]);
static void v_props(const char *p);

#define f0()
#define f1(a)
#define f2(a,b)
#define f3(a,b,c)

static int bases_done = 0;

static YYLTYPE loc;

void
validator(struct cbd*cbd)
{
  iterator_fnc *fncs = ifnc_init();
  phase = "val";
  iterator(cbd,fncs);
  mesg_print(stderr);
  free(fncs);
}

static void
v_aliases(struct entry *e)
{
#if 0
  List_node *lp;
  for (lp = e->aliases->first; lp; lp = lp->next)
    {
      struct alias *ap = lp->data;
      lnum = ap->l.line; file = ap->l.file;
      vwarning("vwarning test on alias %s", ap->cgp->tight);
    }
#endif
}

static void
v_allow(struct entry *e)
{
  List_node *lp;
  for (lp = e->allows->first; lp; lp = lp->next)
    {
      struct allow *ap = lp->data;
      const char *sig = NULL;
     
      /*lnum = ap->l.line; file = ap->l.file;*/
      /* warning("validating @allow\n"); */
      
      /* check that lhs and rhs both resolve to gdl sigs */
      if ((sig = gdlsig_str(&ap->l, ap->lhs,1,1)))
	{
	  ap->lsig = pool_copy((ucp)sig, e->owner->pool);
	  hash_add(e->b_allow, ap->lsig, ap);
	}
      if ((sig = gdlsig_str(&ap->l, ap->rhs,1,1)))
	{
	  ap->rsig = pool_copy((ucp)sig, e->owner->pool);
	  hash_add(e->b_allow, ap->rsig, ap);
	}
    }
}

static void
v_allow_2(struct entry *e)
{
  if (e->allows)
    {
      List_node *lp;
      for (lp = e->allows->first; lp; lp = lp->next)
	{
	  struct allow *ap = lp->data;
	  unsigned char *sig = NULL;
	  loc.line = ap->l.line; loc.file = (char*)ap->l.file;
	  sig = hash_find(e->b_sig, ap->lsig);
	  if (!sig)
	    mesg_verr(&loc,"@allow left side %s is not a base", ap->lhs);
	  sig = hash_find(e->b_sig, ap->rsig);
	  if (!sig)
	    mesg_verr(&loc,"@allow right side %s is not a base", ap->rhs);
	}
    }
}

static int
allowed(struct entry *e, unsigned char *a, unsigned char *b)
{
  if (e && e->b_allow)
    {
      Hash *allowed = hash_find(e->b_allow, a);
      if (allowed && hash_find(allowed, b))
        return 1;
    }
  return 0;
}

static void
v_bases(struct entry *e)
{
  List_node *outer;

  const char *entry_phase = phase;
  phase = "bases";
  
  e->b_pri = hash_create(1);
  e->b_alt = hash_create(1);
  e->b_sig = hash_create(1);

  for (outer = e->bases->first; outer; outer = outer->next)
    {
      List *bp = ((List *)(outer->data));
      List_node *inner = bp->first;
      struct loctok *ltp = (struct loctok *)inner->data;
      const char *sig = NULL, *pri_sig;
      unsigned char *pri;
      
      pri = ltp->tok;

      if ((sig = gdlsig_str(&ltp->l,pri,1,1)))
	{
	  unsigned char *known_sig = NULL;
	  if (verbose)
	    fprintf(stderr, "%s:%d: registering pri %s with sig %s\n", file, lnum, pri, sig);

	  if ((known_sig = hash_find(e->b_sig, (ucp)sig)) && !allowed(e, pri, known_sig))
	    {
	      mesg_verr(&ltp->l, "duplicate or equivalent primary base %s ~~ %s", pri, known_sig);
	    }
	  else
	    {
	      pri_sig = (ccp)pool_copy((ucp)sig, e->owner->pool);
	      hash_add(e->b_pri, pri, (ucp)pri_sig);
	      hash_add(e->b_sig, (ucp)pri_sig, pri);
	    }
	  /*free((char *)sig);*/ /* CHECK ME! */
	}
      else
	mesg_verr(&ltp->l,"gdlsig_str failed on %s", pri);
  
      /* Additional list members are alt bases */
      if (list_len(bp) > 1)
	{
	  for (inner = inner->next; inner; inner = inner->next)
	    {
	      struct loctok *ltp = (struct loctok *)inner->data;
	      unsigned char *alt = NULL;
	      const char *alt_sig = NULL;
	      
	      alt = ltp->tok;
	      if (verbose)
		fprintf(stderr, "%s:%d: adding alt %s to pri %s\n", file, lnum, alt, pri);
	      alt_sig = gdlsig_str(&ltp->l, alt,1,1);
	      if (strcmp((ccp)pri_sig,(ccp)alt_sig))
		mesg_verr(&ltp->l, "alt %s is not equivalent to primary %s (%s != %s)", alt, pri, alt_sig, pri_sig);
	      else
		hash_add(e->b_alt, alt, pri);	      
	      f1((const char *)inner->data);
	    }
	}
    }

  phase = entry_phase;
  
  v_allow_2(e);
}

static void
v_cbd(struct cbd *c)
{
  f3(c->project, c->lang, c->name);
  if (list_len(c->propss))
    list_exec(c->propss, (list_exec_func*)v_props);
}

static void
v_dcfs(struct entry *e)
{
  List_node *lp;
  for (lp = e->dcfs->first; lp; lp = lp->next)
    {
#if 0
      unsigned char *lhs = ((ucp)(lp->data));
      unsigned char *rhs = hash_find(e->hdcfs, lhs);
      f2(lhs, rhs);
#endif
    }
}

static void v_entry_edit(struct entry *e);

static void
v_entry(struct entry *e)
{
  bases_done = 0;
  
  if (!cbdpos((ccp)e->cgp->pos, strlen((ccp)e->cgp->pos)))
    mesg_verr(&e->l, "unknown POS %s", e->cgp->pos);

  if (e->ed)
    v_entry_edit(e);
  
  if (e->disc)
    /* One day this will be in OTF inline */;
}

static void
v_entry_edit(struct entry *e)
{
  /* struct sense *sp = NULL; */
  if (NULL != e->ed)
    {
      if (e->ed->target)
	{
	  if (hash_find(e->owner->hentries, e->ed->target->tight))
	    {
	      if (verbose)
		fprintf(stderr, "target %s OK\n", e->ed->target->tight);
	      /* if we are renaming this is an error */
	      if (e->ed->type == REN_E)
		mesg_verr(e->ed->lp, "can't rename to existing entry %s", e->ed->target->tight);
	    }
	  else
	    {
	      if (verbose)
		fprintf(stderr, "target %s NOT\n", e->ed->target->tight);
	      /* if we are merging this is an error */
	      if (e->ed->type == MRG_E)
		mesg_verr(e->ed->lp,
			     "can't merge to non-existent entry %s", e->ed->target->tight);
	    }
	}
    }
#if 0
  for (sp = list_first(e->senses); sp; sp = list_next(e->senses))
    if (sp->ed)
      /* should validate sense merges as well? Is this used? Is it useful? */
#endif
}


static void
v_end_cbd(struct cbd *c)
{
  ;
}

static void
v_end_entry(struct entry *e)
{
  /* @end entry */
}

static void
v_forms(struct entry *e)
{
  if (e->forms && list_len(e->forms))
    {
      List_node *lp;
      for (lp = e->forms->first; lp; lp = lp->next)
	{
	  Form *f2p = (Form*)(lp->data);
	  f1(/* @form */ f2p->form);
	  if (f2p->lang) /* careful: we only should only emit this if lang is explicit in form */
	    f1(/* %% */ f2p->lang);
	  if (f2p->base)
	    {
	      if (!hash_find(e->b_pri, f2p->base))
		{
		  static YYLTYPE l;
		  unsigned char *pri = hash_find(e->b_alt, f2p->base);
		  l.file = (char*)f2p->file;
		  l.line = f2p->lnum;
		  if (pri)
		    mesg_verr(&l, "alt base %s should be primary %s", f2p->base, pri);
		  else
		    {
		      const char *sig;
		      sig = gdlsig_str(&l, (ucp)f2p->base,1,1);
		      if (sig)
			{
			  unsigned char *sig_pri = hash_find(e->b_sig, (ucp)sig);
			  if (sig_pri)
			    mesg_verr(&l, "form base %s should be %s", f2p->base, sig_pri);
			  else
			    mesg_verr(&l, "base %s not found in bases", f2p->base);
			}
		      else
			{
			  mesg_verr(&l,"gdlsig_str failed on %s", pri);
			}
		    }
		}
	    }
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
v_meta(struct entry *e)
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
#if 0
		struct equiv *val = (struct equiv *)(mo->val);
		f2(/* @equiv % */ val->lang, val->text);
#endif
	      }
	      break;
	    case PL_ID:
	      {
#if 0
		struct pleiades *val = (struct pleiades*)(mo->val);
		f3(/* @pl_coord @pl_id @pl_uid */ val->coord, val->id, val->uid);
#endif
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
v_parts(struct entry *e)
{
  List_node *lp;
  for (lp = e->parts->first; lp; lp = lp->next)
    {
      struct parts *p = (struct parts*)lp->data;
      if (p->cgps && list_len(p->cgps))
	{
	  /* @parts */;
	  List_node *cp;
	  for (cp = p->cgps->first; cp; cp = cp->next)
	    f1(((struct cgp*)(cp->data))->tight);
	}
    }
}

static void
v_phon(struct entry *e)
{
  f1(/* @phon */ (ccp)e->phon);
}

static void
v_props(const char *p)
{
  f1(/* @proplist */ p);
}

static void
v_root(struct entry *e)
{
  f1(/* @root */ (ccp)e->root);
}

static void
v_senses(struct entry *e)
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
	{
	  if (!cbdpos((ccp)sp->pos, strlen((ccp)sp->pos)))
	    mesg_verr(&sp->l, "unknown POS %s", sp->pos);
	}
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
v_stems(struct entry *e)
{
  List_node *lp;
  f0(/* @stems */ );
  for (lp = e->stems->first; lp; lp = lp->next)
    {
#if 0
      unsigned char *stem = /* (ucp)(lp->data) */"";
      f1(stem);
#endif
    }
}

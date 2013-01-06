#include "warning.h"
#include "lang.h"
#include "sigs.h"
#include "ilem.h"
#include "f2.h"
#include "xcl.h"
#include "list.h"
#include "memblock.h"

extern int lem_extended;

static void sigs_d_new(struct xcl_context *xcp, struct xcl_d *d);
static void sigs_l_new(struct xcl_context *xcp, struct xcl_l *l);
static struct sig const* const*sigs_form_in_sigset(struct xcl_context *xcp, 
						   struct ilem_form *ifp, 
						   struct sigset *sp, int *nfinds);

static struct siglook look_new = { "sigs_new", sigs_form_in_sigset , sig_look_new };

void
sig_new(struct xcl_context *xcp)
{
  xcl_map(xcp, NULL, NULL, sigs_d_new, sigs_l_new);
}

static void
sigs_d_new(struct xcl_context *xcp, struct xcl_d *d)
{
  if (sparse_lem)
    {
      if (d->type == xcl_d_field_start)
	xcl_set_sparse_skipping(d->subtype);
      else if (d->type == xcl_d_field_end)
	xcl_set_sparse_skipping(NULL);
    }
}

static void
sigs_l_new(struct xcl_context *xcp, struct xcl_l *l)
{
  if (sparse_skipping)
    {
      BIT_SET(l->f->instance_flags, ILEM_FLAG_SPARSE_SKIP);
      return;
    }

  if (!l->f)
    {
      if (l->sig)
	setup_ilem_form(xcp->sigs,l,npool_copy((Uchar *)l->sig,xcp->pool));
      else
	setup_ilem_form(xcp->sigs,l,npool_copy((Uchar *)l->inst,xcp->pool));
    }

  if (!l->f->f2.cf || !*l->f->f2.cf)
    {
      if (l->f->f2.gw && !strcmp((char*)l->f->f2.gw, "X"))
	l->f->f2.gw = NULL;
      if (l->f->f2.pos && !strcmp((char*)l->f->f2.pos, "X"))
	l->f->f2.pos = NULL;
      sigs_lookup(xcp, l, &look_new);
    }
  else
    {
      if (!BIT_ISSET(l->f->f2.flags, F2_FLAGS_LEM_NEW))
	sigs_l_check(xcp, l);
      else
	{
	  struct sig const * const *early_sigs = sigs_early_sig(xcp, l->f);
	  if (early_sigs)
	    sigs_cache_add(l->f, early_sigs);
	}
    }
}

static struct sig const * const *
sigs_form_in_sigset(struct xcl_context *xcp, struct ilem_form *ifp, 
		    struct sigset *sp, int *nfinds)
{
  struct sig *candidates = NULL, *c, **res = NULL;
  int ncand = 0;
  struct f2 *f = &ifp->f2;

  if ((candidates = hash_find(BIT_ISSET(f->flags,F2_FLAGS_LEM_BY_NORM)?sp->norms:sp->forms,
			      f->form)))
    {
      *nfinds = candidates->count;
      res = calloc((1+candidates->count),sizeof(struct sig *));
      for (c = candidates; c; c = c->next)
	{
	  if (!c->f2p)
	    f2_parse(c->set->file,c->lnum,npool_copy(c->sig,xcp->pool),
		     c->f2p = mb_new(xcp->sigs->mb_f2s),
		     NULL, xcp->sigs);
	  res[ncand++] = c;
	}
      res[ncand] = NULL;
      if (verbose)
	fprintf(stderr,"sigs_new.c: %d result%s for %s in %s:%s\n", ncand, (ncand==1)?"":"s",f->form, sp->project,sp->lang);
    }
  else if (verbose)
    fprintf(stderr,"sigs_new.c: no results for %s in %s:%s\n", f->form, sp->project,sp->lang);

  return (struct sig const * const *)res;
}

/* return 0 if all is OK; 1 for any error 
 * this ensures that only good sigs can get into the cache, and we don't
 * try to lemmatize based on bad data
 */
int
sigs_new_sig(struct xcl_context *xcp, struct ilem_form *fp)
{
  int status = 0;

  if (!fp->f2.pos)
    {
      if (fp->fcount && fp->finds && fp->finds[0]->f2.pos)
	fp->f2.pos = fp->finds[0]->f2.pos;
      else
	{
	  vwarning2(file, fp->lnum, "%s: must give POS with new lemm", fp->sublem);
	  ++status;
	}
    }

  if (!fp->f2.gw)
    {
      if (fp->fcount && fp->finds && fp->finds[0]->f2.gw)
	fp->f2.gw = fp->finds[0]->f2.gw;
      else
	{
	  vwarning2(file, fp->lnum, "%s: must give GW with new lemm", fp->sublem);
	  ++status;
	}
    }
  else if (fp->f2.sense)
    {
      if (strcmp((char*)fp->f2.gw, (char*)fp->f2.sense))
	{
	  if (!fp->f2.epos)
	    {
	      if (fp->fcount && fp->finds && fp->finds[0]->f2.epos)
		fp->f2.epos = fp->finds[0]->f2.epos;
	      else
		{
		  vwarning2(file, fp->lnum, "%s: must give EPOS when giving SENSE with new lemm", fp->sublem);
		  ++status;
		}
	    }
	}
      else if (!fp->f2.epos)
	fp->f2.epos = fp->f2.pos;
    }
  else
    {
      fp->f2.sense = fp->f2.gw;
      if (!fp->f2.epos)
	fp->f2.epos = fp->f2.pos;
    }

  if (BIT_ISSET(fp->f2.core->features,LF_BASE) && !fp->f2.base)
    {
#if 0
      if (fp->fcount && fp->finds && fp->finds[0]->f2.base)
	fp->f2.base = fp->finds[0]->f2.base;
      else
	{
	  vwarning2(file, fp->lnum, "%s: must give BASE with new lemm in lang %s", fp->sublem, fp->f2.core->name);
	  ++status;
	}
#else
      if (!fp->f2.base && !lem_extended)
	{
	  vwarning2(file, fp->lnum, "%s: must give BASE with new lemm in lang %s", fp->sublem, fp->f2.core->name);
	  ++status;
	}
#endif
    }
  if (BIT_ISSET(fp->f2.core->features,LF_NORM) && !fp->f2.norm)
    {
      if (fp->fcount && fp->finds[0]->f2.norm)
	fp->f2.norm = fp->finds[0]->f2.norm;
      else
	{
	  vwarning2(file, fp->lnum, "%s: must give NORM with new lemm in lang %s", fp->sublem, fp->f2.core->name);
	  ++status;
	}
    }

  if (status)
    BIT_SET(fp->f2.flags, F2_FLAGS_PARTIAL);

  fp->f2.project = (unsigned char*)xcp->project;
    
  fp->f2.sig = f2_sig(&fp->f2, xcp->pool);

  return status;
}

/* This routine is only called when a new lem (one flagged with '+') has
   to be added to the cache.  We have to fill in cf/gw/sense/pos/epos as
   necessary and then make a sig and return it in the appropriate format
   for the cache */
struct sig const * const *
sigs_early_sig(struct xcl_context *xcp, struct ilem_form *fp)
{
  struct sig ** ret = calloc(2, sizeof(struct sig*));
  List *sigs_for_lang = NULL;

#if 0
  if (!fp->f2.cf && fp->f2.norm)
    fp->f2.cf = fp->f2.norm;

  if (!fp->f2.gw && fp->f2.sense)
    fp->f2.gw = fp->f2.sense;
  else if (fp->f2.gw && !fp->f2.sense)
    fp->f2.sense = fp->f2.gw;

  if (!fp->f2.pos && fp->f2.epos)
    fp->f2.pos = fp->f2.epos;

  if (fp->f2.pos && !fp->f2.epos)
    fp->f2.epos = fp->f2.pos; /* This is a change in early versions of L2 which required explicit EPOS */
#endif

  if (sigs_new_sig(xcp, fp))
    return NULL;

  ret[0] = mb_new(xcp->sigs->mb_sigs);
  ret[0]->sig = fp->f2.sig;
  ret[0]->literal = (unsigned char*)fp->literal;
  sigs_for_lang = sig_context_register(xcp->sigs, xcp->project, (const char*)fp->f2.lang, 1);
  ret[0]->set = fp->sp = list_first(sigs_for_lang);
  ret[0]->f2p = &fp->f2;
  ret[0]->lnum = fp->lnum;

  ret[1] = NULL;

  return (struct sig const* const*)ret;
}

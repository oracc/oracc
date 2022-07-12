#include <ctype128.h>
#include "warning.h"
#include "memblock.h"
#include "npool.h"
#include "lang.h"
#include "sigs.h"
#include "f2.h"
#include "ilem_form.h"
#include "xcl.h"
#include "words.h"

extern int lem_dynalem;
extern int verbose;
extern const char *xcl_project;
extern int wordset_debug;

static struct siglook look_check = { "sigs_check", sigs_inst_in_sigset, sig_look_check };

static void
zero_word_pct(struct sig **s, int n)
{
  int i;
  for (i = 0; i < n; ++i)
    if (s[i] && s[i]->f2p && s[i]->f2p->words)
      s[i]->f2p->words->pct = 0;
}

void
sig_check(struct xcl_context *xcp)
{
  xcl_map(xcp, NULL, NULL, NULL, sigs_l_check);
}

void
sigs_l_check(struct xcl_context *xcp, struct xcl_l *l)
{
  /* Don't check COF_TAILS */
  if (l->f 
      && (l->f->f2.cf || l->f->f2.norm)
      && !BIT_ISSET(l->f->f2.flags, F2_FLAGS_INVALID)
      && (!BIT_ISSET(l->f->f2.flags,F2_FLAGS_COF_TAIL) 
	  && l->inst && l->inst[strlen(l->inst)-1] != '='))
    {
      sigs_lookup(xcp, l, &look_check);

      if (l->cof_tails)
	sigs_cof_finds(l);

#if 1
      if (BIT_ISSET(l->f->f2.flags, F2_FLAGS_LEM_NEW))
	{
	  /* 1) now we silently ignore missing .sig files we need to ignore that sp->file can be
	        NULL also 
	     2) sp->file == "cache" is no longer used, so that's out as well
 	   */
	  if (l->f->sp /* && l->f->sp->file && strcmp((const char *)l->f->sp->file, "cache") */
	      && !BIT_ISSET(l->f->f2.flags, F2_FLAGS_FROM_CACHE))
	    {
	      (void)sigs_early_sig(xcp, l->f);
	      /*
		struct sig const * const *early_sigs = sigs_early_sig(xcp, l->f);
		if (early_sigs)
		sigs_cache_add(l->f, early_sigs);
	      */
	    }
	}
#endif
    }
}

/* FIX ME: record what fails to match in flags somehow */

static void
setup_set(struct f2 *f)
{
  if (!f->words)
    {
      if (f->sense)
	f->words = w2_create_set(f->sense);
      else
	f->words = w2_create_set(f->gw);
    }
  else
    f->words->pct = 0;
}

int
field_ok(const Uchar *s1, const Uchar *s2)
{
  if ((s1 && strcmp((char*)s1,"X")) 
      && (s2 && strcmp((char*)s2,"X")))
    return !strcmp((char*)s1,(char*)s2);
  else
    return 1;
}

int
cfnorm_ok(struct f2 *f1, struct f2 *f2)
{
  if (f1->cf && f1->norm)
    {
      return field_ok(f1->cf,f2->cf) 
	&& field_ok(f1->norm,f2->norm);
    }
  else 
    {
      int ok = field_ok(f1->cf,f2->cf);
      if (!ok)
	{
	  /* If no norm is given, then the instance
	     form could be using the cf-position to
	     give a norm; otherwise the notation 
	     a[thing]N$ would result in norm=a */
	  if (f2->norm && BIT_ISSET(f2->core->features, LF_NORM))
	    /* if (f2->norm && BIT_ISSET(f2->flags, F2_FLAGS_LEM_BY_NORM)) */
	    {
	      ok = field_ok(f1->cf, f2->norm);
#if 0
	      /* No.  No NORM in core features has nothing to do
		 with checking, only with rendering */
	      if (!ok)
		return !BIT_ISSET(f2->core->features, LF_NORM);
	      else
		return ok;
#else
	      return ok;
#endif
	    }
	  else
	    return ok;
	}
      else
	{
	  /* Here we know that the CFs match and that no NORM was
	     specified.  By definition this is a match.
	   */
	  return ok;
	}
    }
}

int
posepos_ok(struct f2 *f1, struct f2 *f2)
{
  if (f1->pos && f1->epos)
    {
      return field_ok(f1->pos,f2->pos) 
	&& field_ok(f1->epos,f2->epos);
    }
  else if (f2->pos && f2->epos)
    {
      if (f1->pos) /* no f1->epos */
	return (field_ok(f1->pos,f2->pos) /* null f1->epos matches any f2->epos */
		/*&& !strcmp((char*)f2->pos, (char*)f2->epos) // this test was bad */
		)
	  || field_ok(f1->pos, f2->epos); /* allow sag[good]N to mean sag[good]'N */
      else if (f1->epos) /* a[blah]'N */
	return field_ok(f1->epos, f2->epos);
      else
#if 1
	/* pos/epos is sorted later on if there is more than one match; we can
	   allow defaulting of epos as long as there is only one match */
	return 1;
#else
	return !strcmp(f2->pos,f2->epos); /* if no POS is given on instance ignore senses where pos and epos differ */
#endif
    }
  else
    {
      return field_ok(f1->pos,f2->pos);
    }
}

/* f1 is the instance lemmatization; f2 is a candidate from the lemm-xxx.sig file */
int
xsense_ok(struct f2 *f1, struct f2 *f2, int gw_wild, const char *FILE, size_t LINE)
{
  int sub = 0;
  
  if ((f1->sense && f2->sense && !strcmp((char*)f1->sense, (char*)f2->sense))
      || (!f1->sense && f1->gw && f2->sense && !strcmp((char*)f1->gw, (char*)f2->sense))
      )
    {
      if (!f1->words)
	setup_set(f1);
      if (!f2->words)
	setup_set(f2);
      if (f1->words)
	f1->words->pct = 101;
      if (f2->words)
	f2->words->pct = 101;
      return 1;
    }

  if (gw_wild)
    {

#if 0 /* wait until end of subr to do this */
      if ((!f1->sense || !*f1->sense)
	  && (!f2->gw || !strcmp((char*)f1->gw,(char*)f2->gw)))
	{
	  if (!f1->words)
	    setup_set(f1);
	  if (!f2->words)
	    setup_set(f2);
	  f1->words->pct = f2->words->pct = 1;
	  return 1;
	}
#endif

      /* blank GW is a wildcard and matches any GW/SENSE */
      if (!f1->gw || !*f1->gw)
	{
	  if (!f1->words)
	    setup_set(f1);
	  if (!f2->words)
	    setup_set(f2);
	  if (f1->words)
	    f1->words->pct = 100;
	  if (f2->words)
	    f2->words->pct = 100;
	  return 1;
	}
    }

#if 0 /* NO: don't do this test here, do it after wordset stuff further down */
  else if (!f1->sense && f1->gw && f2->gw && !strcmp((char*)f1->gw, (char*)f2->gw))
    {
      if (!f1->words)
	setup_set(f1);
      f1->words->pct = 99;
      return 1;
    }
#endif

  /* else: setup_set will index gw instead of sense */
  setup_set(f1);
  setup_set(f2);
  sub = w2_subset(f2->words, f1->words);
  if (wordset_debug)
    fprintf(stderr, "[%s:%d]: wordset result = %d (f1->gw=%s; f2->gw=%s)\n", FILE, (int)LINE, sub, f1->gw, f2->gw);
  switch (sub)
    {
    case W2_PARTIAL:
    case W2_FULL:
      return 1;
    case W2_NONE:
    case W2_UNKNOWN:
      /* return 0 */; /* FALL THROUGH TO GW CHECK */
    }

  if (gw_wild)
    {
      if ((!f1->sense || !*f1->sense)
	  && (!strcmp((char*)f1->gw,(char*)f2->gw)))
	{
	  f1->words->pct = f2->words->pct = 1;
	  return 1;
	}
    }
  
  return 0;
}

/* Check that the scripts match without breaking any other
   L2 assumptions */
static int
script_ok(const Uchar *l1, const Uchar *l2)
{
  const Uchar *s1, *s2;
  if (!l1 || !l2)
    return 1;
  s1 = l1 + strlen((const char *)l1);
  s2 = l2 + strlen((const char *)l2);
  if (isdigit(s1[-1]) || isdigit(s2[-1]))
    {
      while (s1 > l1 && s2 > l2 && s1[-1] != '-' && s2[-1] != '-')
	{
	  if (*--s1 != *--s2)
	    return 0;
	}
      return s1 > l1 && s2 > l2 && s1[-1] == '-' && s2[-1] == '-';
    }
  return 1;
}

int
f2_test(struct f2 *f, struct f2 *f2)
{
  return sense_ok(f,f2,1)
    && f2_test_no_sense(f, f2)
    ;
}

int
f2_test_no_sense(struct f2 *f, struct f2 *f2)
{
  return posepos_ok(f,f2)
    && field_ok(f->base,f2->base)
    && field_ok(f->cont,f2->cont)
    && field_ok(f->morph,f2->morph)
    && field_ok(f->morph2,f2->morph2)
    && (!f->form || !strcmp((char*)f->form, "*") || script_ok(f->lang, f2->lang))
    ;
}

static int
psu_bit_ok(struct f2 *f1, struct f2 *f2)
{
  return BIT_ISSET(f1->flags, F2_FLAGS_IS_PSU)
    == BIT_ISSET(f2->flags, F2_FLAGS_IS_PSU);
}

/* We check COFs by first obeying only SENSE; if the calling 
   loop doesn't find any matches, cof_ok gets called again
   with force_sense = 0 to do a fuzzier search */
static int
cof_ok(struct ilem_form *ifp, struct f2 *f2p, int force_sense)
{
  int i, sense_null = 0;
  struct ilem_form *m;

  if (!f2p->parts)
    return 0;

  for (i = 0, m = ifp->multi; m && f2p->parts[i]; ++i, m = m->multi)
    {
      if (force_sense && !m->f2.sense && m->f2.gw)
	{
	  m->f2.sense = m->f2.gw;
	  sense_null = 1;
	}

      if (!cfnorm_ok(&m->f2, f2p->parts[i])
	  || !f2_test(&m->f2, f2p->parts[i]))
	{
	  if (verbose)
	    vnotice2(ifp->file, ifp->lnum, "cof_ok: no match to %s", ifp->literal);
	  if (sense_null)
	    m->f2.sense = NULL;
	  BIT_SET(m->f2.flags, F2_FLAGS_COF_INVALID);
	  return 0;
	}

      if (sense_null)
	m->f2.sense = NULL;
    }

  /* On success only, flag all the tails */
  for (m = ifp->multi; m; m = m->multi)
    {
      BIT_SET(m->f2.flags,F2_FLAGS_COF_TAIL);
      BIT_CLEAR(m->f2.flags,F2_FLAGS_COF_INVALID);
    }

  if (verbose)
    vnotice2(ifp->file, ifp->lnum, "cof_ok: found match to %s", ifp->literal);
  return 1;
}

static int skip_pass1 = 0;
struct sig const * const *
sigs_inst_in_sigset_pass2(struct xcl_context *xcp, struct ilem_form *ifp, 
		    struct sigset *sp, int *nfinds)
{
  struct sig const * const *ret = NULL;
  skip_pass1 = 1;
  ret = sigs_inst_in_sigset(xcp,ifp,sp,nfinds);
  skip_pass1 = 0;
  return ret;
}
/* This routine is only called from sigs_lookup when there is a CF */
struct sig const * const *
sigs_inst_in_sigset(struct xcl_context *xcp, struct ilem_form *ifp, 
		    struct sigset *sp, int *nfinds)
{
  struct sig *candidates = NULL, *c = NULL, **res, **part;
  int ncand = 0, pct_top = 0;
  int pass1 = 1, partial_ok = 0, pass_1_found_forms = 0, cof_pass_1 = 1;
  struct f2 *f = &ifp->f2;
  int sp_is_cache = (sp && sp->file && !strcmp((const char *)sp->file, "cache"));

  /* We don't check cof-tails directly, but from the head */
  if (ifp->type && !strcmp(ifp->type, "cof-tail"))
    return NULL;

  if (skip_pass1)
    pass1 = 0;
  
 top:

  /* select a candidate list: 
     try f->form
     if  0 candidates try f->cf and f->norm
     	use the shortest list >0 obtained from f->cf or cf->norm
   */
  if (pass1)
#if 1
    {
      if (BIT_ISSET(f->flags,F2_FLAGS_LEM_BY_NORM))
	candidates = hash_find(sp->norms, f->norm);
      else
	candidates = hash_find(sp->forms, f->form);
    }
#else
    candidates = hash_find(sp->forms,f->form);
#endif

#if 0
  else
    BIT_SET(f->flags,F2_FLAGS_NO_FORM);
#endif

  if (!candidates)
    {
      if (f->cf)
	{
	  if (BIT_ISSET(f->flags, F2_FLAGS_LEM_BY_NORM))
	    candidates = hash_find(sp->norms, f->norm);
	  else
	    {
	      /* short-circuit lookup by norm in cache unless we
		 are lemmatizing by normalization */
	      if (sp_is_cache)
		{
		  if (nfinds)
		    *nfinds = 0;
		  return NULL;
		}
	      candidates = hash_find(sp->norms,f->cf);
	      if (candidates && f->norm)
		{
		  struct sig *c2 = hash_find(sp->norms,f->norm);
		  if (c2 && c2->count < candidates->count)
		    candidates = c2;
		}
	      if (!pass_1_found_forms) /*  && !BIT_ISSET(f->flags, F2_FLAGS_LEM_NEW)) */
		BIT_SET(f->flags,F2_FLAGS_NO_FORM);
	    }
	}
    }
  else
    pass_1_found_forms = 1;

  if (!candidates || !candidates->count)
    return NULL;

  if (candidates == candidates->next)
    fprintf(stderr,"candidate ref loop\n");

  /* try all non-null fields for a match in the candidate list;
     keep all matching results
   */
  res = calloc((1+candidates->count),sizeof(struct sig *));
  part = calloc((1+candidates->count),sizeof(struct sig *));
 cof_pass_1:
  partial_ok = 0;
  for (c = candidates; c; c = c->next)
    {
      struct f2 *f2 = NULL;

      if (!c->f2p)
	{
	  f2_parse(c->set->file,c->lnum,npool_copy(c->sig,xcp->pool),
		   c->f2p = mb_new(xcp->sigs->mb_f2s),
		   NULL, sp->owner);
	}

#if 0
      /* This is wrong now that we allow lem of akk-x-stdbab against, e.g., ogca:akk */
      if (strcmp((const char *)f->lang, (const char *)c->f2p->lang))
	continue;
#endif

      f2 = c->f2p;
      if (psu_bit_ok(f,f2) && cfnorm_ok(f,f2))
	{
	  if (f2_test(f, f2))
	    {
	      if (ifp->multi)
		{
		  if (!cof_ok(ifp,f2,cof_pass_1))
		    {
		      if (partial_ok)
			memset(part, '\0', partial_ok * sizeof(struct sig*));
		      partial_ok = 0;
		      /* set COF_INVALID on the HEAD to indicate that the HEAD
			 is OK but one of the TAILs failed */
		      BIT_SET(f->flags, F2_FLAGS_COF_INVALID);
		      continue;
		    }
		  else
		    BIT_CLEAR(f->flags, F2_FLAGS_COF_INVALID);
		}
	      res[ncand] = c;
	      if (f->words)
		{
		  if (wordset_debug)
		    fprintf(stderr, "[%s:%d]: f->words->pct = %d; pct_top = %d\n",
			    __FILE__, __LINE__, f->words->pct, pct_top);
		  res[ncand]->pct = f->words->pct;
		  if (f->words->pct > pct_top)
		    pct_top = f->words->pct;
		}
	      else
		res[ncand]->pct = 0;
	      ++ncand;
	    }
	  else
	    {
	      if (!sp_is_cache)
		part[partial_ok++] = c;
	    }
	}
      else
	{
	  if (partial_ok)
	    memset(part, '\0', partial_ok * sizeof(struct sig*));
	  partial_ok = 0;
	}
    }

  if (pct_top >= 100 && !BIT_ISSET(f->flags,F2_FLAGS_PARTIAL) && !BIT_ISSET(f->flags,F2_FLAGS_NO_FORM))
    {
      int dest, i;
      for (i = dest = 0; i < ncand; ++i)
	{
	  if (res[i]->pct == pct_top)
	    {
	      if (dest < i)
		res[dest++] = res[i];
	      else
		++dest;
	    }
	}
      res[dest] = NULL;
      *nfinds = dest;
      return (struct sig const * const *)res;
    }
  
  /* CHECKME: is this unconditional return correct, or is the COF case
     different? */
  if (ncand == 0 && sp_is_cache)
    {
      if (nfinds)
	*nfinds = 0;
      return NULL;
    }

  if (ncand == 0 && ifp->multi && cof_pass_1)
    {
      cof_pass_1 = 0;
      goto cof_pass_1;
    }

  if (ncand == 0 && pass1)
    {
      if (partial_ok)
	{
	  free(res);
	  res = part;
	  ncand = partial_ok;
	  BIT_SET(f->flags,F2_FLAGS_PARTIAL);
	}
      else
	{
	  pass_1_found_forms = pass1 = 0;
	  free(res);
	  candidates = NULL;
	  goto top;
	}
    }
  else if (ncand > 1 && !lem_dynalem)
    {
      if (!f->sense)
	{
	  /* we do this now rather than setting
	     sense to gw at the very beginning because
	     leaving sense empty allows single-sense 
	     words to match even when only the GW is used */
	  int i, s_pct_top = 0;
	  f->sense = f->gw;
	  for (i = 0; i < ncand; ++i)
	    {
	      /* if gw matched on first pass pct = 1 */
	      if (res[i]->pct /* > 1 */ )
		{
		  if (res[i]->pct >= s_pct_top)
		    s_pct_top = res[i]->pct;
		  else
		    res[i]->pct = 0;
		}
	      else if (sense_ok(f,res[i]->f2p,0))
		{
		  if (f->words)
		    {
		      if (wordset_debug)
			fprintf(stderr, "[%s:%d]: f->words->pct = %d; pct_top = %d\n",
				__FILE__, __LINE__, f->words->pct, pct_top);
		      if (f->words->pct > s_pct_top)
			{
			  s_pct_top = f->words->pct;
			  res[i]->pct = f->words->pct;
			}
		    }
		  else
		    res[i]->pct = s_pct_top = 100;
		}
	      else
		{ /* f->words->pct can == 1 if gw match happened earlier when gw was wild */
		  if (f->words->pct > s_pct_top)
		    s_pct_top = f->words->pct;
		  res[i]->pct = f->words->pct;
		}
	    }
	  f->sense = NULL;
	  if (s_pct_top)
	    pct_top = s_pct_top;
	}
      if (pct_top)
	{
	  int src, dest;
	  for (src = dest = 0; src < ncand; ++src)
	    if (res[src]->pct == pct_top)
	      {
		if (dest == src)
		  ++dest;
		else
		  res[dest++] = res[src];
	      }
	  res[dest] = NULL;
	  ncand = dest;
	}
      else
	{
	  /* nothing to do */
	}
    }
  else if (ncand == 1)
    {
      /* nothing to do */
    }
  else
    {
      /* lem_dynalem || (ncand == 0 and pass1 == 1) */
    }
  if (part && res != part)
    free(part);

  *nfinds = ncand;

  if (ncand)
    zero_word_pct(res, ncand);
  
  return ncand ? (struct sig const * const *)res : NULL;
}

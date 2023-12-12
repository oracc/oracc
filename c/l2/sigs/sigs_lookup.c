#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "warning.h"
#include "hash.h"
#include "npool.h"
#include "memblock.h"
#include "lang.h"
#include "sigs.h"
#include "ilem.h"
#include "ilem_form.h"
#include "xcl.h"
#include "lang.h"

extern int fuzzy_aliasing;

static int multi_sub = 0;

#if 0
static void already_tried_aliasing_init(void);
static void already_tried_aliasing_term(void);
static int already_tried_aliasing(const char *form, struct f2 *f2);
#endif

static int
has_perfect_match(struct ilem_form **fpp, int nfinds)
{
  int i;
  for (i = 0; i < nfinds; ++i)
    if (fpp[i]->pct >= 100)
      return 1;
  return 0;
}

/* this must return 0 if the ref_fp->gw eq 0 */
static int 
select_gw_zero(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  return strcmp((char*)ref_fp->f2.pos,"0");
}

/* this must return 0 if the ref_fp->pos == fp->pos */
static int 
exact_pos_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (ref_fp->f2.pos && fp->f2.pos)
    return strcmp((char*)ref_fp->f2.pos,(char*)fp->f2.pos);
  else
    return 1;
}

/* this must return 0 if the ref_fp->pos == fp->epos */
static int 
implicit_epos_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (ref_fp->f2.pos && fp->f2.epos)
    return strcmp((char*)ref_fp->f2.pos,(char*)fp->f2.epos);
  else
    return 1;
}

/* this must return 0 if the fp->pos == fp->epos; it is used only when
   no POS or EPOS is specified on the instance.
 */
static int 
default_pos_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (fp->f2.pos && fp->f2.epos)
    return strcmp((char*)fp->f2.pos,(char*)fp->f2.epos);
  else
    return 1;
}

/* this must return 0 if the ref_fp->gw matches fp->sense.
 * Argument order in sense_ok is important--we want ref_fp to test as subset of fp.
 */
static int 
rematch_on_sense(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  int ret = sense_ok(&ref_fp->f2, &fp->f2, 0);
  if (ref_fp->f2.words)
    fp->pct = ref_fp->f2.words->pct;
  return !ret;
}

static int 
wordset_pct_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (fp && fp->pct >= 100)
    return 0;
  return 1;
}

/* this must return 0 if the ref_fp->norm == fp->cf */
static int 
implicit_norm_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (ref_fp->f2.cf && fp->f2.norm)
    return strcmp((char*)ref_fp->f2.cf,(char*)fp->f2.norm);
  else
    return 1;
}

#if 0
/* this must return 0 if the core languages are the same for fp and ref_fp */
static int 
context_lang_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (ref_fp->f2.core && fp->f2.core 
      && ref_fp->f2.core->name && fp->f2.core->name)
    return strcmp((char*)ref_fp->f2.core->name,(char*)fp->f2.core->name);
  else
    return 1;
}
#endif

/* this must return 0 if the fp's form is '*' indicating is normalized; ref_fp is ignored */
static int 
wild_form_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (fp->f2.form && !strcmp((char*)fp->f2.form, "*"))
    return 0;
  else
    return 1;
}

struct sig_find_state
{
  struct sigset *sp;
  struct ilem_form *fp;
  struct ilem_form **finds;
  const unsigned char *fp_aliased_form;
  int nfinds;
  struct sig const * const *sigs_found; 
  int no_form;
  int partial;
  int best;
};

static List *sig_state = NULL;

static struct sig_find_state *
sigs_state_best(void)
{
  struct sig_find_state *best_sans_form = NULL, *best_with_form = NULL, *curr = NULL;

  if (!sig_state || !list_len(sig_state))
    return NULL;

  if (list_len(sig_state) == 1)
    return list_first(sig_state);

  /* Choose the best match: 
       matches with a FORM are better than those without
       matches which are not PARTIAL are better than those which are
       matches with less ambiguity are better than those with more
       matches from earlier in the sigset list are better than later
   */
  for (curr = list_first(sig_state); curr; curr = list_next(sig_state))
    {
      if (curr->no_form)
	{
	  if (!best_sans_form
	      || curr->nfinds < best_sans_form->nfinds)
	    best_sans_form = curr;
	}
      else
	{
	  if (!best_with_form
	      || (best_with_form->partial && !curr->partial)
	      || curr->nfinds < best_with_form->nfinds)
	    best_with_form = curr;
	}
    }

  if (best_with_form)
    return best_with_form;
  else
    return best_sans_form;
}

static void
ss_list_xfree(struct sig_find_state *s)
{
#if 0
  if (!s->best && s->finds)
    free(s->finds);
#endif
  free(s);
}

/* FIXME: this causes memory leaks if finds that are
   not later used are not freed during sigs_state_free */
static void
sigs_state_free(void)
{
  if (sig_state)
    {
      list_free(sig_state,(list_free_func*)ss_list_xfree);
      sig_state = NULL;
    }
}

static void
sigs_state_init(void)
{
  if (sig_state)
    sigs_state_free();
  sig_state = list_create(LIST_SINGLE);
}

static void
sigs_state_save(struct sigset *sp, struct ilem_form *fp, 
		struct sig const * const *sigs_found, int nfinds)
{
  struct sig_find_state *s = malloc(sizeof(struct sig_find_state));
  s->sp = sp;
  s->fp = fp;
  if (fp->f2.oform)
    s->fp_aliased_form = fp->f2.form;
  else
    s->fp_aliased_form = NULL;
  s->finds = fp->finds;
  s->sigs_found = sigs_found;
  s->nfinds = nfinds;
  s->no_form = (BIT_ISSET(fp->f2.flags, F2_FLAGS_NO_FORM) ? F2_FLAGS_NO_FORM : 0);
  s->partial = (BIT_ISSET(fp->f2.flags, F2_FLAGS_PARTIAL) ? F2_FLAGS_PARTIAL : s->no_form);
  s->best = 0;
  list_add(sig_state, s);
}

void
setup_ilem_form(struct sig_context *scp, struct xcl_l *l, unsigned char*pinst)
{
  l->f = mb_new(scp->mb_ilem_forms);
  l->f->f2.owner = l->f;
  l->f->f2.lang = ++pinst;
  while (*pinst && ':' != *pinst)
    ++pinst;
  *pinst++ = '\0';
  l->f->f2.form = pinst;
  while (*pinst && '=' != *pinst)
    ++pinst;
  *pinst++ = '\0';
  if (!l->f->sublem)
    l->f->sublem = (char*)pinst;
  f2_parse((unsigned char*)l->parent->xc->file,l->lnum,pinst,&l->f->f2,NULL,scp);
}

static void
setup_ilem_finds(struct sig_context *scp, struct ilem_form *ip, 
		 struct sig const * const *sigs, int count)
{
  int i;

  if (!sigs)
    {
      ip->finds = NULL;
      ip->fcount = 0;
      return;
    }

  if (sigs && sigs[0] && sigs[0]->sig)
    {
      ip->fcount = count;
      ip->finds = malloc((1+count) * sizeof(struct ilem_form*));
      for (i = 0; i < count; ++i)
	{
	  ip->finds[i] = mb_new(scp->mb_ilem_forms);
	  ip->finds[i]->props = ip->props;
	  ip->finds[i]->f2.owner = ip->finds[i];
	  f2_parse(sigs[i]->set ? sigs[i]->set->file : (const unsigned char *)"cache", 
		   sigs[i]->lnum, 
		   npool_copy(sigs[i]->sig,scp->pool),
		   &ip->finds[i]->f2,NULL,scp);
	  ip->finds[i]->f2.sig = (Uchar*)sigs[i]->sig;
	  ip->finds[i]->literal = (char*)sigs[i]->literal;
	  ip->finds[i]->f2.rank = sigs[i]->rank;
	  ip->finds[i]->freq = sigs[i]->freq;
	  ip->finds[i]->pct = sigs[i]->pct;
	}
      ip->finds[i] = NULL;
    }
  else
    {
      ip->fcount = 0;
      ip->finds = NULL;
    }
}

static void
sigs_lookup_sub_sub(struct xcl_context *xcp, struct xcl_l *l, 
		    struct siglook *look, struct ilem_form *ifp)
{
  List *sigsets = NULL;
  const char *lem_lang = NULL;
  struct sigset *sp, *first_sp = NULL;
  char *projlang = NULL, *tmplang = NULL;
  int cache_find = 0;
  struct sig_find_state *best;
  static int nfinds = 0;
  struct sig const * const *sigs_found = NULL;
  struct sigset *sp_parent = NULL;
  int look_pass2 = 0;
  int free_flag = 0;
  
  extern int lem_autolem;
  extern int lem_dynalem;

  if (!ifp->f2.cf && !ifp->f2.norm && look->type == sig_look_check)
    return;

  lem_lang = (char*)ifp->f2.lang;

  if (tmplang)
    {
      free(tmplang);
      tmplang = NULL;
    }
  lem_lang = tmplang = tag_no_script((char*)lem_lang);
  projlang = malloc(strlen((char*)xcp->project)+strlen((char*)lem_lang)+2);
  sprintf(projlang,"%s%%%s",xcp->project,lem_lang);

  /* Load the list of sets from the config.xml but don't load any actual signatures yet */
  if (!(sigsets = hash_find(xcp->sigs->langs,(unsigned char *)projlang)))
    sigsets = sig_autoload_sets(xcp->sigs,xcp->project,(const char*)lem_lang);

  free(projlang);
  sigs_state_init();

  for (sp = list_first(sigsets); sp; sp = list_next(sigsets))
    {
      look_pass2 = ifp->fcount = nfinds = 0; /* must reset these each time */
#if 0
      /* Can't do this because sig_state references it */
      if (ifp->finds)
	{
	  free(ifp->finds);
	  ifp->finds = NULL;
	}
#endif
      sigs_found = NULL;

      if (ifp->f2.oform) /* A previous sigset tried aliasing */
	{
	  ifp->f2.form = ifp->f2.oform;
	  ifp->f2.oform = NULL;
	}

      /* This is where the actual lemm-xxx.sig sets get loaded */
      if (!sp->loaded)
	{
	  sig_load_set(xcp->sigs,sp);
	  if (ifp->lang && !ifp->lang->defsigs)
	    ifp->lang->defsigs = sp;
	}

      if (verbose)
	fprintf(stderr, "trying sig_set %s:%s\n", sp->project, sp->lang);
      
      if ((sigs_found = look_cache.test(xcp,ifp,sp,&nfinds)))
	{
	  /* if we are autolemming and we have finds we are done;
	     but remember that during autolemming we may also be
	     checking some of the previously lemmed finds. So we
	     need to detect if this form is an autolemmed instance
	     by looking at cf
	   */
	  sp_parent = sp;
	  sp = sp->cache;
	  /* F2_FLAGS_NO_FORM is left set on +-ed forms, but if we've retrieved it
	     from the cache by definition there must be a form */
	  BIT_SET(l->f->f2.flags, F2_FLAGS_FROM_CACHE);
	  BIT_CLEAR(l->f->f2.flags, F2_FLAGS_NO_FORM);
	  if (sigs_found && (lem_autolem || lem_dynalem) && !ifp->f2.cf)
	    {
	      setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
	      ifp->sp = sp;
	      ifp->look = &look_cache;
	      return;
	    }
	  first_sp = sp;
	  if (BIT_ISSET(sigs_found[0]->f2p->flags, F2_FLAGS_NOT_IN_SIGS))
	    BIT_SET(l->f->f2.flags, F2_FLAGS_NOT_IN_SIGS);
	}

      if (verbose)
	fprintf(stderr, "sigs_found after cache = %p\n", (void*) sigs_found);

#if 0      
      /* NO: this was a bad fix that you shouldn't make again,
	 Steve.  The code is designed to test the cache and
	 then exec the following if block when the entry is
	 not found in the cache */
      else
	sigs_found = look->test(xcp,ifp,sp,&nfinds);
#endif
    retry_after_cache:
      if (!sigs_found)
	{
	  BIT_CLEAR(ifp->f2.flags, F2_FLAGS_NO_FORM);
	  BIT_CLEAR(ifp->f2.flags, F2_FLAGS_PARTIAL);

	  if (look_pass2)
	    {
#if 0
	      if (sigs_found)
		{
		  free(sigs_found);
		  free_flag = 0;
		}
#endif
	      sigs_found = sigs_inst_in_sigset_pass2(xcp,ifp,sp,&nfinds);
	      free_flag = 1;
	    }
	  else
	    {
	      sigs_found = look->test(xcp,ifp,sp,&nfinds);
	      free_flag = 1;
	    }

	  /* fprintf(stderr, "sigs_found after %s:%s = %p\n", sp->project, sp->lang, (void*)sigs_found); */

	  /* if we are autolemming and we have finds we are done;
	     but remember that during autolemming we may also be
	     checking some of the previously lemmed finds. So we
	     need to detect if this form is an autolemmed instance
	     by looking at cf
	     
	     !!! FIXME !!! THIS DOES NOT WORK BECAUSE AKK-949 ALSO HAS CF=NULL 
	   */
	  if (sigs_found
	      && ((lem_autolem && (!ifp->f2.pos || !strcmp((const char *)ifp->f2.pos,"X"))) || lem_dynalem)
	      && !ifp->f2.cf)
	    {
	      setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
	      ifp->sp = sp;
	      ifp->look = look;
	      if (free_flag)
		{
		  free_flag = 0;
		  free((void*)sigs_found);
		}
	      return;
	    }

	  /* For Sum see if we can match on an alias */
	  if (/* sigs_found 
		 && BIT_ISSET(ifp->f2.flags,F2_FLAGS_NO_FORM)
		 && */
	      !strncmp((char*)ifp->f2.lang,"sux",3)
	      && (!sigs_found
		  || BIT_ISSET(ifp->f2.flags, F2_FLAGS_NO_FORM)))
	    {
	      if (nfinds)
		{
		  int i;
		  static int alias_nfinds;
		  int no_form = BIT_ISSET(ifp->f2.flags,F2_FLAGS_NO_FORM);
#if 0
		  already_tried_aliasing_init();
#endif
		  setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
		  /* can't free sigs_found here */
		  for (alias_nfinds = i = 0; ifp->finds[i]; ++i)
		    {
#if 0
		      /* This is broken atm because I reset form/oform for each sigset */
		      if (already_tried_aliasing((const char *)ifp->f2.form, &ifp->finds[i]->f2))
			continue;
#endif
		      if (f2_alias(xcp->sigs, &ifp->f2, &ifp->finds[i]->f2))
			{
			  struct sig const * const *alias_sigs_found = NULL;
			  BIT_CLR(ifp->f2.flags,F2_FLAGS_NO_FORM);
			  alias_sigs_found = look->test(xcp,ifp,sp,&alias_nfinds);
			  if (alias_nfinds)
			    {
			      free((void*)sigs_found);
			      sigs_found = alias_sigs_found;
			      nfinds = alias_nfinds;
			      no_form = 0;
			      break;
			    }
			  else
			    {
			      if (alias_sigs_found)
				free((void*)alias_sigs_found);
			    }
			}
		    }
#if 0
		  already_tried_aliasing_term();
#endif
		  if (no_form)
		    BIT_SET(ifp->f2.flags,F2_FLAGS_NO_FORM);
		  /* This just frees the temp setup we did for aliasing */
		  free(ifp->finds);
		  ifp->finds = NULL;
		  ifp->fcount = 0;
		}
	      else
		{
		  if (f2_alias(xcp->sigs, &ifp->f2, NULL))
		    {
		      struct sig const * const *alias_sigs_found = NULL;
		      static int alias_nfinds;
		      int no_form = BIT_ISSET(ifp->f2.flags,F2_FLAGS_NO_FORM);
		      BIT_CLR(ifp->f2.flags,F2_FLAGS_NO_FORM);
		      alias_nfinds = 0;
		      alias_sigs_found = look->test(xcp,ifp,sp,&alias_nfinds);
		      if (alias_nfinds)
			{
			  if (sigs_found)
			    {
			      free((void*)sigs_found);
			      free_flag = 0;
			    }
			  sigs_found = alias_sigs_found;
			  nfinds = alias_nfinds;
			  no_form = 0;
			}
		      else
			{
			  if (alias_sigs_found)
			    free((void*)alias_sigs_found);
			  if (no_form)
			    BIT_SET(ifp->f2.flags,F2_FLAGS_NO_FORM);
			}
		    }
		}
	    }

	  if (sigs_found 
	      && BIT_ISSET(ifp->f2.flags,F2_FLAGS_NO_FORM)
	      && fuzzy_aliasing)
	    {
	      int i;
	      static int alias_nfinds;
	      int no_form = BIT_ISSET(ifp->f2.flags,F2_FLAGS_NO_FORM);
	      setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
	      /* can't free sigs_found here */
	      for (alias_nfinds = i = 0; ifp->finds[i]; ++i)
		{
		  if (f2_extreme_alias(xcp->sigs, &ifp->f2, &ifp->finds[i]->f2))
		    {
		      struct sig const * const *alias_sigs_found = NULL;
		      BIT_CLR(ifp->f2.flags,F2_FLAGS_NO_FORM);
		      alias_sigs_found = look->test(xcp,ifp,sp,&alias_nfinds);
		      if (alias_nfinds)
			{
			  free((void*)sigs_found);
			  sigs_found = alias_sigs_found;
			  nfinds = alias_nfinds;
			  no_form = 0;
			  break;
			}
		      else
			{
			  if (alias_sigs_found)
			    free((void*)alias_sigs_found);
			}
		    }
		}
	      if (no_form)
		BIT_SET(ifp->f2.flags,F2_FLAGS_NO_FORM);
	      /* This just frees the temp setup we did for aliasing */
	      free(ifp->finds);
	      ifp->finds = NULL;
	      ifp->fcount = 0;
	      
	    }
	}

      if (nfinds > 1 && (lem_autolem || multi_sub)) /* multi_sub is a hack that will go away when PSUs match SENSE as well as GW */
	{
	  nfinds = 1;
	}
      
      if (nfinds)
	{
	  setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
#if 0
	  if (free_flag)
	    {
	      free((void*)sigs_found);
	      free_flag = 0;
	    }
#endif
	}
      
      if (nfinds > 1 && has_perfect_match(ifp->finds, nfinds))
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)wordset_pct_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}

      /* 2020-01-03: not clear that this reduction of senses is
	 correct; probably better to keep all the senses and then use
	 statistics to select most common sense */
      if ((!lem_dynalem && !lem_autolem)
	  && (nfinds > 1 && !ifp->f2.pos && !ifp->f2.epos))
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, NULL/*test does not care about fp*/, NULL, 
			    (select_func*)default_pos_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}

      if (nfinds && lem_autolem && ifp->f2.pos)
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)exact_pos_test, NULL, &tmp_nfinds);
	  /* This one is allowed to produce 0 results so we ignore N matches to '; PN' in autolem */
	  if (tmp_nfinds < nfinds)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}
      
      if (nfinds > 1 && ifp->f2.pos)
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)implicit_epos_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	  if (lem_autolem && nfinds > 1)
	    {
	      fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
				(select_func*)select_gw_zero, NULL, &tmp_nfinds);
	      if (tmp_nfinds)
		{
		  tmp_nfinds = 1;
		  memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
		  ifp->fcount = nfinds = tmp_nfinds;
		}
	      else
		ifp->fcount = nfinds = 1; /* Just use the first result */
	    }
	}

      if (BIT_ISSET(ifp->f2.flags,F2_FLAGS_PARTIAL) || (nfinds > 1 && !ifp->f2.sense))
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  int *saved_pct = NULL, i, top_saved = 0;
	  
	  saved_pct = malloc(nfinds * sizeof(int));
	  for (i = 0; i < nfinds; ++i)
	    {
	      if (ifp->finds[i]->pct > top_saved)
		top_saved = ifp->finds[i]->pct;
	      saved_pct[i] = ifp->finds[i]->pct;
	    }

	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)rematch_on_sense, NULL, &tmp_nfinds);
	  if (tmp_nfinds == 0)
	    {
#if 1
	      if (top_saved)
		{
		  for (i = 0; i < nfinds; ++i)
		    ifp->finds[i]->pct = saved_pct[i];
		}
	      else
		{
		  /* If there's no match on GW/SENSE we can't assume any of the CF matches is good */
		  /* We check words->pct because that gets set when gw_wild = 1 and we have a GW match */
		  ifp->fcount = nfinds = 0;
		  if (ifp->finds)
		    {
		      free(ifp->finds);
		      ifp->finds = NULL;
		    }
		  free((void*)sigs_found);
		  sigs_found = NULL;
		  BIT_CLEAR(ifp->f2.flags,F2_FLAGS_PARTIAL);
		  if (look_pass2 == 0)
		    {
		      look_pass2 = 1;
		      goto retry_after_cache;
		    }
		}
#endif
	    }
	  else if (tmp_nfinds && tmp_nfinds < nfinds)
	    {
	      int i;
	      for (i = 0; i < tmp_nfinds; ++i)
		if (f2_test_no_sense(&ifp->f2, &fpp[i]->f2))
		  BIT_CLEAR(fpp[i]->f2.flags, F2_FLAGS_PARTIAL);
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}

      if (nfinds > 1)
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)wordset_pct_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}

      if (nfinds > 1)
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)implicit_norm_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}

#if 0 /* lang == qpn can't happen any more */
      if (nfinds > 1 && !strcmp((char*)sp->lang, "qpn"))
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)context_lang_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }
	}
#endif

      if (nfinds > 1 && BIT_ISSET(ifp->f2.flags, F2_FLAGS_LEM_BY_NORM))
	{
	  int i;
	  const char *disambig = NULL;

	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)wild_form_test, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
	    {
	      memcpy(ifp->finds,fpp,(1+tmp_nfinds)*sizeof(struct ilem_form *));
	      ifp->fcount = nfinds = tmp_nfinds;
	    }

	  if (nfinds > 1)
	    {
	      /* uniq by form, keeping only the form which matches the look-up lem;
	       * need to check entire sig for identity because one day Akk will have
	       * M1 and M2 to consider, and that will require disambiguating the 
	       * normalized lemming just as we do the form-based lemming
	       */
	      disambig = strrchr((char *)ifp->f2.form, '/');
	      for (i = 0; i < nfinds; ++i)
		{
		  const char *find_disambig = strrchr((char*)ifp->finds[i]->f2.form,'/');
		  if (((find_disambig && disambig && !strcmp(disambig, find_disambig))
		       || (!find_disambig && !disambig))
		      && f2_test(&ifp->f2, &ifp->finds[i]->f2))
		    {
		      if (i)
			ifp->finds[0] = ifp->finds[i];
		      ifp->finds[1] = NULL;
		      nfinds = 1;
		      break;
		    }
		}
	    }
	}

      if (!first_sp)
	first_sp = sp;

      if (nfinds && strcmp((const char *)sp->file, "cache"))
	{
	  sigs_state_save(sp, ifp, sigs_found, nfinds);
	  if (verbose)
	    fprintf(stderr, "saving %s\n", sigs_found[0]->sig);
	}

#if 1
      if (nfinds == 1  
	  && !BIT_ISSET(ifp->f2.flags, F2_FLAGS_PARTIAL)
	  && !BIT_ISSET(ifp->f2.flags, F2_FLAGS_NO_FORM)
	  )
	{
	  if (verbose)
	    fprintf(stderr, "breaking with nfinds=1 and not PARTIAL or NO_FORM\n");
	  break;
	}
      else if (nfinds > 1 && !strcmp((const char *)sp->file, "cache"))
	{
	  sp = sp_parent;
	  if (sigs_found)
	    free((void*)sigs_found);
	  sigs_found = NULL;
	  nfinds = 0;	  
	  BIT_CLR(l->f->f2.flags, F2_FLAGS_FROM_CACHE);
	  goto retry_after_cache;
	}
      else
	{
	  if (verbose)
	    {
	      fprintf(stderr, "PARTIAL = %d; NO_FORM=%d\n",
		      BIT_ISSET(ifp->f2.flags, F2_FLAGS_PARTIAL),
		      BIT_ISSET(ifp->f2.flags, F2_FLAGS_NO_FORM));
	    }
	}
#else
      if (nfinds == 1  || !BIT_ISSET(ifp->f2.flags, F2_FLAGS_NO_FORM))
	break;
#endif
    }

  if (sp && !strcmp((const char *)sp->file, "cache"))
    {
      ifp->fcount = sigs_found[0]->ifp->fcount;
      if (!ifp->f2.norm
	  || !ifp->f2.cf
	  || (!strcmp((char *)ifp->f2.cf, (char *)ifp->f2.norm) 
	      && strcmp((char *)ifp->f2.cf, (char *)sigs_found[0]->ifp->f2.cf)))
	{
	  if (ifp->f2.cf)
	    ifp->f2.norm = ifp->f2.cf;
	  ifp->f2.cf = sigs_found[0]->ifp->f2.cf;
	}
      if (!ifp->f2.sense
	  || (!strcmp((char*)ifp->f2.gw, (char*)ifp->f2.sense) 
	      && strcmp((char*)ifp->f2.gw, (char*)sigs_found[0]->ifp->f2.gw)))
	{
	  ifp->f2.sense = ifp->f2.gw;
	  ifp->f2.gw = sigs_found[0]->ifp->f2.gw;
	}
      ifp->f2.sig = sigs_found[0]->ifp->f2.sig;
      if (sigs_found[0]->ifp->f2.pos && !ifp->f2.pos)
	ifp->f2.pos = sigs_found[0]->ifp->f2.pos;
      if (sigs_found[0]->ifp->f2.epos && !ifp->f2.epos)
	ifp->f2.epos = sigs_found[0]->ifp->f2.epos;
      if (sigs_found[0]->ifp->f2.norm && !ifp->f2.norm)
	ifp->f2.norm = sigs_found[0]->ifp->f2.norm;
      if (sigs_found[0]->ifp->f2.base && !ifp->f2.base)
	ifp->f2.morph = sigs_found[0]->ifp->f2.base;
      if (sigs_found[0]->ifp->f2.cont && !ifp->f2.cont)
	ifp->f2.morph = sigs_found[0]->ifp->f2.cont;
      if (sigs_found[0]->ifp->f2.morph && !ifp->f2.morph)
	ifp->f2.morph = sigs_found[0]->ifp->f2.morph;
      if (sigs_found[0]->ifp->f2.morph2 && !ifp->f2.morph2)
	ifp->f2.morph2 = sigs_found[0]->ifp->f2.morph2;
      ifp->finds = malloc(ifp->fcount * sizeof(struct ilem_form *));
      memcpy(ifp->finds, sigs_found[0]->ifp->finds, ifp->fcount * sizeof(struct ilem_form *));
      sp = sigs_found[0]->set;
      BIT_CLEAR(ifp->f2.flags, F2_FLAGS_NO_FORM);
      BIT_CLEAR(ifp->f2.flags, F2_FLAGS_PARTIAL);
      if (BIT_ISSET(sigs_found[0]->ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS))
	BIT_SET(ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS);
      if (BIT_ISSET(sigs_found[0]->ifp->f2.flags, F2_FLAGS_LEM_NEW))
	BIT_SET(ifp->f2.flags, F2_FLAGS_LEM_NEW);
    }
  else
    {
      if ((best = sigs_state_best()))
	{
	  sp = best->sp;
	  ifp = best->fp;
	  if (ifp->finds != best->finds)
	    {
	      if (ifp->finds)
		free(ifp->finds);
	      ifp->finds = best->finds;
	    }
	  /* this needs to be set regardless of whether ifp->finds == best->finds */
	  ifp->fcount = nfinds = best->nfinds;
	  best->best = 1;
	  if (best->fp_aliased_form)
	    {
	      ifp->f2.oform = ifp->f2.form;
	      ifp->f2.form = best->fp_aliased_form;
	    }
	  
	  /* sigs_found = best->sigs_found; */
	  
	  /* Suppress NO_FORM on +-ed lems so they get output as 'newsig'
	     and not 'bad' */
	  /* No: this is the wrong way to do this--we have to let NO_FORM
	     stay set because it is used by the extended lemming, and
	     just be sure to test in x2_serialize that we are outputting
	     a +-ed lem as newsig and not bad */
	  if (BIT_ISSET(ifp->f2.flags, F2_FLAGS_LEM_NEW) && best->no_form)
	    {
	      BIT_SET(ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS);
	      /*BIT_CLEAR(ifp->f2.flags, F2_FLAGS_NO_FORM);*/
	    }
	  else if (best->no_form)
	    BIT_SET(ifp->f2.flags, F2_FLAGS_NO_FORM);
	  else
	    BIT_CLEAR(ifp->f2.flags, F2_FLAGS_NO_FORM);
	  
	  if (BIT_ISSET(ifp->f2.flags, F2_FLAGS_LEM_NEW) && best->partial)
	    {
	      BIT_SET(ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS);
	      BIT_CLEAR(ifp->f2.flags, F2_FLAGS_PARTIAL);
	    }
	  else if (best->partial == F2_FLAGS_PARTIAL)  /* best->partial can == F2_FLAGS_NO_FORM */
	    BIT_SET(ifp->f2.flags, F2_FLAGS_PARTIAL);
	  else
	    BIT_CLEAR(ifp->f2.flags, F2_FLAGS_PARTIAL);

#if 0 /* can't happen any more */
	  if (!strcmp(sp->file, "cache"))
	    BIT_SET(l->f->f2.flags, F2_FLAGS_FROM_CACHE);
#endif
	}
      else
	sp = list_first(sigsets);
    }

  sigs_state_free();

  if (ifp->fcount >= 1)
    {
      l->f->sp = sp;
      l->f->look = look;
      l->user = sp->psus;
    }
  else
    {
      extern int lem_autolem;
      l->f->sp = sp;
      l->f->look = look;

      if ((lem_autolem || BIT_ISSET(ifp->f2.flags, F2_FLAGS_LEM_NEW)) && !cache_find) /* lem_dynalem not needed */
	{
	  if (!ifp->sp)
	    {
	      List *autosigs = sig_autoload_sets(xcp->sigs,xcp->project,(const char*)lem_lang);
	      ifp->sp = list_first(autosigs);
	    }
	  BIT_SET(ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS);
	}
    }
  if (free_flag)
    free((void*)sigs_found);
}

static void
sigs_lookup_sub(struct xcl_context *xcp, struct xcl_l *l, struct siglook *look, struct ilem_form *ifp)
{
  if (ifp->multi)
    {
      /* Check the COF tails first */
      struct ilem_form *mp;
#if 0
      if (!ifp->f2.parts)
	ifp->f2.parts = malloc(ifp->mcount * sizeof(struct f2*));
#endif
      multi_sub = 1;
      for (mp = ifp->multi; mp; mp = mp->multi)
	{
	  sigs_lookup_sub_sub(xcp, l, look, mp);
	  if (mp->fcount == 0)
	    {
	      BIT_SET(mp->f2.flags, F2_FLAGS_COF_INVALID);
	      BIT_SET(ifp->f2.flags, F2_FLAGS_COF_INVALID);
	    }
	}
    }
  /* Now check the COF head */
  sigs_lookup_sub_sub(xcp, l, look, ifp);
  multi_sub = 0;
}

void
sigs_lookup(struct xcl_context *xcp, struct xcl_l *l, struct siglook *look)
{
  if (!l->inst || !*l->inst)
    return;

  if (!l->f)
    setup_ilem_form(xcp->sigs,l,npool_copy((Uchar *)l->inst,xcp->pool));

  if (l->f->ambig)
    {
      struct ilem_form *ifp = l->f;
      do
	{
	  sigs_lookup_sub(xcp, l, look, ifp);
	  ifp = ifp->ambig;
	}
      while (ifp);
    }
  else
    {
      if (!BIT_ISSET(l->f->f2.flags, F2_FLAGS_COF_TAIL))
	sigs_lookup_sub(xcp, l, look, l->f);
    }
}

#if 0
static struct npool *already_aliased_pool = NULL;
static Hash_table *already_aliased = NULL;

static void
already_tried_aliasing_init(void)
{
  already_tried_aliasing_term();
  already_aliased = hash_create(1023);
  already_aliased_pool = npool_init();
}

static int
already_tried_aliasing(const char *form, struct f2 *f2)
{
  unsigned char buf[1024];
  int ret = 0;

  if (!form || !f2 || !f2->cf || !f2->gw || !f2->pos)
    return 0;

  sprintf((char *)buf, "%s=%s[%s]%s", form, f2->cf, f2->gw, f2->pos);

  if (!(ret = (uintptr_t)hash_find(already_aliased, buf)))
    hash_add(already_aliased, npool_copy(buf, already_aliased_pool), "1");

  return ret;
}

static void
already_tried_aliasing_term(void)
{
  if (already_aliased)
    {
      hash_free(already_aliased, NULL);
      already_aliased = NULL;
    }
  if (already_aliased_pool)
    {
      npool_term(already_aliased_pool);
      already_aliased_pool = NULL;
    }
}
#endif

#include <stdio.h>
#include <stdlib.h>
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

static void already_tried_aliasing_init(void);
static void already_tried_aliasing_term(void);
static int already_tried_aliasing(const char *form, struct f2 *f2);

/* this must return 0 if the ref_fp->pos == fp->pos */
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

/* this must return 1 if the ref_fp->gw matches fp->sense */
static int 
rematch_on_sense(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  int ret = sense_ok(&fp->f2, &ref_fp->f2, 0);
  return !ret;
#if 0
  if (ref_fp->f2.cf && fp->f2.norm)
    return strcmp((char*)ref_fp->f2.cf,(char*)fp->f2.norm);
  else
    return 1;
#endif
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

/* this must return 0 if the ref_fp->norm == fp->cf */
static int 
context_lang_test(struct ilem_form *fp, struct ilem_form *ref_fp, void *setup)
{
  if (ref_fp->f2.core && fp->f2.core 
      && ref_fp->f2.core->name && fp->f2.core->name)
    return strcmp((char*)ref_fp->f2.core->name,(char*)fp->f2.core->name);
  else
    return 1;
}

struct sig_find_state
{
  struct sigset *sp;
  struct ilem_form *fp;
  struct ilem_form **finds;
  int nfinds;
  struct sig const * const *sigs_found; 
  int no_form;
  int partial;
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

/* FIXME: this causes memory leaks if finds that are
   not later used are not freed during sigs_state_free */
static void
sigs_state_free(void)
{
  if (sig_state)
    {
      list_free(sig_state,list_xfree);
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
  s->finds = fp->finds;
  s->sigs_found = sigs_found;
  s->nfinds = nfinds;
  s->no_form = (BIT_ISSET(fp->f2.flags, F2_FLAGS_NO_FORM) ? F2_FLAGS_NO_FORM : 0);
  s->partial = (BIT_ISSET(fp->f2.flags, F2_FLAGS_PARTIAL) ? F2_FLAGS_PARTIAL : s->no_form);
  list_add(sig_state, s);
}

void
setup_ilem_form(struct sig_context *scp, struct xcl_l *l, unsigned char*pinst)
{
  l->f = mb_new(scp->mb_ilem_forms);
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
	  f2_parse(sigs[i]->set ? sigs[i]->set->file : (const unsigned char *)"cache", 
		   sigs[i]->lnum, 
		   npool_copy(sigs[i]->sig,scp->pool),
		   &ip->finds[i]->f2,NULL,scp);
	  ip->finds[i]->f2.sig = (Uchar*)sigs[i]->sig;
	  ip->finds[i]->literal = (char*)sigs[i]->literal;
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

  extern int lem_autolem;

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

  if (!(sigsets = hash_find(xcp->sigs->langs,(unsigned char *)projlang)))
    sigsets = sig_autoload_sets(xcp->sigs,xcp->project,(const char*)lem_lang);

  free(projlang);
  sigs_state_init();

  for (sp = list_first(sigsets); sp; sp = list_next(sigsets))
    {
      ifp->fcount = nfinds = 0; /* must reset these each time */
      ifp->finds = NULL;
      sigs_found = NULL;
    
      if (!sp->loaded)
	sig_load_set(xcp->sigs,sp);

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
	  if (sigs_found && lem_autolem && !ifp->f2.cf)
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
	  sigs_found = look->test(xcp,ifp,sp,&nfinds);

	  /* if we are autolemming and we have finds we are done;
	     but remember that during autolemming we may also be
	     checking some of the previously lemmed finds. So we
	     need to detect if this form is an autolemmed instance
	     by looking at cf
	   */
	  if (sigs_found && lem_autolem && !ifp->f2.cf)
	    {
	      setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
	      ifp->sp = sp;
	      ifp->look = look;
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
		  already_tried_aliasing_init();
		  setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);
		  for (alias_nfinds = i = 0; ifp->finds[i]; ++i)
		    {
		      if (already_tried_aliasing((const char *)ifp->f2.form, &ifp->finds[i]->f2))
			continue;
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
		  already_tried_aliasing_term();
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
			  free((void*)sigs_found);
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

      if (nfinds)
	setup_ilem_finds(xcp->sigs, ifp, sigs_found, nfinds);

      if (nfinds > 1 && !ifp->f2.pos && !ifp->f2.epos)
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
	}

      if (nfinds > 1 && !ifp->f2.sense)
	{
	  int tmp_nfinds = 0;
	  struct ilem_form **fpp;
	  fpp = ilem_select(ifp->finds, nfinds, ifp, NULL, 
			    (select_func*)rematch_on_sense, NULL, &tmp_nfinds);
	  if (tmp_nfinds < nfinds && tmp_nfinds > 0)
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

      if (!first_sp)
	first_sp = sp;

      if (nfinds && strcmp((const char *)sp->file, "cache"))
	sigs_state_save(sp, ifp, sigs_found, nfinds);

#if 1
      if (nfinds == 1  
	  && !BIT_ISSET(ifp->f2.flags, F2_FLAGS_PARTIAL)
	  && !BIT_ISSET(ifp->f2.flags, F2_FLAGS_NO_FORM)
	  )
	break;
      else if (nfinds > 1 && !strcmp((const char *)sp->file, "cache"))
	{
	  sp = sp_parent;
	  sigs_found = NULL;
	  nfinds = 0;
	  
	  goto retry_after_cache;
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
	  ifp->finds = best->finds;
	  ifp->fcount = nfinds = best->nfinds;
	  
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

      if ((lem_autolem || BIT_ISSET(ifp->f2.flags, F2_FLAGS_LEM_NEW)) && !cache_find)
	{
	  if (!ifp->sp)
	    {
	      List *autosigs = sig_autoload_sets(xcp->sigs,xcp->project,(const char*)lem_lang);
	      ifp->sp = list_first(autosigs);
	    }
	  BIT_SET(ifp->f2.flags, F2_FLAGS_NOT_IN_SIGS);
	}
    }

  free(tmplang);
}

static void
sigs_lookup_sub(struct xcl_context *xcp, struct xcl_l *l, struct siglook *look, struct ilem_form *ifp)
{
  if (ifp->multi)
    {
      struct ilem_form *mp;
#if 0
      if (!ifp->f2.parts)
	ifp->f2.parts = malloc(ifp->mcount * sizeof(struct f2*));
#endif
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
  sigs_lookup_sub_sub(xcp, l, look, ifp);
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

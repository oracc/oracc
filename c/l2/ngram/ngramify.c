#include <stddef.h>
#include <stdlib.h>
#include "warning.h"
#include "ilem_form.h"
#include "xcl.h"
#include "ngram.h"
#include "links.h"
#include "lang.h"
#include "sigs.h"
#include "props.h"

int ng_debug = 0;
static int ng_match_logging = 1;
int ngramify_per_lang = 0;
int ngramify_disambiguating = 0;

int ngram_obey_lines = 0;

static FILE *ng_match_log = NULL;

static List *ngramify_mem = NULL;

extern int verbose;
static int ngram_id = -1;
static struct ML *match_list = NULL;
static int wild_reset;

static void add_match(struct xcl_l *matches, struct f2 **fmatches, 
		      struct CF*tt, const char *psu, struct f2 *psu_form,
		      void *user, int nmatches);
static void add_wild_match(struct xcl_l *lp);
static int check_predicates(struct f2 *p, struct CF *cfp);
static struct f2 **match(struct CF *step, struct xcl_l *lp, int *nmatchesp, struct prop *p);
static int match_nle(struct NLE *nle, union xcl_u *cl, int cl_index, int cl_max);
static struct match *next_match(void);
static struct NLE **nle_heads(struct NL*nlp, struct ilem_form *fp, int *n_nodes);
static int nle_cmp(struct NLE **a, struct NLE **b);
static int try_match(int match_index, 
		     struct CF**steps, int max_steps, int step_index,
		     union xcl_u*cls, int cl_index, int cl_max,
		     struct CF **tts, const char *psu, struct f2 *psu_form,
		     struct prop *props);
static struct f2 **lnodes_of(struct ilem_form *ifp, int *nparsesp);
static int psu_cofs_ok(void);

int
get_ngram_id(void)
{
  return ngram_id;
}

void
ngdebug(const char *mess,...)
{
  if (ng_debug)
    {
      va_list ap;
      va_start(ap,mess);
      fputs("[ng]: ",f_log);
      vfprintf(f_log,mess,ap);
      fputc('\n',f_log);
      va_end(ap);
    }
}

void
ngramify_init()
{
  if (ng_match_logging && !ng_match_log)
    {
      ng_match_log = fopen("01tmp/ng_match.log", "wb");
      if (NULL == ng_match_log)
	{
	  fprintf(stderr, "ngramify_init: couldn't open 01tmp/ng_match.log--no ngram match logging will be done.\n");
	  ng_match_logging = 0;
	}
    }
  if (!match_list)
    {
      match_list = calloc(1,sizeof(struct ML));
      match_list->matches = NULL;
    }
  else
    ngramify_reset();
  if (!ngramify_mem)
    ngramify_mem = list_create(LIST_SINGLE);
}
void
ngramify_reset()
{
  match_list->matches_used = 0;
#if 0
  list_free(ngramify_mem,free);
  ngramify_mem = list_create(LIST_SINGLE);
#endif
}

void
ngramify_term()
{
  if (match_list)
    {
      free(match_list->matches);
      free(match_list);
      match_list = NULL;
    }
  if (ng_match_log)
    fclose(ng_match_log);
  if (ngramify_mem)
    {
      list_free(ngramify_mem, free);
      ngramify_mem = NULL;
    }
}

static const char *
action_name(enum nlcp_actions act)
{
  switch (act)
    {      
    case nlcp_action_rewrite:
      return "rewrite";
    case nlcp_action_props:
      return "props";
    case nlcp_action_psu:
      return "psu";
    case nlcp_action_psa:
      return "psa";
    }
  return "<unknown action>";
}

void
ngramify(struct xcl_context *xcp, struct xcl_c*cp)
{
  int i;
  struct NL_context *nlcp = xcp->user;
  union xcl_u*clnodes = cp->children;
  int nclnodes = cp->nchildren;
  const char *user_name = NULL;
  
  ++ngram_id;
  ngdebug("[ngramify@%d] start processing %d XCL children",ngram_id,nclnodes);
  /* we have to have at least two nodes left in the list to be able to make an ngram match */

  /* NOT TRUE: if we are attaching properties these may be singletons */
  /*  for (i = 0; i < (nclnodes - 1); ++i) */
  for (i = 0; i < nclnodes; ++i)
    {
      int i_nle,n_nle;
      struct NLE**nle;

      /* We don't need to iterate into xcl_node_c entries because
	 xcl_map is going to do that for us */
      if (clnodes[i].c->node_type != xcl_node_l)
	continue;

      if (ngramify_per_lang)
	{
	  if (clnodes[i].l->f)
	    {
	      if (!clnodes[i].l->f->sp /*&& (clnodes[i].l->f->sp || clnodes[i].l->f->f2.pos))*/
		  && (!clnodes[i].l->f || !clnodes[i].l->f->lang
		      || !(clnodes[i].l->f->sp = clnodes[i].l->f->lang->defsigs)))
		continue; /* silently ignore l nodes whose lang can't be associated with a sigset */
	      
	      /* This should switch for bigrams as well */
	      if (xcp->user)
		{
		  if ((int)(uintptr_t)xcp->user == NGRAMIFY_USE_BIGRAMS)
		    {
		      nlcp = clnodes[i].l->f->sp->bigrams;
		      ngdebug("context=bigrams");
		      user_name = "bigrams";
		    }
		  else if ((int)(uintptr_t)xcp->user == NGRAMIFY_USE_COLLOS)
		    {
		      nlcp = clnodes[i].l->f->sp->collos;
		      ngdebug("context=collos");
		      user_name = "collos";
		    }
		  else
		    {
		      enum langcode c = c_none;
		      /*const char *lang = NULL;*/
		      if (clnodes[i].l->f->lang)
			{
			  c = clnodes[i].l->f->lang->core->code;
			  /*lang = clnodes[i].l->f->lang->core->name;*/
			}
		      else
			{
			  char base[4], *b = base;
			  const char *tmp;
			  if ((tmp = (const char*)clnodes[i].l->f->f2.lang))
			    {
			      struct langcore *lcp = NULL;
			      /*lang = (const char*)clnodes[i].l->f->f2.lang;*/
			      while (*tmp && '-' != *tmp)
				*b++ = *tmp++;
			      *b = '\0';
			      if ((lcp = lang_core(base,3)))
				c = lcp->code;
			    }
			}
		      if (c != c_none)
			{
			  nlcp = ((struct NL_context **)xcp->user)[c];
			  ngdebug("context=psus for lang %s", clnodes[i].l->f->lang);
			  user_name = "psus";
			}
		      else
			{
			  nlcp = NULL;
			  ngdebug("context=null");
			  user_name = "null";
			}
		    }
		}
	      else
		{
		  nlcp = clnodes[i].l->f->sp->psus;
		}
	      if (!nlcp)
		continue;
	    }
	  else
	    continue;
	}

      if (!nlcp || !(nle = nle_heads(nlcp->nlp,clnodes[i].l->f,&n_nle)))
	continue;

      /*match_list = NULL;*/
      wild_reset = -1;
      for (i_nle = 0; i_nle < n_nle; ++i_nle)
	{
	  ngramify_reset();
	  ngdebug("[ngramify] testing %s:NLE#%d: %s",
		  nle[i_nle]->owner->name,
		  nle[i_nle]->priority,
		  nle[i_nle]->line);
	  if (match_nle(nle[i_nle], clnodes, i, nclnodes))
	    {
	      int first_non_d;
	      ngdebug("[ngramify] NLE#%d match triggers %s:%d; user=%s; action=%s",
		      nle[i_nle]->priority,
		      nle[i_nle]->file,
		      nle[i_nle]->lnum,
		      user_name,
		      action_name(nlcp->action));

	      if (clnodes)
		{
		  /* i is the index into the clnodes where we started this successful match_nle */
		  for (first_non_d = i;
		       first_non_d < nclnodes 
			 && clnodes[first_non_d].c
			 && clnodes[first_non_d].c->node_type != xcl_node_l;
		       ++first_non_d)
		    ;
		  if (first_non_d < nclnodes)
		    {
		      if (ng_match_logging)
			{
			  struct ilem_form *ip = match_list->matches->lp->f;
			  fprintf(ng_match_log, "%s\t%s:%d\t%s:%d\t/%s/%s\t%s\n",
				  nlcp->nlp->name,
				  nle[i_nle]->file, nle[i_nle]->lnum, 
				  ip->file, (int)ip->lnum,
				  clnodes[first_non_d].l->parent->xc->project, clnodes[first_non_d].l->ref,
				  nle[i_nle]->line
				  );
			}
		      match_list->matches->user = nle[i_nle]->user;
		      if (match_list->matches->psu_form)
			if (clnodes[first_non_d].l->f->newflag)
			  BIT_SET(match_list->matches->psu_form->flags, F2_FLAGS_LEM_NEW);
		      nlcp->func(xcp,match_list);
		    }
		}
	      
	      /* After a successful match we skip all of the
		 lnodes that were used in the match; in PV there
		 was a variable, $single_match, which in principle
		 could allow lnodes in a match to be processed more
		 than once, but it was intialized to 1 and never
		 changed, so that is not implemented in CV.  In the
		 new implementation the same effect can be achieved by
		 having multiple NLCP's and iterating over the XCL
		 tree for each NLCP
	       */
	      i += match_list->matches_used - 1;
	      break;
	    }
	  else
	    {
	      ngdebug("[ngramify] NLE#%d test failed",
		      nle[i_nle]->priority);
	    }
	}
    }
  ngdebug("[ngramify] returning");
}


/* should really rewrite try_match signature to pass nle instead of
   all these arguments taken from nle */
static int
match_nle(struct NLE *nle, union xcl_u *cl, int cl_index, int cl_max)
{
  return try_match(0, 
		   nle->cfs, nle->ncfs, 0,
		   cl, cl_index, cl_max,
		   nle->tts,
		   nle->psu, nle->psu_form,
		   nle->props)
    && psu_cofs_ok();
}


/*

  NEED TO IMPLEMENT COF_ID SO THAT THIS ROUTINE CAN CHECK MATCH AND COF_ID
  TO ENSURE THAT WE DO NOT CROSS-MATCH COF HEAD/TAIL FROM DIFFERENT COFS

 */

/* For any lp that is in the PSU match, if it is a COF head,
   check that all of its COF tails have F2_FLAGS_NGRAM_MATCH
   set */
static int
psu_cofs_ok(void)
{
  int i;
  return 1;
#if 0
  struct match *new_mp = malloc(match_list->matches_used * sizeof(struct match));
  int new_mp_len = 0;
#endif
  /* matches_used[0] = maru ; matches_used[1] = shipru */
  for (i = 0; i < match_list->matches_used; ++i)
    {
#if 0
      int mp_keeper = 1;
#endif
      struct match *mp = &match_list->matches[i];
      if (mp->lp->cof_tails)
	{
	  int j;
	  struct f2 **new_f2s = malloc((1+mp->nmatches) * sizeof(struct f2*));
	  int n_new_f2s = 0;
	  /* matching_f2s = maru/maru/maru/maru */
	  for (j = 0; mp->matching_f2s[j] && mp->matching_f2s[j]->parts; ++j)
	    {
	      int keeper_f2 = 1, k;
	      /* parts = shipru */
	      for (k = 0; mp->matching_f2s[j]->parts[k]; ++k)
		{
		  if (!BIT_ISSET(mp->matching_f2s[j]->parts[k]->flags, F2_FLAGS_NGRAM_MATCH))
		    {
		      keeper_f2 = 0;
		      break;
		    }
		  else
		    {
		      BIT_SET(mp->matching_f2s[j]->parts[k]->flags, F2_FLAGS_NGRAM_KEEP);
		    }
		}
	      if (keeper_f2)
		new_f2s[n_new_f2s++] = mp->matching_f2s[j];
	    }
	  /* Now, if we selected some of the COF HEADs we must trim the matches_used 
	     entries to remove F2s which are not part of the match, and we can skip 
	     further COF-HEAD processing of those.
	     
	     If we didn't find any keepers, this entire PSU match is dead.
	   */
	  if (n_new_f2s)
	    {
	      int k;
	      memcpy(mp->matching_f2s, new_f2s, n_new_f2s * sizeof(struct f2 *));
	      mp->matching_f2s[n_new_f2s] = NULL;
	      mp->nmatches = n_new_f2s;
	      free(new_f2s);

	      /* Trim the matches_used which must be TAILs to retain only
		 F2s flagged as keepers */
	      for (k = i+1; k < match_list->matches_used; ++k)
		{
		  struct match *mp = &match_list->matches[k];
		  struct f2 **f2_keepers = malloc((1+mp->nmatches) * sizeof(struct f2 *));
		  int n_keepers = 0;
		  for (j = 0; mp->matching_f2s[j]; ++j)
		    {
		      if (BIT_ISSET(mp->matching_f2s[j]->flags, F2_FLAGS_NGRAM_KEEP))
			f2_keepers[n_keepers++] = mp->matching_f2s[j];
		    }
		  memcpy(mp->matching_f2s, f2_keepers, n_keepers * sizeof(struct f2*));
		  mp->matching_f2s[n_keepers] = NULL;
		  mp->nmatches = n_keepers;
		  free(f2_keepers);
		}
	      i = k;
	    }
	  else
	    {
	      free(new_f2s);
	      ngdebug("[psu_cofs_ok] returning false");
	      return 0;
	    }
	}
    }
  ngdebug("[psu_cofs_ok] returning true");
  return 1;
}

static int
is_ignorable(union xcl_u *u)
{
  /* for now simply ignore all discontinuities */
  if (u->c->node_type == xcl_node_d)
    {
      return 1;
    }
  if (u->c->node_type == xcl_node_l)
    {
      if (u->l->f && BIT_ISSET(u->l->f->instance_flags,F2_FLAGS_PSU_SKIP))
	{
	  return 1;
	}
    }
  return 0;
}

static int
try_match(int match_index, 
	  struct CF**steps, int max_steps, int step_index,
	  union xcl_u *cls, int cl_index, int cl_max,
	  struct CF **tts, const char* psu, struct f2 *psu_form,
	  struct prop *p)
{
  int wild_tries = 0;

  if (step_index >= max_steps)
    return 1;
 wild:
  {
    struct f2 **matches = NULL;
    int nmatches = 0;
    struct xcl_l *curr_l;

    /* move cl past ignorable discontinuities */
    while (cl_index + wild_tries < cl_max
	   && is_ignorable(&cls[cl_index+wild_tries]))
      ++cl_index;

    if (cl_index + wild_tries >= cl_max)
      return 0;
    else if (cls[cl_index+wild_tries].c->node_type != xcl_node_l)
      return 0;
    else
      curr_l = cls[cl_index+wild_tries].l;

    /* Don't use PSU_STOP for the first item in an NLE */
    if (curr_l->f && step_index && BIT_ISSET(curr_l->f->instance_flags,F2_FLAGS_PSU_STOP))
      return 0;

    matches = match(steps[step_index],curr_l,&nmatches,p);
    if (matches)
      {
	list_add(ngramify_mem,matches);
	add_match(curr_l, matches, tts ? tts[step_index] : NULL, psu, psu_form,
		  steps[step_index]->owner->user, nmatches);
	if (try_match(match_index+wild_tries+1,
		      steps, max_steps, step_index+1,
		      cls, cl_index+wild_tries+1, cl_max,
		      tts, psu, psu_form, p))
	  return 1;
	else
	  {
	    if (steps[step_index]->wild > wild_tries)
	      {
		add_wild_match(cls[cl_index+wild_tries].l);
		++wild_tries;
		goto wild;
	      }
	    else
	      return 0;
	  }
      }
    else
      return 0;
  }
}

static struct match *
next_match(void)
{
  /* WATCHME: this allocator works as long as the returned pointer is only used
     within the routine that calls next_match */
  if (match_list->matches_used == match_list->matches_alloced)
    {
      match_list->matches_alloced += 128;
      match_list->matches = realloc(match_list->matches,
				    match_list->matches_alloced * sizeof(struct match));
    }
  return &match_list->matches[match_list->matches_used++];
}

static void
add_match(struct xcl_l *lp, struct f2 **matches, struct CF*tt, 
	  const char *psu, struct f2 *psu_form, void *user, int nmatches)
{
  struct match *mp = next_match();
  mp->lp = lp;
  mp->wild = 0;
  mp->matching_f2s = matches;
  mp->nmatches = nmatches;
  mp->tt = tt;
  mp->psu = psu;
  mp->psu_form = psu_form;
  mp->user = user;
  if (matches[0]->cf)
    ngdebug("[add_match] registering match to %s[%s]",matches[0]->cf,matches[0]->gw);
  else if (matches[0]->pos)
    ngdebug("[add_match] registering match to %s",matches[0]->pos);
  else
    ngdebug("[add_match] registering match with no cf/pos");
}

static void
add_wild_match(struct xcl_l *lp)
{
  struct match *mp = next_match();
  mp->wild = 1;
  mp->lp = lp;
}

static int
match_props(struct prop *ilemp, struct prop *ngrmp)
{
  if (NULL == ilemp)
    return 0;
  while (ngrmp)
    {
      if (props_find_prop_sub(ilemp, ngrmp->name, ngrmp->value))
	ngrmp = ngrmp->next;
      else
	{
	  ngdebug("[props] failed on %s=%s", ngrmp->name, ngrmp->value);
	  return 0;
	}
    }
  ngdebug("[props] all prop tests passed OK");
  return 1;
}

static struct f2 **
match(struct CF *step, struct xcl_l *lp, int *nmatchesp, struct prop *props)
{
  static struct f2 *matches[128]; /*FIXME: dynamic please*/
  int nmatches = 0, i, nparses;
  struct f2 **parses;

#define add_lp_match(pform) matches[nmatches++]=pform /* just until we do it dynamically */
  
  if (lp->f)
    parses = lnodes_of(lp->f,&nparses);
  else
    nparses = 0;
  ngdebug("[match] testing %d parses for match to %s",nparses,step->cf);
  for (i = 0; i < nparses; ++i)
    {
      struct f2 *p = NULL;

      if (props)
	if (parses[i]->owner == NULL || !match_props(parses[i]->owner->props, props))
	  {
	    ngdebug("[props]: failed prop test because NLE has props and parse has none");
	    continue;
	  }
      
      p = parses[i];
      if ((step && p)
	  && (step->wild
	      || (step->f2 && !step->f2->cf && step->f2->pos && p->pos
		  && !strcmp((const char*)step->f2->pos,(char*)p->pos))
	      || (step->cf && p->cf && !strcmp(step->cf,(char*)p->cf))))
	{
	  if (check_predicates(p,step))
	    {
	      if (!step->neg)
		{
		  add_lp_match(p);
		  BIT_SET(p->flags,F2_FLAGS_NGRAM_MATCH);
		}
	    }
	  else
	    {
	      if (step->neg)
		{
		  add_lp_match(p);
		  BIT_SET(p->flags,F2_FLAGS_NGRAM_MATCH);
		}
	    }
	  if (step->wild && wild_reset == -1)
	    wild_reset = -2;
	}
    }
  matches[nmatches] = NULL;
  *nmatchesp = nmatches;
  ngdebug("[match] returning %d matches",nmatches);
  return matches[0] 
    ? memcpy(malloc((1+nmatches)*sizeof(struct form*)),
	     matches,
	     (1+nmatches)*sizeof(struct form*))
    : NULL;
}

static int
check_predicates(struct f2 *p, struct CF *cfp)
{
  const char *pform = (char*)p->form;
  int i = 0;

  if (cfp->f2->form && *cfp->f2->form)
    if (pform && !*pform && p->cof_id)
      pform = (char*)((struct f2*)((void*)(uintptr_t)p->cof_id))->form;

  if (pform && cfp->f2->form && *cfp->f2->form
      && strcmp((char*)pform, (char*)cfp->f2->form))
    return 0;
  
  /* Care needed with f2_test because we have to reverse argument 
     order to ensure that no sense on instance still matches 
     glossary's sig */
  if (!(cfnorm_ok(p, cfp->f2) && f2_test(cfp->f2, p)))
    return 0;
  
  if (cfp->preds)
    {
      for (i = 0; cfp->preds[i]; ++i)
	{
	  if (!strcmp(cfp->preds[i]->key, "vpr"))
	    {
	      /* is there a verb prefix? look at pos[0] == 'V' and morph =~ /:~/ */
	      if (p->pos && *p->pos == 'V' && p->morph && strstr((char*)p->morph, ":~"))
		{
		  if (cfp->preds[i]->neg)
		    return 0;
		}
	      else
		{
		  if (cfp->preds[i]->neg)
		    ; /* test passes , keep checking more preds */
		  else
		    return 0;
		}
	    }
	  else
	    {
	      fprintf(stderr, "%s:%d: bad <predicate> name '%s'\n",
		      cfp->owner->file,
		      cfp->owner->lnum,
		      cfp->preds[i]->key);
	      return 0; /* error condition, don't care about cfp->neg */
#if 0
	      const char *aname = cfp->preds[i]->key;
	      struct tok_tab *tp = NULL/*xfftok(aname,strlen(aname))*/;
	      if (tp)
		{
		  const char *lval = NULL/*fbyo(p,tp->offset)*/;
		  ngdebug("[check_predicates] testing %s for predval %s==instval %s",
			  aname,cfp->preds[i]->value, lval);
		  if (!strcmp(cfp->preds[i]->value,"*"))
		    ret = 1; /* NB no ret variable any more */
		  if (!lval || strcmp(cfp->preds[i]->value,lval))
		    ret = 0;
		}
#endif
	    }
	}
    }
  /* each cfp->neg has been handled by the time we get here, do don't check any more */
  return 1;
}

/*The name of this routine is an anachronism preserved to simplify the
  process of porting PV to CV.  In CV we actually make an array of all
  the forms referenced by lp's form member which are candidates for
  matching.
 */
static struct f2 **
lnodes_of(struct ilem_form *fp, int *nparsesp)
{
  struct f2 **parses = NULL;

  if (fp->ambig)
    {
      int i, j;
      struct ilem_form *tmp;
      int n = 0;
      /* count the total number of finds */
      for (tmp = fp; tmp; tmp = tmp->ambig)
	{
	  if (tmp->finds)
	    n += tmp->fcount;
	  else if (tmp->f2.cf || tmp->f2.pos)
	    ++n;
	}
      /* make parses an array containing all the finds of all the ambigs */
      parses = malloc(n * sizeof(struct f2 *));
      for (i = 0, tmp = fp; tmp; tmp = tmp->ambig)
	{
	  if (tmp->finds)
	    {
	      for (j = 0; j < tmp->fcount; ++j)
		parses[i++] = &tmp->finds[j]->f2;
	    }
	  else
	    {
	      if (tmp->f2.cf || tmp->f2.pos)
		parses[i++] = &tmp->f2;
	    }
	}
      *nparsesp = n;
#if 0
      for (i = 0, tmp = fp; i < fp->acount; ++i, tmp = tmp->ambig)
	if (tmp->f2.cf) /* FIXME: this needs to use fp->finds */
	  parses[i] = &tmp->f2;
#endif
    }
  else if (fp->finds)
    {
      int i;
      parses = malloc((*nparsesp = fp->fcount) * sizeof(struct f2 *));
      for (i = 0; i < fp->fcount; ++i)
	parses[i] = &fp->finds[i]->f2;
      *nparsesp = fp->fcount;
    }
  /* This already ensures that xcl lnodes with only POS get returned as candidate parses */
  else if (fp->f2.cf
	   || (fp->f2.pos /* && !strcmp((char*)fp->f2.pos,"n") */)) /* need to match GN etc. */
    {
      parses = malloc(sizeof(struct f2*));
      parses[0] = &fp->f2;
      *nparsesp = 1;
    }
  else
    {
      parses = NULL;
      *nparsesp = 0;
    }
  if (parses)
    list_add(ngramify_mem,parses);
  return parses;
}

/* FIXME: need to take presence of PSU into account in this so that
   PSU's sort at start of order */
static int
nle_cmp(struct NLE **a, struct NLE **b)
{
  if ((*a)->ncfs != (*b)->ncfs)
    return (*b)->ncfs - (*a)->ncfs;
  return (*a)->priority - (*b)->priority;
}

/*TODO: consider reimplementation of PV's initial wildcard allowance.

  TODO: reimplement PV's selection of ngrams by environment (i.e., MD).
 */
static struct NLE **
nle_heads(struct NL*nlp, struct ilem_form *fp, int *n_nodes)
{
  struct NLE **retp;
  struct f2 **p;
  struct NLE_set**nles_p;
  struct NLE_set *nles;
  int nparses, i, n_nles, total_nles, one = 1;
  Hash_table *seen_cfs = NULL;
  int try_pos = 0, arg_try_pos = 0;
  const unsigned char *cf_or_pos = NULL;

  if (!nlp)
    return NULL;

  if (!fp)
    {
      ngdebug("[nle_heads] passed NULL fp");
      *n_nodes = 0;
      return NULL;
    }
  else
    {
      /* support POS here as well as form ? */
      if (fp->f2.cf)
	ngdebug("[nle_heads] processing fp %s[%s]",fp->f2.cf,fp->f2.gw);
      else if (fp->f2.pos)
	ngdebug("[nle_heads] processing fp POS %s",fp->f2.pos);
      else /* This probably shouldn't be here unless it gets used by NSA vel sim */
	ngdebug("[nle_heads] processing fp form %s",fp->f2.form);
    }

  p = lnodes_of(fp,&nparses);
  ngdebug("[nle_heads] nparses == %d",nparses);
  seen_cfs = hash_create(1);
  nles_p = malloc((nparses+1)*sizeof(struct NLE_set*)); /* have to +1 for possible '*' in active hash */
  for (total_nles = n_nles = i = 0; i < nparses; ++i)
    {
#if 1
      try_pos = (arg_try_pos || !p[i]->cf);
      if (try_pos)
	{
	  if (!p[i]->pos || hash_find(seen_cfs, p[i]->pos))
	    continue;
	  cf_or_pos = p[i]->pos;
	}
      else
	{
	  if (!p[i]->cf || hash_find(seen_cfs, p[i]->cf))
	    continue;
	  cf_or_pos = p[i]->cf;
	}
#else
      /* allow lookup by ->pos here if ->cf == NULL */
      if (!p[i]->cf || hash_find(seen_cfs, p[i]->cf))
	continue;
#endif

      nles = hash_find(nlp->owner->active_hash,
		       (unsigned char *)cf_or_pos);
      if (nles)
	{
	  ngdebug("[nle_heads] found %s %s in %s", (try_pos ? "POS" : "CF"), cf_or_pos, nlp->name);
	  nles_p[n_nles++] = nles;
	  total_nles += nles->pp_used;
	  hash_add(seen_cfs, cf_or_pos, &one);
	}
    }
  hash_free(seen_cfs, NULL);
  if ((nles = hash_find(nlp->owner->active_hash, (unsigned char *)"*")))
    {
      ngdebug("[nle_heads] found CF '*' in active hash");
      nles_p[n_nles++] = nles;
      total_nles += nles->pp_used;
    }

  retp = malloc(total_nles * sizeof(struct NLE*));
  list_add(ngramify_mem,retp);
  *n_nodes = total_nles;

  for (total_nles = i = 0; i < n_nles; ++i)
    {
      memcpy(retp+total_nles,nles_p[i]->pp, nles_p[i]->pp_used * sizeof(struct NLE*));
      total_nles += nles_p[i]->pp_used;
    }

  free(nles_p);

  if (total_nles > 1)
    qsort(retp,total_nles,sizeof(struct NLE*),
	  (int(*)(const void *,const void*))nle_cmp);

  return retp;
}

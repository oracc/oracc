#include <ctype128.h>
#include "pool.h"
#include "xcl.h"
#include "props.h"
#include "sigs.h"
#include "ilem.h"
#include "warning.h"
#include "xli.h"

extern int lem_autolem, lem_dynalem;

int lem_do_wrapup = 1;
int lem_percent_threshold = 20;
int lem_extended = 1;
extern int verbose;
extern int bootstrap_mode;

static int md_match(const char *value, const char *key, Hash_table *mdsets, const char *mdrefs);
static struct ilem_form **md_selector(Hash_table *xcl_context_meta, Hash_table *mdsets, struct ilem_form **fp, int *nfinds);
static void md_select(struct xcl_l *lp, struct ilem_form *fp);
static void rank_disambig(struct xcl_context *xcp, struct xcl_l *lp);
static void wrapup_props(struct xcl_l *lp, struct ilem_form *fp);

static void ilem_wrapup_sub(struct xcl_context *xcp, struct xcl_l *lp, struct ilem_form *fp);

#if 0
/* this must return 1 if type == cof-tail */
static int 
cof_tail_test(struct ilem_form *fp, void *user, void *setup)
{
  return fp->type && !strcmp(fp->type,"cof-tail");
}
#endif

static int
cfgwpos_test(struct ilem_form *fp, void *user, void *setup)
{
  struct ilem_form*up = user;
  if (!fp->f2.pos)
    return 0;
  return strcmp((const char *)fp->f2.cf, (const char *)up->f2.cf)
    ||  strcmp((const char *)fp->f2.gw, (const char *)up->f2.gw)
    ||  strcmp((const char *)fp->f2.pos, (const char *)up->f2.pos);
}

static int
default_word_test(struct ilem_form *fp, void *user, void *setup)
{
  return (fp->f2.rank & 4) ? 0 : 1;
}

static int
default_esense_test(struct ilem_form *fp, void *user, void *setup)
{
  return (fp->f2.rank & 2) ? 0 : 1;
}

static int
default_isense_test(struct ilem_form *fp, void *user, void *setup)
{
  return (fp->f2.rank & 1) ? 0 : 1;
}

static int
frequency_test(struct ilem_form *fp, void *user, void *setup)
{
  return (*(int*)user == fp->freq) ? 0 : 1;
}

static int
gw_eq_sense_test(struct ilem_form *fp, void *user, void *setup)
{
  if (!fp->f2.gw || !fp->f2.sense)
    return 0;
  return !strcmp((char*)fp->f2.gw,(char*)fp->f2.sense);
}

static int
threshold_test(struct ilem_form *fp, void *user, void *setup)
{
  return (lem_percent_threshold > fp->freq) ? 0 : 1;
}

/* When all the lemmatization and ngram processing has been done, we
   iterate over the XCL tree one more time rewriting it ready for
   output.

   First we trim the results according to metadata matches if
   necessary.
   
   Then, the first item in the finds list becomes the principal entry;
   any further items in the list are dropped unless their percentage
   of occurrences is >= lem_percent_threshold.
 */
#if 0
void
ilem_drop_ambig_no_finds(xcl_l *lp)
{
  
}
#endif
void
ilem_wrapup(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (!xcp || !lp || !lp->f)
    return;

  /* Now attach the final version of the form to the lem node as a sig */
  if (lp->f->ambig)
    {
      struct ilem_form *fp = NULL, *first_fp = NULL;
#if 0
      if (lem_dynalem)
	ilem_drop_ambig_no_finds(lp);
#endif
      fp = lp->f;
      first_fp = lp->f;
      do
	{
	  ilem_wrapup_sub(xcp, lp, fp);
	  if ((fp = fp->ambig))
	    {
	      fp->sp = first_fp->sp;
	      fp->look = first_fp->look;
	    }
	}
      while (fp);
      if (lem_autolem || lem_dynalem)
	rank_disambig(xcp, lp);
    }
  else
    {
      ilem_wrapup_sub(xcp, lp, lp->f);
    }
}

void
ilem_wrapup_sub(struct xcl_context *xcp, struct xcl_l *lp, struct ilem_form *fp)
{
  struct ilem_form **fretp;
  int fcount;

  if (!fp->finds)
    {
      if (!fp->f2.cf && BIT_ISSET(fp->f2.flags, F2_FLAGS_NORM_IS_CF))
	fp->f2.cf = fp->f2.norm;
      if (BIT_ISSET(fp->f2.flags,F2_FLAGS_LEM_NEW) && !lem_autolem)
	/*(void)sigs_new_sig(xcp,fp)*/; /* not needed any more now that +-forms always go to cache and get checked on their way */
      else
	sigs_warn(xcp, lp, fp->look, fp);
      return;
    }
#if 0
  /* This must have been to make +-ed forms show up as wrong, but
     the real solution is to remove the + signs after merge */
  else if (fp->look && fp->look->type == sig_look_check)
    BIT_CLEAR(fp->f2.flags,F2_FLAGS_LEM_NEW);
#endif

  if (fp->fcount > 1)
    {
      if (lem_autolem || lem_dynalem)
	{
	  /*
	   * New handling for autolem/dynalem:
	   *
	   * First see if there is a default word flag in rank
	   * Then see if there is a default sense flag in rank
	   * Then see if any of the finds has a frequency > 0; they
	   *     are guaranteed to be sorted in order of frequency in 
	   *     the input .sig file so it's enough to check first find
	   * Then find the first implicit match--this was the first sense
	   *     in .glo order
	   */
	  fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,default_word_test,
			      NULL,&fcount);
	  if (fcount && fcount < fp->fcount)
	    {
	      memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
	      fp->fcount = fcount;
	    }
	  if (fp->fcount > 1)
	    {
	      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,default_esense_test,
				  NULL,&fcount);
	      if (fcount && fcount < fp->fcount)
		{
		  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
		  fp->fcount = fcount;
		}
	    }
	  if (fp->fcount > 1)
	    {
	      if (fp->finds[0]->freq > 0)
	      fretp = ilem_select(fp->finds,fp->fcount,&fp->finds[0]->freq,NULL,frequency_test,
				  NULL,&fcount);
	      if (fcount && fcount < fp->fcount)
		{
		  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
		  fp->fcount = fcount;
		}
	    }

	  if (fp->fcount > 1)
	    {
	      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,default_isense_test,
				  NULL,&fcount);
	      if (fcount && fcount < fp->fcount)
		{
		  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
		  fp->fcount = fcount;
		}
	    }

	  if (fp->fcount > 1)
	    {
	      /* Unresolved ambiguity */
	    }
	}
      else
	{
	  /* check that all the finds come from the same word (i.e., that CF/GW/POS matched) */
	  if (fp->fcount > 1)
	    {
	      fretp = ilem_select(fp->finds,fp->fcount,fp->finds[0],NULL,cfgwpos_test,
				  NULL,&fcount);
	      if (fcount && fcount == fp->fcount)
		{
		  /* OK, now we can go ahead and use default explicit/implicit sense */
		  if (fp->fcount > 1)
		    {
		      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,default_esense_test,
					  NULL,&fcount);
		      if (fcount && fcount < fp->fcount)
			{
			  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
			  fp->fcount = fcount;
			}
		    }
		  if (fp->fcount > 1)
		    {
		      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,default_isense_test,
					  NULL,&fcount);
		      if (fcount && fcount < fp->fcount)
			{
			  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
			  fp->fcount = fcount;
			}
		    }
		  if (fp->fcount > 1)
		    {
		      /* disambiguate cases where daku[kill] should match daku[kill//kill] but not daku[kill//defeat] */
		      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,gw_eq_sense_test,
					  NULL,&fcount);
		      if (fcount && fcount < fp->fcount)
			{
			  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
			  fp->fcount = fcount;
			}
		    }
		}
	    }
	  
	}
    }
  
  if (fp->fcount > 1 && !fp->explicit)
    md_select(lp, fp);

#if 0
  /* NEW: 2010-09-17 */
  if (!(fp->explicit&M_NORM0))
    fp->f2.norm = NULL;
#endif

  if (fp->fcount > 1)
    {
#if 1
      fretp = ilem_select(fp->finds,fp->fcount,NULL,NULL,threshold_test,
			 NULL,&fcount);
      if (fcount && fcount < fp->fcount)
	{
	  memcpy(fp->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
	  fp->fcount = fcount;
	}
#else
      int i,j;
      struct ilem_form *rover;
      /* FIXME: This code assumes that the finds list is sorted in
	 most-frequent-first order but I don't think that can be
	 assumed.  It's more complicated than just sorting in m-f-f
	 order, though, because the first sense(s) may have been
	 sorted to the front because they have defsense set */
      for (i = 1; 
	   i < fp->fcount && fp->finds[i]->freq >= lem_percent_threshold;
	   ++i)
	;
      if (i > 1)
	{
	  fp->acount = i-1;
	  for (j = 1, rover = lp->f; j < i; ++j, rover = rover->ambig)
	    rover->ambig = fp->finds[j];
	}
#endif
    }

  /* This handles verbose notices about success as well as diagnostics
     about failure, so we have to call it regardless of fp->count's
     value */
  if (fp->look && fp->look->type == sig_look_check)
      sigs_warn(xcp, lp, fp->look, fp);

  if (fp->fcount == 1 
      || (fp->fcount > 1 
	  && (lem_autolem || lem_dynalem || BIT_ISSET(fp->f2.flags, F2_FLAGS_LEM_NEW))))
    {
      /* must do this before inherit so that ->multi inheriting works */
      if (fp->finds[0]->f2.parts)
	fp->f2.parts = fp->finds[0]->f2.parts;

      if (!fp->finds[0]->f2.project
	  || strcmp(xcp->project, (char*)fp->finds[0]->f2.project))
	{
	  BIT_SET(fp->f2.flags, F2_FLAGS_NEW_BY_PROJ);
	  fp->finds[0]->f2.exo_project = fp->finds[0]->f2.project; /* save external source of sig */
	  fp->finds[0]->f2.exo_lang = fp->finds[0]->f2.lang; /* save lang of external sig */
	}
      else
	fp->f2.exo_project = NULL;
      /* Now force f2.project to the project being lemmatized */
      fp->f2.project = (unsigned char *)xcp->project;

      ilem_inherit(fp, fp->finds[0]); /* not lp->f because of ambig */

      fp->f2.sig = f2_sig(xcp, fp, &fp->f2);
      
      /* check this after inherit to get fields set correctly */
      if (!bootstrap_mode && strcmp((char*)fp->f2.lang, (char*)fp->finds[0]->f2.lang))
	{
#if 0
	  char *errsig = err_sig(fp);
	  vwarning2(fp->file,fp->lnum,
		    "%s is new for lang %s",
		    errsig, fp->f2.lang);
#endif
	  BIT_SET(fp->f2.flags, F2_FLAGS_NEW_BY_LANG);
	}

      fp->dict = fp->finds[0]->dict;
      fp->freq = fp->finds[0]->freq;
      fp->pct = fp->finds[0]->pct;
      
      /* FIXME: should this be run even when there are no finds? */
      if (fp->fcount)
	wrapup_props(lp, fp);

      /* Now attach the final version of the form to the lem node as a sig;
       *  - failed matches don't get a sig as it pollutes the glossary
       *  - COF tails don't get a sig either
       */
      if (!fp->f2.sig && !BIT_ISSET(fp->f2.flags, F2_FLAGS_COF_TAIL))
	fp->f2.sig = (unsigned char *)f2_sig(xcp,fp,&fp->f2);

      if (fp->multi && lp->f->mcount > 0)
	{
	  int i = 0;
	  struct ilem_form *m;
	  for (i = 0, m = fp->multi; m; m = m->multi)
	    if (fp->f2.parts && fp->f2.parts[i])
	      fp->multi->f2.tail_sig = fp->f2.parts[i]->tail_sig;
	}
    }
  else if (!BIT_ISSET(fp->f2.flags, F2_FLAGS_FROM_CACHE))
    BIT_SET(fp->f2.flags, F2_FLAGS_INVALID);

#if 0
  if (lp->ante_para || lp->post_para)
    ilem_para_resolve_anchors(lp);
#endif
  
#if 0 /* can't do this any more because the cache references the ilem node */
  free(fp->finds);
  fp->finds = NULL;
  fp->fcount = 0;
#endif
}

static int
md_match(const char *value, const char *key, Hash_table *mdsets, const char *mdrefs)
{
  char idbuf[32];
  const char *mdp = mdrefs;

  while (mdp && *mdp)
    {
      char *idp = idbuf;
      Hash_table *mdhash;
      while (*mdp && !isspace(*mdp))
	*idp++ = *mdp++;
      *idp = '\0';
      mdhash = hash_find(mdsets,(unsigned char *)idbuf);
      if (mdhash)
	{
	  const char *mdvalue = hash_find(mdhash,(unsigned char *)key);
	  if (value && mdvalue && !strcmp(value,mdvalue))
	    return 1;
	}
      else
	{
	  /* programmer or XML construction error; don't waste any more time with it */
	  fprintf(stderr, "md_match: mdref %s not in mdsets (mdsets = %p)\n", 
		  idbuf,(void*)mdsets);
	  return 0;
	}
      while (isspace(*mdp))
	++mdp;
    }
  return 0;
}

static struct ilem_form **
md_selector(Hash_table *xcl_context_meta, Hash_table *mdsets, struct ilem_form **fp, 
	    int *nfinds)
{
  const char * const keys[] = { "subgenre", "genre", "time", "place", NULL };
  struct ilem_form **res = malloc((1+*nfinds) * sizeof(struct ilem_form*));
  struct ilem_form **matches = fp;
  int i, nres;
  Hash_table *mdhashes;
  
  mdhashes = hash_find(mdsets, (unsigned char *)"#mdsets");

  if (!mdhashes)
    {
      fprintf(stderr,"md_match: no #mdsets entry found in metadata\n");
      *nfinds = 0;
      return NULL;
    }

  for (i = 0; keys[i]; ++i)
    {
      int j;
      /* For each key in turn, select forms which match the key */
      for (j = nres = 0; matches[j]; ++j)
	{
	  if (md_match(hash_find(xcl_context_meta,(unsigned char *)keys[i]),
		       keys[i], mdhashes, 
		       hash_find(mdsets,(unsigned char *)matches[j]->xml_id)))
	    res[nres++] = matches[j]; /* res and matches may be same array here */
	}

      if (nres)
	res[nres] = NULL;

      /* If we have 1 matching form we are done; 
	 if we have 0, reset and go again with the next key;
	 if we have >1, go again with the list so far */
      if (nres == 1)
	break;
      else if (!nres)
	matches = fp;
      else
	matches = res;
    }

  if (nres)
    memcpy(fp,res,(1+nres)*sizeof(struct ilem_form*));
  free(res);

  *nfinds = nres;

  return fp;
}

static void
md_select(struct xcl_l *lp, struct ilem_form *fp)
{
  struct ilem_form **mdfinds = fp->finds;
  int nselected = fp->fcount;
  Hash_table *use_mds = NULL;
  if (fp->sp->mdsets_hash)
    use_mds = fp->sp->mdsets_hash;
#if 0
  else
    {
      struct formsbylang *fblp = hash_find(fp->fsp->langs,
					   (unsigned char*)fp->lang);
      if (fblp)
	use_mds = fblp->meta;
      else
	return;
    }
#endif
  if (use_mds)
    mdfinds = md_selector(lp->parent->meta 
			  ? lp->parent->meta 
			  : lp->parent->ancestor_meta, 
			  use_mds, fp->finds, &nselected);
  if (nselected > 0)
    {
      fp->finds = mdfinds;
      fp->fcount = nselected;
    }
}

/* For each property in the found form from the CBD, search
   in the properties attached to the instance form to see if it
   was identified in this instance.
   
   If it was, import the P-attribute from the CBD form.
   
   If not, import the entire property from the CBD form.
 */
static void
wrapup_props(struct xcl_l *lp, struct ilem_form *fp)
{
  struct prop *xfp;
  struct prop *more = NULL;
  for (xfp = fp->finds[0]->props; xfp; xfp = xfp->next)
    {
      struct prop *xfp2;
      if ((xfp2 = props_find_prop(lp->f,xfp->name,xfp->value)))
	{
	  xfp2->p = xfp->p;
	}
      else
	{
	  struct prop *last = more;
	  if (last)
	    {
	      while (last->next)
		last = last->next;
	      last->next = malloc(sizeof(struct prop));
	      *last->next = *xfp;
	      last->next->xml_id = NULL;
	      last->next->next = NULL;
	    }
	  else
	    {
	      more = malloc(sizeof(struct prop));
	      *more = *xfp;
	      more->xml_id = NULL;
	      more->next = NULL;
	    }
	}
    }
  if (more)
    {
      struct prop *last;
      for (last = fp->props; last && last->next; last = last->next)
	;
      if (last)
	last->next = more;
      else
	fp->props = more;
    }
}

/* This is a very simple initial implementation: just look for the 
   result with the highest rank and use that.
 */
static void
rank_disambig(struct xcl_context *xcp, struct xcl_l *lp)
{
  struct ilem_form *fp = lp->f, *top = NULL;
  int top_rank = 0;
  for (fp = lp->f; fp; fp = fp->ambig)
    {
      if (fp->fcount)
	{
	  if (fp->finds[0]->f2.rank > top_rank)
	    {
	      top_rank = fp->finds[0]->f2.rank;
	      top = fp->finds[0];
	    }
	}
    }
  if (top)
    {
      lp->f = top;
      lp->f->ambig = NULL;
    }
}

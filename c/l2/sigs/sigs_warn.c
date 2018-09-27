#include <stdio.h>
#include <stdlib.h>
#include "warning.h"
#include "hash.h"
#include "npool.h"
#include "memblock.h"
#include "sigs.h"
#include "ilem.h"
#include "ilem_form.h"
#include "xcl.h"

char *
err_sig(struct ilem_form *fp)
{
  char *tsig = NULL;

  if (!fp->f2.cf && !fp->f2.gw && !fp->f2.pos)
    return NULL;

  tsig = malloc((fp->f2.cf ? strlen((char*)fp->f2.cf) : 0)
		+ (fp->f2.gw ? strlen((char*)fp->f2.gw) : 0)
		+ (fp->f2.pos ? strlen((char*)fp->f2.pos) : 0)
		+ (fp->f2.norm ? strlen((char*)fp->f2.norm) : 0)
		+ 7);
  sprintf(tsig,"%s[%s]%s$%s",
	  fp->f2.cf ? fp->f2.cf : (unsigned char *)"X",
	  fp->f2.gw ? fp->f2.gw : (unsigned char *)"X",
	  fp->f2.pos ? fp->f2.pos : (unsigned char *)"",
	  fp->f2.norm ? fp->f2.norm : (unsigned char *)"");
  return tsig;
}

static const char *
tryinst(struct xcl_l *lp, struct ilem_form *fp)
{
  char *try;

  if (lp->f->ambig)
    {
      if (lp->f->literal && lp->f->sublem)
	{
	  try = malloc(strlen(lp->f->literal)
		       + strlen(fp->sublem)
		       + 5);
	  sprintf(try, "%s in %s", fp->sublem, lp->f->literal);
	}
      else
	{
	  try = malloc(strlen(lp->inst) + 1);
	  strcpy(try, lp->inst);
	}
    }
  else
    {
      if (fp->sublem)
	{
	  try = malloc(strlen(fp->sublem) + 1);
	  strcpy(try,fp->sublem);
	}
      else
	{
	  try = malloc(strlen(lp->inst) + 1);
	  strcpy(try,lp->inst);
	}
    }
      
  return try;
}

#if 0
static char *
cof_form(struct ilem_form *fp, int curr)
{
  int i, n = 0;
  char *ret = NULL;

  if (fp->f2.norm)
    {
      n = strlen((char*)fp->f2.norm) + 4;
      for (i = 0; i < fp->mcount-1; ++i)
	if (fp->multi[i].f2.norm)
	  n += strlen((char*)fp->multi[i].f2.norm) + 4;
	else
	  return NULL;
    }
  else
    return NULL;

  ret = malloc(n);
  sprintf(ret, "$(%s)", fp->f2.norm);
  for (i = 0; i < fp->mcount-1; ++i)
    {
      if (i == curr)
	sprintf(ret+strlen(ret), " $%s", fp->multi[i].f2.norm);
      else
	sprintf(ret+strlen(ret), " $(%s)", fp->multi[i].f2.norm);
    }

  return ret;
}

static char *
entrysig(struct ilem_form *fp)
{
  char *tsig = NULL;

  if (!fp->f2.cf && !fp->f2.gw && !fp->f2.pos)
    return NULL;

  tsig = malloc((fp->f2.cf ? strlen((char*)fp->f2.cf) : 0)
		+ (fp->f2.gw ? strlen((char*)fp->f2.gw) : 0)
		+ (fp->f2.sense ? strlen((char*)fp->f2.sense) : 0)
		+ (fp->f2.pos ? strlen((char*)fp->f2.pos) : 0)
		+ 12);
  sprintf(tsig,"@entry %s [%s] %s",
	  fp->f2.cf ? fp->f2.cf : (unsigned char *)"X",
	  fp->f2.gw ? fp->f2.gw : (unsigned char *)"X",
	  fp->f2.pos ? fp->f2.pos : (unsigned char *)"");
  return tsig;
}
#endif

static const char *
trysig(struct ilem_form *fp)
{
  char *tsig = NULL;

  if (!fp->f2.cf && !fp->f2.gw && !fp->f2.pos)
    return NULL;

  tsig = malloc((fp->f2.cf ? strlen((char*)fp->f2.cf) : 0)
		+ (fp->f2.gw ? strlen((char*)fp->f2.gw) : 0)
		+ (fp->f2.sense ? strlen((char*)fp->f2.sense) : 0)
		+ (fp->f2.pos ? strlen((char*)fp->f2.pos) : 0)
		+ 6);
  sprintf(tsig,"%s[%s%s%s]%s",
	  fp->f2.cf ? fp->f2.cf : (unsigned char *)"X",
	  fp->f2.gw ? fp->f2.gw : (unsigned char *)"X",
	  fp->f2.sense ? "//" : "",
	  fp->f2.sense ? fp->f2.sense : (unsigned char *)"",
	  fp->f2.pos ? fp->f2.pos : (unsigned char *)"");
  return tsig;
}

void
sigs_warn(struct xcl_context *xcp, struct xcl_l *l, 
	  struct siglook *look, struct ilem_form *fp)
{
  int nfinds = fp->fcount;
  struct sigset *sp = fp->sp;

  /* don't generate warnings for cof_tails because they have
     already been given for the cof_head */
  if (l->cof_head || !l->f->f2.cf)
    return;

  /* this is an error condition that must come from an error */
  if (!look)
    return;

  if (nfinds > 1 || verbose 
      || BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM) 
      || BIT_ISSET(fp->f2.flags,F2_FLAGS_PARTIAL))
    {
      if (look->type == sig_look_new)
	{
	  if (verbose)
	    {
	      const char *es = (nfinds == 1 ? "" : "es");
	      vwarning2(fp->file,fp->lnum,
			"%d match%s for FORM `%s' in glossary %s:%s; freq=%d, pct=%d",
			nfinds, es, fp->f2.form, sp->project, sp->lang,
			fp->freq, fp->pct);
	    }
	}
      else if (nfinds && !BIT_ISSET(fp->f2.flags, F2_FLAGS_LEM_NEW))
	{
	  const char *tsig = trysig(fp);
	  const char *tinst = tryinst(l,fp);
	  if (BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM))
	    vwarning2(fp->file,fp->lnum,
		      "no FORM `%s'; %d match%s for %s in glossary %s:%s",
		      strcmp((char*)fp->f2.form, "*") ? fp->f2.form : fp->f2.norm,
		      nfinds, (nfinds>1)?"es":"", tsig, sp->project, sp->lang);
	  else
	    vwarning2(fp->file,fp->lnum,
		      "found %d %smatch%s for %s=%s in %s:%s",
		      nfinds, 
		      (BIT_ISSET(fp->f2.flags,F2_FLAGS_PARTIAL))?"partial ":"",
		      (nfinds>1)?"es":"", 
		      strcmp((char*)fp->f2.form, "*") ? fp->f2.form : fp->f2.norm,
		      tsig, sp->project, sp->lang);
	  free((void*)tsig);
	  free((void*)tinst);
	}
    }

  if (!BIT_ISSET(fp->f2.flags, F2_FLAGS_LEM_NEW)
      && (BIT_ISSET(fp->f2.flags,F2_FLAGS_PARTIAL)
	  || (nfinds > 1 
	      && !BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM)
	      && !(look->type == sig_look_new))
	  || verbose > 1
	  || (verbose && look->type == sig_look_new)
	  ))
    {
      int i, max_show;
      
      if (nfinds > 3)
	max_show = nfinds;
      else
	max_show = nfinds;
      for (i = 0; i < max_show; ++i)
	{
	  char *sig = NULL;

	  if (fp->finds && fp->finds[i])
	    sig = (char*)fp->finds[i]->f2.sig;
	    
	  /* refactor this to use fp->finds */
	  if (sig)
	    {
	      char *tab = strchr(sig,'\t');
	      const char *colon;
	      extern int nwarning;
	      if (tab)
		*tab = '\0';
	      colon = strchr(sig, ':');
	      if (colon)
		while (':' == *colon)
		  ++colon;
	      else
		colon = (char*)sig;
	      vwarning2(fp->file,fp->lnum,"\t%s",colon);
	      --nwarning;
	      if (tab)
		*tab = '\t';
	    }
	  else
	    break;
	}
      return;
    }
 else
   {
     if ((fp->fcount == 0 && (!BIT_ISSET(fp->f2.flags,F2_FLAGS_LEM_NEW)) && fp->f2.cf)
	 )
       /*	 || BIT_ISSET(fp->f2.flags,F2_FLAGS_COF_INVALID)) */
       {
	 const char *tsig = trysig(fp);
	 const char *tinst = tryinst(l,fp);
	 char tlang[32];
	 if (sp)
	   {
	     strcpy(tlang,(char*)sp->lang);

#if 0
	     /* THIS NEEDS MORE WORK */
	     if (BIT_ISSET(fp->f2.flags, F2_FLAGS_COF_INVALID))
	       {
		 /* This means that a COF has matched on the HEAD
		    but failed on one of the TAILs */
		 int i;
		 for (i = 0; i < fp->mcount-1; ++i)
		   {
		     if (BIT_ISSET(fp->multi[i].f2.flags, F2_FLAGS_COF_INVALID))
		       {
			 if (!BIT_ISSET(fp->multi[i].f2.flags, F2_FLAGS_LEM_NEW))
			   {
			     char *sublem = strchr(fp->multi[i].sublem, ':');
			     char *esig = NULL, *cform = NULL;
			     if (sublem)
			       ++sublem;
			     else
			       sublem = fp->multi[i].sublem;
			     vwarning2(fp->file,fp->lnum,
				       "%s (segment #%d of COF): not found in %s:%s",
				       sublem, i+2,
				       sp->project, sp->lang);
			     esig = entrysig(&fp->multi[i]);
			     cform = cof_form(fp, i);
			     if (esig && cform)
			       vnotice("\t%s may need @form %s", esig, cform);
			     if (esig)
			       free(esig);
			     if (cform)
			       free(cform);
			   }
			 /* we need to break after the first error, even if that is a LEM_NEW fail
			    because if one element in the chain fails they all do so not breaking
			    effectively causes spurious knock-on errors */
			 break;
		       }
		   }
	       }
	     else 
#endif
	     if (BIT_ISSET(fp->f2.flags,F2_FLAGS_NO_FORM))
	       vwarning2(fp->file,fp->lnum,
			 "no FORM `%s' and no matches for %s in glossary %s:%s",
			 fp->f2.form,
			 tsig, sp->project, sp->lang);
	     else
	       {
		 if (fp->multi)
		   {
		     vwarning2(fp->file,fp->lnum,
			       "no matches for COF %s=%s in glossary %s:%s",
			       fp->f2.form, fp->literal, sp->project, sp->lang);
		   }
		 else
		   {
		     vwarning2(fp->file,fp->lnum,
			       "no matches for %s=%s in glossary %s:%s",
			       fp->f2.form, tsig, sp->project, sp->lang);
		   }
	       }
	   }
	 else
	   {
	     if (!BIT_ISSET(fp->f2.flags, F2_FLAGS_INVALID))
	       vwarning2(fp->file,fp->lnum,
			 "no matches for %s=%s because there's no glossary",
			 fp->f2.form, tsig);
	   }
	 free((void*)tsig);
	 free((void*)tinst);
       }
     else
       {
	 /* warn about missing NORM etc. */
       }
   }
}

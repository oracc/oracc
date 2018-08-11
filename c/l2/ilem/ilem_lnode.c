#include "pool.h"
#include "xcl.h"
#include "ilem.h"
#include "sigs.h"
#include "warning.h"

extern int lem_autolem, lem_dynalem, verbose;
#if 0
static int
freq_cmp(struct ilem_form **a,struct ilem_form **b)
{
  return (*b)->freq - (*a)->freq;
}

static int
pn_00_test(struct ilem_form *fp,void *user, void*data)
{
  return !strcmp((char*)fp->f2.gw, "00");
}

static void
safe_update_literal(struct ilem_form *fp)
{
#if 0
  if (fp->pos && fp->epos && strcmp(fp->pos,fp->epos) && !(fp->explicit&M_EPOS))
    {
      fp->epos = fp->pos;
      fp->result |= NEW_EPOS;
    }
#endif

  if (fp->type && !strcmp(fp->type,"cof-head"))
    fp->literal = (char *)pool_copy((unsigned char *)fp->finds[0]->literal);
  else
    {
      if (!is_proper(fp) && !(fp->explicit&M_SENSE))
	fp->f2.sense = fp->finds[0]->f2.sense;
      
      if (!fp->sublem 
	  || (!fp->sublem[1] && (fp->sublem[0] == 'X' || fp->sublem[0] == 'L'))
	  || is_proper(fp)) /* MAKE NN-upgrades selectable by a variable? */
	{
	  static char buf[128];
	  char *cf_or_norm0 = (char*)((strcmp(fp->lang,"sux")&&fp->f2.norm)
				      ? fp->f2.norm : fp->f2.cf);
	  if (!fp->f2.pos[2] && 'P' == fp->f2.pos[0] && 'N' == fp->f2.pos[1])
	    fp->f2.sense = (unsigned char *)"00";
	  if (fp->sdisamb)
	    sprintf(buf,"%s[%s\\%s]",cf_or_norm0,fp->f2.sense,fp->sdisamb);
	  else if (!strcmp(fp->lang,"sux"))
	    sprintf(buf,"%s[%s]",cf_or_norm0,fp->f2.gw);
	  else
	    sprintf(buf,"%s[%s]",cf_or_norm0,fp->f2.sense);
	  if (strcmp(fp->lang,"sux"))
	    {
	      if (fp->f2.epos && strcmp((char*)fp->f2.pos,(char*)fp->f2.epos))
		sprintf(buf+strlen(buf),"'%s",fp->f2.epos);
	      else
		strcat(buf,(char*)fp->f2.pos);
	    }
	  if (fp->fdisamb)
	    {
	      if (*fp->fdisamb == '\\')
		strcat(buf,fp->fdisamb);
	      else
		sprintf(buf+strlen(buf),"\\%s",fp->fdisamb);
	    }
	  fp->literal = (char *)pool_copy((unsigned char *)buf);
	}
    }
}
#endif
void
ilem_lnode(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (!xcp || !lp || !lp->f)
    {
      /* This can result from a previous error, so we don't need to
	 warn about it again */
      /*vwarning("internal error: lem_lnode called with NULL arg");*/
      return;
    }
  if (lp->f->f2.pos && lp->f->f2.pos[0] == 'u' && !lp->f->f2.pos[1])
    return;

#if 0
  if (!lp->f->cf) /* this skips POS=u, n, X, also DN etc. */
    return;
#endif

  if (lp->f->sublem && lp->f->f2.cf)
    {
      sigs_l_check(xcp,lp);
    }
  else if ((lem_autolem || lem_dynalem) && lp->f->f2.form)
    {
      if (verbose > 1)
	fprintf(stderr,"lem_lnode:%s:%d: processing %s\n",
		lp->f->f2.file,(int)lp->f->f2.lnum,lp->f->f2.form);
#if 0
      foreach (; ; )
	{
	  lem_forms(xcp->project,lp,"project");
	  if (!lp->f->result)
	    break;
	}
      xff_inherit(lp->f,lp->f->finds[0]);
      lp->f->fcount = 1;
      safe_update_literal(lp->f);
#endif

#ifdef OLD_IMPLEMENTATION
	  if (lp->f->result == 0 && xcp->system)
	    {
	      extern int shadow_lem;
	      lem_forms(xcp->system,lp,"system");
	      if (lp->f->result > 0 && shadow_lem)
		lp->f->instance_flags |= F2_FLAGS_SHADOW;
	    }
	  if (lp->f->result > 0)
	    {
	      if (!strcmp(lp->f->finds[0]->f2.pos,"PN"))
		{
		  struct ilem_form **fretp;
		  int fcount;
		  fretp = lem_select(lp->f->finds,lp->f->fcount,NULL,NULL,pn_00_test,NULL,&fcount);
		  if (fcount == 1)
		    {
		      memcpy(lp->f->finds,fretp,(1+fcount)*sizeof(struct ilem_form *));
		      lp->f->fcount = fcount;
		    }		  
		}
	      if (lp->f->fcount > 1)
		qsort(lp->f->finds,lp->f->fcount,sizeof(struct ilem_form *),
		      (int(*)(const void *,const void*))freq_cmp);
	    }
	  if (lp->f->fcount)
	    {
	      xff_inherit(lp->f,lp->f->finds[0]);
	      safe_update_literal(lp->f);
	    }
	}
      else
	{
	  xff_inherit(lp->f,lp->f->finds[0]);
	  lp->f->fcount = 1;
	  safe_update_literal(lp->f);
	}
#endif
    }
}

#include "lang.h"
#include "warning.h"
#include "xcl.h"
#include "ilem.h"

extern int dump_sigs;

void
sigs_dump_sigs(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (dump_sigs > 1)
    {
      fprintf(stdout, "%s:%d:%s:%s", lp->f->file, (int)lp->f->lnum, lp->f->ref, lp->f->sublem ? lp->f->sublem : lp->inst);
      if (lp->f->f2.sig)
	fprintf(stdout, "\t%s", lp->f->f2.sig);
      fprintf(stdout, "\n");
    }
  else
    {
      if (lp && lp->f && lp->f->f2.pos && strcmp((const char*)lp->f->f2.pos,"X") 
	  && !BIT_ISSET(lp->f->f2.flags,F2_FLAGS_FROM_CACHE)
	  && ((lp->f->f2.base && *lp->f->f2.base)
	      || !BIT_ISSET(lp->f->f2.core->features,LF_BASE))
	  && lp->f->f2.sig)
	{
#if 1
	  fprintf(stdout, "%s:%d:%s\t%s\n", lp->f->file, (int)lp->f->lnum, lp->f->sublem,lp->f->f2.sig);
#else
	  fprintf(stdout,"@%s%s\t%s\n",
		  lp->f->f2.project,
		  lp->f->sublem,
		  lp->f->f2.sig);
#endif
	}
      else
	{
	  fprintf(stdout, "%s:%d:%s\n", lp->f->file, (int)lp->f->lnum, lp->f->sublem ? lp->f->sublem : lp->inst);
	}
    }
}

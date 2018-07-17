#include "lang.h"
#include "warning.h"
#include "xcl.h"
#include "ilem.h"

void
sigs_dump_sigs(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (lp && lp->f && lp->f->f2.pos && strcmp((const char*)lp->f->f2.pos,"X") 
      && !BIT_ISSET(lp->f->f2.flags,F2_FLAGS_FROM_CACHE)
      && ((lp->f->f2.base && *lp->f->f2.base)
	  || !BIT_ISSET(lp->f->f2.core->features,LF_BASE))
      && lp->f->f2.sig)
    fprintf(stdout,"@%s%s\t%s\n",
	    lp->f->f2.project,
	    lp->f->sublem,
	    lp->f->f2.sig);
}

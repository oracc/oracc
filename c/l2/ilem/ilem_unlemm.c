#include "warning.h"
#include "xcl.h"
#include "ilem.h"

extern FILE *f_unlemm;

void
ilem_unlemm_warnings(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (!xcp || !lp || !lp->f)
    {
      /* This can result from a previous error, so we don't need to
	 warn about it again */
      /*vwarning("internal error: lem_lnode called with NULL arg");*/
      return;
    }
  
  if (lp->f->f2.pos && !lp->f->f2.pos[1] && *lp->f->f2.pos == 'X')
    fprintf(f_unlemm,"%%%s\t%s\n",lp->f->f2.lang,lp->f->f2.form);
}

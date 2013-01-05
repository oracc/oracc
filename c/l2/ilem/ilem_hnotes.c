#include "warning.h"
#include "ilem_form.h"
#include "xcl.h"
#include "ilem.h"

#if 0
static char *
render_fp(struct ilem_form *fp)
{
  static char buf[128];
  sprintf(buf,"%s[%s]%s",fp->f2.cf,fp->f2.gw,fp->f2.pos);
  return buf;
}
#endif

void
ilem_harvest_notices(struct xcl_context *xcp, struct xcl_l *lp)
{
  if (!xcp || !lp || !lp->f)
    {
      /* This can result from a previous error, so we don't need to
	 warn about it again */
      /*vwarning("internal error: lem_lnode called with NULL arg");*/
      return;
    }
  
  if (!lp->f->f2.pos || (!lp->f->f2.pos[1] 
		      && (*lp->f->f2.pos == 'X' || *lp->f->f2.pos == 'u' 
			  || *lp->f->f2.pos == 'n' || *lp->f->f2.pos == 'M'))
      || lp->f->is_part)
    return;

#if 0
  if (!lp->f->dict || !strcmp(lp->f->dict->dict_id,"#runtime"))
    vwarning2(lp->f->f2.file,lp->f->f2.lnum,"%s=%s creates new ENTRY %s", 
	      lp->f->f2.form, lp->f->sublem, render_fp(lp->f));
  else if (N_RESULT(lp->f->result))
    {
      char *n = (char*)lem_n_bits(lp->f->result),*n2;
      for (n2 = n; *n2; ++n2)
	if (*n2 == '|')
	  *n2 = ',';
      vwarning2(lp->f->file,lp->f->lnum,"%s=%s adds new %s to %s",
		lp->f->form,lp->f->sublem,n,render_fp(lp->f));
    }
#endif
}

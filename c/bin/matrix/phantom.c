#include <psd_base.h>
#include "warning.h"
#include "pf_lib.h"
void
phantom_prepare_phantoms (void *vp)
{
  Composite_column*cp = vp;
  if (cp->needs_phantom)
    {
      Source_column *sp;
      ++cp->complex->complex_phantoms;
      for (sp = cp->down; sp; sp = sp->down)
	{
	  if (sp->columns_occupied || sp->filler_flag)
	    sp->needs_phantom = TRUE;
	  else
	    {
	      Source_column *sp2;
	      for (sp2 = sp; sp2->left && 0 == sp2->columns_occupied; sp2 = sp2->left)
		;
	      if (sp2->index)
		sp2->needs_phantom = TRUE;
	      else
		{
		  vwarning2(iterate_line->file,iterate_line->linenum,"%s",
			    "unable to migrate phantom leftwards");
		  sp->needs_phantom = TRUE;
		}
	    }
	}
    }
}

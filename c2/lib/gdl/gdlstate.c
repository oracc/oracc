#include <stdio.h>
#include <gdlstate.h>

/* Simple routine to dump out gdlstate when printing raw XML, i.e.,
   the simple form that reflects the internal storage structure */
void
gdlstate_rawxml(FILE *fp, struct gdlstate *sp)
{
  if (sp)
    {
      if (sp->f_star)
	fprintf(fp, " flags=\"*\"");
    }
}

#include <stddef.h>
#include "lang.h"
#include "ilem_form.h"
#include "f2.h"
void
ilem_inherit(struct ilem_form*inheritor, struct ilem_form *from)
{
#define inheritx(memb) if(!(inheritor->memb)&&from->memb)inheritor->memb=from->memb

  inheritx(type);
  inheritx(aform);
  inheritx(fdisamb);
  inheritx(sdisamb);
  inheritx(onorm);
  inheritx(psu_sense);
  inheritx(pref);
  inheritx(xml_id);
  inheritx(status);
#undef inheritx
  if (BIT_ISSET(from->f2.flags, F2_FLAGS_CLEARED))
    f2_clear(&inheritor->f2);
  f2_inherit(&inheritor->f2, &from->f2);
  
  /* mcount == -1 means this is a TAIL not a HEAD */
  if (inheritor->multi && inheritor->mcount > 0)
    {
      int i;
      struct ilem_form *m;
      for (i = 0, m = inheritor->multi; m; ++i, m = m->multi)
	if (from->f2.parts && from->f2.parts[i])
	  f2_inherit(&m->f2, from->f2.parts[i]);
    }
}

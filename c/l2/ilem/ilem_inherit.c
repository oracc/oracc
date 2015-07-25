#include <stddef.h>
#include "ilem_form.h"

static void
inherit_f2(struct f2 *inheritor_f2, struct f2 *from_f2)
{
#define inherit(memb) if((!(inheritor_f2->memb) || !(*inheritor_f2->memb))&&from_f2->memb)inheritor_f2->memb=from_f2->memb

  if (BIT_ISSET(inheritor_f2->flags, F2_FLAGS_FROM_CACHE))
    {
      *inheritor_f2 = *from_f2;
      return;
    }

  if (!strcmp((const char *)inheritor_f2->form, "*"))
    inheritor_f2->form = from_f2->cf;
  else
    inherit(form);
  inherit(base);
  inherit(cont);
  inherit(norm);

  /* Fix the CF and GW fields: in L2 we can't make any kind
     of a match without these either matching CF/NORM or
     GW/SENSE, so this coercion is safe */
  if (!inheritor_f2->cf || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			    && strcmp((char*)inheritor_f2->cf,(char*)from_f2->cf)))
    inheritor_f2->cf = from_f2->cf;
  if (!inheritor_f2->gw || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			    && strcmp((char*)inheritor_f2->gw,(char*)from_f2->gw)))
    inheritor_f2->gw = from_f2->gw;
  if (!inheritor_f2->sense || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			       && strcmp((char*)inheritor_f2->sense,(char*)from_f2->sense)))
    inheritor_f2->sense = from_f2->sense;

  if (!inheritor_f2->pos)
    inheritor_f2->pos = from_f2->pos;
  else
    {
      if (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS))
	{
	  if (strcmp((char*)inheritor_f2->pos,(char*)from_f2->pos))
	    {
	      /* silently correct x[y]N where it is really x[y]'N */
	      inheritor_f2->epos = inheritor_f2->pos;
	      inheritor_f2->pos = from_f2->pos;
	    }
	}
    }

  inherit(epos);

  inherit(stem);
  inherit(morph);
  inherit(morph2);
#undef inherit
}

void
ilem_inherit(struct ilem_form*inheritor, struct ilem_form *from)
{
#define inheritx(memb) if(!(inheritor->memb)&&from->memb)inheritor->memb=from->memb

  if (!strcmp((const char *)inheritor->f2.form, "*"))
    {
      if (from->f2.norm)
	inheritor->f2.form = from->f2.norm; /* REALLY?? */
      if (!inheritor->f2.cf)
	inheritor->f2.cf = from->f2.cf; /* REALLY?? */
      /* need to do any more here? */
    }

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
  inherit_f2(&inheritor->f2, &from->f2);
  /* mcount == -1 means this is a TAIL not a HEAD */
  if (inheritor->multi && inheritor->mcount > 0)
    {
      int i;
      struct ilem_form *m;
      for (i = 0, m = inheritor->multi; m; ++i, m = m->multi)
	if (from->f2.parts && from->f2.parts[i])
	  inherit_f2(&m->f2, from->f2.parts[i]);
    }
}

#include <stddef.h>
#include <lang.h>
#include <f2.h>

void
f2_inherit(struct f2 *inheritor_f2, struct f2 *from_f2)
{
#define inherit(memb) if((!(inheritor_f2->memb) || !(*inheritor_f2->memb))&&from_f2->memb)inheritor_f2->memb=from_f2->memb

  if (BIT_ISSET(inheritor_f2->flags, F2_FLAGS_FROM_CACHE))
    {
      *inheritor_f2 = *from_f2;
      return;
    }

#if 0
  if (!strcmp((const char *)inheritor_f2->form, "*"))
    inherit(cf);
#endif

  inherit(base);
  inherit(cont);
  if (!BIT_ISSET(inheritor_f2->core->features, LF_NORM) && inheritor_f2->norm && !strcmp((const char*)inheritor_f2->norm, "X"))
    {
      /* fprintf(stderr, "fixing norm=X\n"); */
      inheritor_f2->norm = from_f2->norm;
      inheritor_f2->sig = NULL;
    }
  else
    inherit(norm);

  /* Fix the CF and GW fields: in L2 we can't make any kind
     of a match without these either matching CF/NORM or
     GW/SENSE, so this coercion is safe */
  if (!inheritor_f2->cf || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			    && strcmp((char*)inheritor_f2->cf,(char*)from_f2->cf)))
    {
      inheritor_f2->cf = from_f2->cf;
      BIT_CLEAR(inheritor_f2->flags, F2_FLAGS_NORM_IS_CF);
    }
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

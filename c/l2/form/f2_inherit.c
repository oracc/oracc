#include <stddef.h>
#include <lng.h>
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

  if (from_f2->exo_project)
    inheritor_f2->exo_project = from_f2->exo_project;
  if (from_f2->exo_lang)
    inheritor_f2->exo_lang = from_f2->exo_lang;
  
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
			    && from_f2->cf && strcmp((char*)inheritor_f2->cf,(char*)from_f2->cf)))
    {
      inheritor_f2->cf = from_f2->cf;
      BIT_CLEAR(inheritor_f2->flags, F2_FLAGS_NORM_IS_CF);
    }
  if (!inheritor_f2->gw || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			    && from_f2->gw && strcmp((char*)from_f2->gw, "X")
			    && strcmp((char*)inheritor_f2->gw,(char*)from_f2->gw)))
    inheritor_f2->gw = from_f2->gw;
  if (!inheritor_f2->sense || (!BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS)
			       && from_f2->sense && strcmp((char*)inheritor_f2->sense,(char*)from_f2->sense)))
    inheritor_f2->sense = from_f2->sense;

  if (!inheritor_f2->pos)
    inheritor_f2->pos = from_f2->pos;
  else
    {
      if (from_f2->pos && !BIT_ISSET(inheritor_f2->flags, F2_FLAGS_NOT_IN_SIGS))
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
  BIT_SET(inheritor_f2->flags, F2_FLAGS_NEWLITERAL);
}

void
f2_clear(struct f2 *f2p)
{
  f2p->cf = f2p->gw = f2p->sense = f2p->pos = f2p->epos
    = f2p->norm = f2p->base = f2p->cont = f2p->morph
    = f2p->morph2 = f2p->rws = f2p->stem = f2p->augment
    = f2p->restrictor = f2p->sig
    = NULL;
  BIT_SET(f2p->flags, F2_FLAGS_CLEARED);
}

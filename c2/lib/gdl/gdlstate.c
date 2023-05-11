#include <stdio.h>
#include <gdlstate.h>

gdlstate_t gs_order_f[] = { gs_f_query, gs_f_bang, gs_f_star, gs_f_hash,
			    gs_f_uf1, gs_f_uf2, gs_f_uf3, gs_f_uf4, gs_f_plus };

gdlstate_t gs_order_o[] = { gs_damaged_o, gs_lost_o, gs_maybe_o, gs_det_o, gs_supplied_o,
			    gs_excised_o, gs_implied_o, gs_erased_o, gs_cancelled_o,
			    gs_superposed_o, gs_glolin_o, gs_glodoc_o };

gdlstate_t gs_order_c[] = { gs_damaged_c, gs_lost_c, gs_maybe_c, gs_det_c, gs_supplied_c,
			    gs_excised_c, gs_implied_c, gs_erased_c, gs_cancelled_c,
			    gs_superposed_c, gs_glolin_c, gs_glodoc_c };

/* Simple routine to dump out gdlstate when printing raw XML, i.e.,
   the simple form that reflects the internal storage structure */
void
gdlstate_rawxml(FILE *fp, gdlstate_t sp)
{
#if 0
  if (sp->f_star)
    fprintf(fp, " flags=\"*\"");
  if (sp->lost == SB_OP)
    fprintf(fp, " o=\"[\"");
  else if (sp->lost == SB_CL)
    fprintf(fp, " c=\"]\"");
#endif
}

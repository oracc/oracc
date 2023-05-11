#include <stdio.h>
#include <string.h>
#include <gdlstate.h>
#include <xmlify.h>

gdlstate_t gs_order_f[] = { gs_f_query, gs_f_bang, gs_f_star, gs_f_hash,
			    gs_f_uf1, gs_f_uf2, gs_f_uf3, gs_f_uf4, gs_f_plus };

gdlstate_t gs_order_o[] = { gs_damaged_o, gs_lost_o, gs_maybe_o, gs_det_o, gs_supplied_o,
			    gs_excised_o, gs_implied_o, gs_erased_o, gs_cancelled_o,
			    gs_superposed_o, gs_glolin_o, gs_glodoc_o, gs_surro_o };

gdlstate_t gs_order_c[] = { gs_damaged_c, gs_lost_c, gs_maybe_c, gs_det_c, gs_supplied_c,
			    gs_excised_c, gs_implied_c, gs_erased_c, gs_cancelled_c,
			    gs_superposed_c, gs_glolin_c, gs_glodoc_c, gs_surro_c };

#define NFLAGS (sizeof(gs_order_f)/sizeof(gdlstate_t))
#define NBRACK (sizeof(gs_order_o)/sizeof(gdlstate_t))

static char flagbuf[NFLAGS/2 + (NFLAGS*3) + 1];
static char brackobuf[NBRACK*2];
static char brackcbuf[NBRACK*2];

static const char *
gs_flagstr(gdlstate_t s)
{
  switch (s)
    {
    case gs_f_star:
      return "*";
    case gs_f_bang:
      return "!";
    case gs_f_query:
      return "?";
    case gs_f_hash:
      return "#";
    }
  return NULL;
}

static const char *
gs_brackostr(gdlstate_t s)
{
  switch (s)
    {
    case gs_supplied_o:
      return "<";
    case gs_lost_o:
      return "[";
    }
  return NULL;
}

static const char *
gs_brackcstr(gdlstate_t s)
{
  switch (s)
    {
    case gs_supplied_c:
      return ">";
    case gs_lost_c:
      return "]";
    }
  return NULL;
}

void
gsraw_flags(gdlstate_t sp)
{
  int i;
  *flagbuf = '\0';
  for (i = 0; i < NFLAGS; ++i)
    if (gs_is(sp,gs_order_f[i]))
      strcat(flagbuf, gs_flagstr(gs_order_f[i]));
}

void
gsraw_bracko(gdlstate_t sp)
{
  int i;
  *brackobuf = '\0';
  for (i = 0; i < NBRACK; ++i)
    if (gs_is(sp,gs_order_o[i]))
      strcat(brackobuf, gs_brackostr(gs_order_o[i]));
}

void
gsraw_brackc(gdlstate_t sp)
{
  int i;
  *brackcbuf = '\0';
  for (i = NBRACK-1; i >= 0; --i)
    if (gs_is(sp,gs_order_c[i]))
      strcat(brackcbuf, gs_brackcstr(gs_order_c[i]));
}

/* Simple routine to dump out gdlstate when printing raw XML, i.e.,
   the simple form that reflects the internal storage structure */
void
gdlstate_rawxml(FILE *fp, gdlstate_t sp)
{
  gsraw_flags(sp);
  gsraw_bracko(sp);
  gsraw_brackc(sp);
  if (*flagbuf)
    fprintf(fp, " flags=\"%s\"", flagbuf);
  if (*brackobuf)
    fprintf(fp, " o=\"%s\"", xmlify((uccp)brackobuf));
  if (*brackcbuf)
    fprintf(fp, " c=\"%s\"", xmlify((uccp)brackcbuf));
}

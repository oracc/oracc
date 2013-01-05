%{
#include <string.h>
#include "block.h"
#include "blocktok.h"
%}
struct block_token
{
  const char *name;
  enum e_type etype;
  const char *n;
  enum block_levels type;
  const char *full;
  const char *abbr;
  const char *nano;
}
%%
composite,  e_composite, NULL, TEXT, NULL, NULL, NULL
score,  e_score, NULL, TEXT, NULL, NULL, NULL
transliteration, e_transliteration, NULL, TEXT, NULL, NULL, NULL
tablet,  	e_object, "tablet", OBJECT, NULL, NULL, NULL
bulla,		e_object, "bulla", OBJECT, "Bulla", "Bulla", "bulla"
prism,		e_object, "prism", OBJECT, "Prism", "Prism", "prism"
envelope,	e_object, "envelope", OBJECT, "Envelope", "Env.", "env"
object,	    e_object, "", OBJECT, NULL, NULL, NULL
obverse, 	e_surface, "obverse", SURFACE, "Obverse", "Obv.", "o"
reverse,	e_surface, "reverse", SURFACE, "Reverse", "Rev.", "r"
side,	e_surface, "side", SURFACE, "Side", "Side", "s."
left,		e_surface, "left", SURFACE, "Left edge", "Left", "l.e."
right,		e_surface, "right", SURFACE, "Right edge", "Right", "r.e."
top,		e_surface, "top", SURFACE, "Top edge", "Top", "t.e."
bottom,	    e_surface, "bottom", SURFACE, "Bottom edge", "Bottom", "b.e."
edge,		e_surface, "edge", SURFACE, "Edge", "Edge", "e."
face,		e_surface, "face", SURFACE, "Face", "face", NULL
surface,	e_surface, "", SURFACE, NULL, NULL, NULL
seal,		e_surface, "seal", SURFACE, "Seal", "seal", NULL
docket,		e_surface, "docket", SURFACE, "Docket", "docket", NULL
column, 	e_column, NULL, COLUMN, NULL, NULL, NULL
m,			e_m, "", MILESTONE, NULL, NULL, NULL
fragment,	e_m, "fragment", MILESTONE, "Fragment", "Frag.", "frg"
cfragment,	e_m, "cfragment", MILESTONE, "Fragment", "Frag.", "frg"
body,		e_m, "body", MILESTONE, NULL, NULL, NULL
colophon,	e_m, "colophon", MILESTONE, NULL, NULL, NULL
catchline,	e_m, "catchline", MILESTONE, NULL, NULL, NULL
date,		e_m, "date", MILESTONE, NULL, NULL, NULL
sealings,	e_m, "sealers", MILESTONE, NULL, NULL, NULL
linecount,	e_m, "linecount", MILESTONE, NULL, NULL, NULL
summary,	e_m, "summary", MILESTONE, NULL, NULL, NULL
signature,  e_m, "signature", MILESTONE, NULL, NULL, NULL
witnesses,  e_m, "witnesses", MILESTONE, NULL, NULL, NULL
include,    e_include,"include",MILESTONE, NULL, NULL, NULL
referto,    e_referto,"referto",MILESTONE, NULL, NULL, NULL
div,        e_div, "div", DIVISION, NULL, NULL, NULL
end,        e_div, "end", DIVISION, NULL, NULL, NULL
translation, e_xtr_translation, NULL, TR_TRANS, NULL, NULL, NULL
transtype,  e_xtr_translation, NULL, TR_SWITCH, NULL, NULL, NULL
label,	    e_enum_top, NULL, TR_LABEL, NULL, NULL, NULL
unit,	    e_enum_top, NULL, TR_UNIT,  NULL, NULL, NULL
span,	    e_xh_span,  NULL, TR_SPAN,  NULL, NULL, NULL
note,	    e_xh_div,   NULL, TR_NOTE,  NULL, NULL, NULL
h1x,        e_h,        NULL, H1,       NULL, NULL, NULL
h2x,        e_h,        NULL, H2,       NULL, NULL, NULL
h3x,        e_h,        NULL, H3,       NULL, NULL, NULL
h1,         e_xh_h1,    NULL, TR_H1,    NULL, NULL, NULL
h2,         e_xh_h2,    NULL, TR_H2,    NULL, NULL, NULL
h3,         e_xh_h3,    NULL, TR_H3,    NULL, NULL, NULL
variants,	e_variants, "variants", DIVISION, NULL, NULL, NULL
variant,    e_variant,  "variant", DIVISION, NULL, NULL, NULL
endvariants,e_variants, "endvariants", DIVISION, NULL, NULL, NULL

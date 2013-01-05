%{
#include <string.h>
#include "nonx.h"
%}
struct nonx_token
{
  const char *name;
  enum x_class class;
  enum x_type  type;
  const char *const str;
  int of_flag;
  int pl_flag;
};
%%
at,     x_ignore, x_at,  "at", 0, 0,
of,     x_ignore, x_of,  "of", 0, 0,
most,   x_qual, x_atmost, "at most", 0, 0,
least,  x_qual, x_atleast, "at least", 0, 0,
about,  x_qual, x_atleast, "about", 0, 0,
all,	x_extent, x_all,     "all", 1, 0
rest,	x_extent, x_rest,    "rest", 1, 0
beginning,	x_extent, x_start,   "beginning", 1, 0
start,	x_extent, x_start,   "start", 1, 0
middle, x_extent, x_middle,  "middle", 1, 0
end, 	x_extent, x_end,     "end", 1, 0
literal,x_extent, x_number,   NULL, 1, 0
n,      x_extent, x_number,   "n", 1, 0
some,   x_extent, x_number,   "n", 1, 0
several,x_extent, x_number,   "n", 1, 0
object, x_scope,  x_object,  "object", 0, 0
tablet,	x_scope,  x_object,  "tablet",  0, 0
seal,	x_scope,  x_line,    "seal",  0, 0
docket,	x_scope,  x_dock,    "docket",  0, 0
bulla,	x_scope,  x_object,  "bulla",  0, 0
prism,	x_scope,  x_object,  "prism",  0, 0
fragment,x_scope, x_object,  "fragment",  0, 0
envelope,x_scope, x_object,  "envelope",  0, 0
obverse,x_scope,  x_surface, "obverse", 0, 0
reverse,x_scope,  x_surface, "reverse", 0, 0
left,	x_scope,  x_surface, "left edge", 0, 0
right,	x_scope,  x_surface, "right edge", 0, 0
top,	x_scope,  x_surface, "top edge", 0, 0
bottom,	x_scope,  x_surface, "bottom edge", 0, 0
edge,	x_scope,  x_surface, "edge", 0, 0
surface,x_scope,  x_surface, "surface", 0, 0
sealing,x_scope,  x_line,    "sealing", 0, 0
column,	x_scope,  x_column,  "column", 0, 0
columns,x_scope,  x_column,  "column", 0, 1
case,	x_scope,  x_line,    "case", 0, 0
cases,	x_scope,  x_line,    "case", 0, 1
line,	x_scope,  x_line,    "line", 0, 0
lines,	x_scope,  x_line,    "line", 0, 1
space,	x_scope,  x_space,   "space", 0, 1
missing,x_state,  x_broken,  "missing", 0, 0
broken, x_state,  x_broken,  "missing", 0, 0
lacuna, x_state,  x_broken,  "missing", 0, 0
effaced,x_state,  x_effaced, "effaced", 0, 0
traces, x_state,  x_traces,  "traces", 0, 0
blank,  x_state,  x_blank,   "blank", 0, 0
erased, x_state,  x_blank,   "erased", 0, 0
illegible,x_state,x_illegible,"illegible",0,0
ruling, x_rule,   x_line,    "ruling", 0, 0
single, x_extent, x_single,  "1", 0, 1
double, x_extent, x_double,  "2", 0, 2
triple, x_extent, x_triple,  "3", 0, 3
impression, x_impression, x_impress, "impression", 0,0
image, x_image_class, x_image, "image", 0,0
empty, x_empty_class, x_empty, "empty", 0,0

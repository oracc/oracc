%{
#include <string.h>
#include <stddef.h>
#include "xcl.h"
%}
struct xcl_tok_tab
{
  const char *name;
  enum xcl_node_types node_type;
  int subtype; /* generic b/c each node_type has its own enum */
}
%%
text,	   xcl_node_c, xcl_c_text
discourse, xcl_node_c, xcl_c_discourse
chunk,	   xcl_node_c, xcl_c_chunk
sentence,  xcl_node_c, xcl_c_sentence
clause,	   xcl_node_c, xcl_c_clause
phrase,	   xcl_node_c, xcl_c_phrase
line-var,  xcl_node_c, xcl_c_line_var
word-var,  xcl_node_c, xcl_c_text
line-start,xcl_node_d, xcl_d_line_start
cell-start,xcl_node_d, xcl_d_cell_start
cell-end,  xcl_node_d, xcl_d_cell_end
gloss-start,xcl_node_d, xcl_d_gloss_start
gloss-end, xcl_node_d, xcl_d_gloss_end
break,	   xcl_node_d, xcl_d_break
punct,	   xcl_node_d, xcl_d_punct
surface,   xcl_node_d, xcl_d_surface
nonx, 	   xcl_node_d, xcl_d_nonx

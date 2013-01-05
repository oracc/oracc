#ifndef PF_LIB_H_
#define PF_LIB_H_

#include "matrix.h"
typedef void (*Pf_func)(void);

struct Process_functions
{
  Uchar *output_fn;
  FILE *output_fp;
  Pf_func 
    before_outers,
    each_outer,
    between_outers,
    after_outers,

    each_text,

    before_block,
    after_block,

    before_sources,
    between_sources,
    after_sources,

    before_composite,
    after_composite,
    each_composite_column,
    between_composite_columns,

    before_reconstructed,
    after_reconstructed,
    each_reconstructed_column,
    between_reconstructed_columns,

    before_source,
    after_source,

    each_source_column,
    between_source_columns,

    before_notes,
    each_note,
    after_notes
      ;
};
typedef struct Process_functions Process_functions;

extern Process_functions pf_align, pf_clear, pf_cmp, pf_debug, 
  pf_sf4, pf_tex, pf_vars, pf_expand, pf_mtx;

extern Process_functions *ipf;
extern Outer *iterate_outer;
extern Uchar *iterate_text;
extern Block *iterate_block;
extern Line *iterate_line;
extern Composite_column *iterate_composite_column,
   *iterate_reconstructed_column;
extern Source_column *iterate_source_column;

extern void iterate (Process_functions *pf, List *base);
extern Uchar * print_escaped (Uchar *s);

#endif

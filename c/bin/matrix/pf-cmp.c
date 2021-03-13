#include <psd_base.h>
#include "pf_lib.h"
#include <ctype.h>

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))

static void
cmp_before_outers()
{
}

static void
cmp_each_outer ()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "@composite\n");
}

static void
cmp_between_outers()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "@end composite\n");
}

static void
cmp_after_outers()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "@end composite\n");
}

static void
cmp_each_text()
{
}

static void
cmp_before_block()
{
}

static void
cmp_after_block()
{
}

static void
cmp_before_sources()
{
}

static void
cmp_between_sources()
{
}

static void
cmp_after_sources()
{
}

static void
cmp_before_composite()
{
}

static void
cmp_after_composite()
{
}

static void
cmp_each_composite_column()
{
  if (iterate_block->composite == iterate_block->reconstructed)
    {
      Uchar * best_delim;
      Composite_column *iccp = iterate_composite_column;
      if (iterate_composite_column->done)
	return;
      if (iterate_composite_column->short_line_divider)
	{
	  best_delim = " , ";
	}
      else
	{
	  best_delim = iterate_composite_column->delim;
	  while (iccp->right && !strcmp(iccp->right->text, "0"))
	    {
	      iccp = iccp->right;
	      iccp->done = TRUE;
	      if ('\n' != *best_delim && isspace(*iccp->delim))
		best_delim = iccp->delim;
	    }
	}
      P2("%s%s", iterate_composite_column->text, best_delim);
    }
}

static void
cmp_between_composite_columns()
{
}

static void
cmp_before_reconstructed()
{
}

static void
cmp_after_reconstructed()
{
}

static void
cmp_each_reconstructed_column()
{
  Uchar * best_delim;
  Composite_column *iccp = iterate_reconstructed_column;
  if (iterate_reconstructed_column->done)
    return;
  best_delim = iterate_reconstructed_column->delim;
  while (iccp->right && !strcmp(iccp->right->text, "0"))
    {
      iccp = iccp->right;
      iccp->done = TRUE;
      if ('\n' != *best_delim && isspace(*iccp->delim))
	best_delim = iccp->delim;
    }
  P2("%s%s", iterate_reconstructed_column->text, best_delim);
}

static void
cmp_between_reconstructed_columns()
{
}

static void
cmp_before_source()
{
}

static void
cmp_after_source()
{
}

static void
cmp_each_source_column()
{
}

static void
cmp_between_source_columns()
{
}

static void
cmp_before_notes()
{
}

static void
cmp_each_note()
{
}

static void
cmp_after_notes()
{
}

Process_functions pf_cmp = {
NULL, NULL,
cmp_before_outers,
cmp_each_outer,
cmp_between_outers,
cmp_after_outers,
cmp_each_text,
cmp_before_block,
cmp_after_block,
cmp_before_sources,
cmp_between_sources,
cmp_after_sources,
cmp_before_composite,
cmp_after_composite,
cmp_each_composite_column,
cmp_between_composite_columns,
cmp_before_reconstructed,
cmp_after_reconstructed,
cmp_each_reconstructed_column,
cmp_between_reconstructed_columns,
cmp_before_source,
cmp_after_source,
cmp_each_source_column,
cmp_between_source_columns,
cmp_before_notes,
cmp_each_note,
cmp_after_notes,
};

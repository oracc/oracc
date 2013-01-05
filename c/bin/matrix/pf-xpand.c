#include <psd_base.h>
#include "pf_lib.h"

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))

static void
expand_before_outers()
{
}

static void
expand_each_outer ()
{
}

static void
expand_between_outers()
{
}

static void
expand_after_outers()
{
}

static void
expand_each_text()
{
}

static void
expand_before_block()
{
}

static void
expand_after_block()
{
}

static void
expand_before_sources()
{
}

static void
expand_between_sources()
{
}

static void
expand_after_sources()
{
}

static void
expand_before_composite()
{
  list_exec (iterate_line->columns, phantom_prepare_phantoms);
}

static void
expand_after_composite()
{
}

static void
expand_each_composite_column()
{
  extern unsigned char *expand_composite_complex(Composite_column *sp);
  if (iterate_composite_column->complex == iterate_composite_column)
    {
      Uchar *tmp = expand_composite_complex (iterate_composite_column);
      if (tmp)
	{
	  /*fprintf(stderr,"Expand returned: %s\n",tmp);*/
	  iterate_composite_column->complex_text = xstrdup (tmp);
	}
    }
}

static void
expand_between_composite_columns()
{
}

static void
expand_before_reconstructed()
{
}

static void
expand_after_reconstructed()
{
}

static void
expand_each_reconstructed_column()
{
}

static void
expand_between_reconstructed_columns()
{
}

static void
expand_before_source()
{
}

static void
expand_after_source()
{
}

static void
expand_each_source_column()
{
  if (iterate_source_column->complex == iterate_source_column
      && !iterate_source_column->filler_flag)
    {
      Uchar *tmp = expand_complex (iterate_source_column);
      if (tmp)
	{
	  /*fprintf(stderr,"Expand returned: %s\n",tmp);*/
	  iterate_source_column->complex_text = xstrdup (tmp);
	}
    }
}

static void
expand_between_source_columns()
{
}

static void
expand_before_notes()
{
}

static void
expand_each_note()
{
}

static void
expand_after_notes()
{
}

Process_functions pf_expand = {
NULL, NULL,
expand_before_outers,
expand_each_outer,
expand_between_outers,
expand_after_outers,
expand_each_text,
expand_before_block,
expand_after_block,
expand_before_sources,
expand_between_sources,
expand_after_sources,
expand_before_composite,
expand_after_composite,
expand_each_composite_column,
expand_between_composite_columns,
expand_before_reconstructed,
expand_after_reconstructed,
expand_each_reconstructed_column,
expand_between_reconstructed_columns,
expand_before_source,
expand_after_source,
expand_each_source_column,
expand_between_source_columns,
expand_before_notes,
expand_each_note,
expand_after_notes,
};

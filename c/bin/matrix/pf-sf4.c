#include <psd_base.h>
#include "variants.h"
#include "pf_lib.h"

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))

static void
sf4_before_outers()
{
}

static void
sf4_each_outer ()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "\\begin{composite}\n");
}

static void
sf4_between_outers()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "\\end{composite}\n");
}

static void
sf4_after_outers()
{
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "\\end{composite}\n");
}

static void
sf4_each_text()
{
}

static void
sf4_before_block()
{
}

static void
sf4_after_block()
{
}

static void
sf4_before_sources()
{
}

static void
sf4_between_sources()
{
}

static void
sf4_after_sources()
{
}

static void
sf4_before_composite()
{
}

static void
sf4_after_composite()
{
}
static Uchar *
sf4_expand_var (Variant *vp)
{
  if ('.' == *vp->sp->text || '-' == *vp->sp->text)
    return NULL;
  else
    return vp->sp->text;
}
static
void sf4_print_var (void *vp)
{
  P1("<vs>%s",sf4_expand_var(vp));
}
static void
sf4_each_composite_column()
{
  if (iterate_block->composite == iterate_block->reconstructed)
    {
      List *vars = vars_lookup2(iterate_line->name, iterate_composite_column->index);
      if (list_len(vars))
	{
	  P1("<var>%s", iterate_composite_column->text);
	  list_exec (vars, sf4_print_var);
	  P1("</var>%s", iterate_composite_column->delim);
	}
      else
	{
	  P2("%s%s", iterate_composite_column->text, 
	     iterate_composite_column->delim);
	}
    }
}

static void
sf4_between_composite_columns()
{
}

static void
sf4_before_reconstructed()
{
}

static void
sf4_after_reconstructed()
{
}

static void
sf4_each_reconstructed_column()
{
  List *vars = vars_lookup2(iterate_line->name, iterate_reconstructed_column->index);
  if (list_len(vars))
    {
      P1("<var>%s", iterate_reconstructed_column->text);
      list_exec (vars, sf4_print_var);
      P1("</var>%s", iterate_reconstructed_column->delim);
    }
  else
    {
      P2("%s%s", iterate_reconstructed_column->text, 
	 iterate_reconstructed_column->delim);
    }
}

static void
sf4_between_reconstructed_columns()
{
}

static void
sf4_before_source()
{
}

static void
sf4_after_source()
{
}

static void
sf4_each_source_column()
{
}

static void
sf4_between_source_columns()
{
}

static void
sf4_before_notes()
{
}

static void
sf4_each_note()
{
}

static void
sf4_after_notes()
{
}

Process_functions pf_sf4 = {
NULL, NULL,
sf4_before_outers,
sf4_each_outer,
sf4_between_outers,
sf4_after_outers,
sf4_each_text,
sf4_before_block,
sf4_after_block,
sf4_before_sources,
sf4_between_sources,
sf4_after_sources,
sf4_before_composite,
sf4_after_composite,
sf4_each_composite_column,
sf4_between_composite_columns,
sf4_before_reconstructed,
sf4_after_reconstructed,
sf4_each_reconstructed_column,
sf4_between_reconstructed_columns,
sf4_before_source,
sf4_after_source,
sf4_each_source_column,
sf4_between_source_columns,
sf4_before_notes,
sf4_each_note,
sf4_after_notes,
};

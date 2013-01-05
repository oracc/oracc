#include <psd_base.h>
#include "pf_lib.h"
#include "variants.h"

int indent = 0;
#define t_or_f(c) ((c)==TRUE?"t":"f")

static void
do_indent ()
{
  int i;
  for (i = 0; i < indent; ++i)
    fputc (' ', ipf->output_fp);
}

static void
debug_before_outers()
{
  indent += 2;
}

static void
debug_each_outer ()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "<outer %d>\n", iterate_outer->index);
  indent += 2;
}

static void
debug_between_outers()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</outer %d>\n", iterate_outer->index);
  indent += 2;
}

static void
debug_after_outers()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</outer %d>\n", iterate_outer->index);
}

static void
debug_each_text()
{
}

static void
debug_before_block()
{
  do_indent ();
  fprintf (ipf->output_fp, "<block %d>\n", iterate_block->index);
  indent += 2;
}

static void
debug_after_block()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</block %d>\n", iterate_block->index);
}

static void
debug_before_sources()
{
  do_indent ();
  fprintf (ipf->output_fp, "<before_sources>\n");
}

static void
debug_between_sources()
{
  do_indent ();
  fprintf (ipf->output_fp, "<between_sources>\n");
}

static void
debug_after_sources()
{
  do_indent ();
  fprintf (ipf->output_fp, "<after_sources>\n");
}

static void
debug_before_composite()
{
  do_indent ();
  fprintf (ipf->output_fp, "<composite %d name=%s>\n", 
	   iterate_line->index,
	   iterate_line->name);
  indent += 2;
}

static void
debug_after_composite()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</composite %d>\n", iterate_line->index);
}

static void
debug_print_variant (void *v)
{
  Variant *vp = v;
  do_indent ();
  fprintf (ipf->output_fp, "<variant %s has \"%s\" for \"%s\">\n",
	   vp->siglum_name,
	   vars_get_expanded_variant_text (vp),
	   vars_get_main_text (vp));
}

static void
debug_each_composite_column()
{
  List *vlist;

  do_indent ();
  fprintf (ipf->output_fp, 
	   "<comp_col %d text=\"%s\" delim=\"%s\" viswidth=%d maxwidth=%d zero=%s space=%s\n", 
	   iterate_composite_column->index,
	   iterate_composite_column->text,
	   print_escaped(iterate_composite_column->delim),
	   iterate_composite_column->visible_width,
	   iterate_composite_column->maxwidth,
	   t_or_f(iterate_composite_column->zero_flag),
	   t_or_f(iterate_composite_column->space_flag));
  do_indent ();
  fprintf (ipf->output_fp, 
	   "\tcomplex_index=%d complex_columns=%d complex_has_variant=%s s_l_d=%s phant=%s>\n",
	   iterate_composite_column->complex_index,
	   iterate_composite_column->complex_columns,
	   t_or_f(iterate_composite_column->complex->complex_has_variant),
	   t_or_f(iterate_composite_column->short_line_divider),
	   t_or_f(iterate_composite_column->needs_phantom));
  do_indent ();
  fprintf (ipf->output_fp, "\tcomplex = %p\n", iterate_composite_column);
  vlist = vars_lookup2(iterate_line->name, iterate_composite_column->index);
  if (list_len(vlist))
    {
      indent += 2;
      vars_normalize_length(vlist);
      list_exec (vlist, debug_print_variant);
      indent -= 2;
    }
}

static void
debug_between_composite_columns()
{
}

static void
debug_before_reconstructed()
{
  do_indent ();
  fprintf (ipf->output_fp, "<reconstructed %d name=%s>\n", 
	   iterate_line->index,
	   iterate_line->name);
  indent += 2;
}

static void
debug_after_reconstructed()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</reconstructed %d>\n", iterate_line->index);
}

static void
debug_each_reconstructed_column()
{
  do_indent ();
  fprintf (ipf->output_fp, "<recons_col %d text=\"%s\" delim=\"%s\" maxwidth=%d\n", 
	   iterate_reconstructed_column->index,
	   iterate_reconstructed_column->text,
	   print_escaped(iterate_reconstructed_column->delim),
	   iterate_reconstructed_column->maxwidth);
  do_indent ();
  fprintf (ipf->output_fp, "\tcomplex_index=%d complex_columns=%d zero=%s space=%s phant=%s>\n",
	   iterate_reconstructed_column->complex_index,
	   iterate_reconstructed_column->complex_columns,
	   t_or_f(iterate_reconstructed_column->zero_flag),
	   t_or_f(iterate_reconstructed_column->space_flag),
	   t_or_f(iterate_reconstructed_column->needs_phantom));
}

static void
debug_between_reconstructed_columns()
{
}
static void
debug_before_source()
{
  do_indent ();
  fprintf (ipf->output_fp, "<source %d name=%s>\n", 
	   iterate_line->index,
	   iterate_line->name);
  indent += 2;
}

static void
debug_after_source()
{
  indent -= 2;
  do_indent ();
  fprintf (ipf->output_fp, "</source %d>\n", iterate_line->index);
}

static void
debug_each_source_column()
{
  do_indent ();
  fprintf (ipf->output_fp, "<s_col %d cindex=%d text=\"%s\" ",
	   iterate_source_column->index,
	   iterate_source_column->cindex,
	   iterate_source_column->filler_flag ? (Uchar*)"<filler>" : iterate_source_column->text);
  if (!iterate_source_column->index && *iterate_line->altsig)
    fprintf (ipf->output_fp, "(aka `%s') ", iterate_line->altsig);
  fprintf (ipf->output_fp, "cols=%d viswidth=%d zero=%s space=%s phant=%s", 
	   iterate_source_column->columns_occupied,
	   iterate_source_column->visible_width,
	   t_or_f(iterate_composite_column->zero_flag),
	   t_or_f(iterate_composite_column->space_flag),
	   t_or_f(iterate_source_column->needs_phantom));
  fprintf (ipf->output_fp, "\n");
  do_indent ();
  fprintf (ipf->output_fp, "\tcomposite = %p; composite_end = %p\n", 
	   iterate_source_column->composite,
	   iterate_source_column->composite_end);
  do_indent ();
  fprintf (ipf->output_fp, "  complex = %p ", iterate_source_column->complex);
  if (iterate_source_column->complex_text)
    {
      fprintf (ipf->output_fp, "complex_has_variant=%s complex_text = `%s'>\n", 
	       t_or_f(iterate_source_column->complex_has_variant),
	       print_escaped(iterate_source_column->complex_text));
    }
  else
    fprintf (ipf->output_fp, ">\n");
}

static void
debug_between_source_columns()
{
}

static void
debug_before_notes()
{
}

static void
debug_each_note()
{
}

static void
debug_after_notes()
{
}

Process_functions pf_debug = {
NULL, NULL,
debug_before_outers,
debug_each_outer,
debug_between_outers,
debug_after_outers,
debug_each_text,
debug_before_block,
debug_after_block,
debug_before_sources,
debug_between_sources,
debug_after_sources,
debug_before_composite,
debug_after_composite,
debug_each_composite_column,
debug_between_composite_columns,
debug_before_reconstructed,
debug_after_reconstructed,
debug_each_reconstructed_column,
debug_between_reconstructed_columns,
debug_before_source,
debug_after_source,
debug_each_source_column,
debug_between_source_columns,
debug_before_notes,
debug_each_note,
debug_after_notes,
};

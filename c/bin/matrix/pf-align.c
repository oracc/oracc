#include <psd_base.h>
#include "pf_lib.h"
#include "warning.h"

static int n_notes;

static void
pad(int i)
{
  if (i > 0)
    while (i--)
      fputc (' ', ipf->output_fp);
}

static void
align_before_outers()
{
}

static void
align_each_outer()
{
#if 0
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "<matrix>\n");
#endif
}

static void
align_between_outers()
{
#if 0
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "</matrix>\n");
#endif
}

static void
align_after_outers()
{
#if 0
  if (iterate_outer->type == OUTER_MATRIX)
    fprintf (ipf->output_fp, "</matrix>\n");
#endif
}

static void
align_each_text()
{
  fprintf (ipf->output_fp, "%s", iterate_text);
}

static void
align_before_block()
{
}

static void
align_after_block()
{
  fprintf (ipf->output_fp, "\n");
}

static void
align_before_sources()
{
}

static void
align_between_sources()
{
}

static void
align_after_sources()
{
}

static void
align_before_composite()
{
}

static void
align_after_composite()
{
  fprintf (ipf->output_fp, "\n");
}

static void
align_each_composite_column()
{
  if (0 == iterate_composite_column->index)
    {
      int i = global_col0_maxwidth - iterate_composite_column->visible_width;
      fprintf (ipf->output_fp, "%s", iterate_composite_column->text);
      pad(i);
    }
  else
    {
      int i = iterate_composite_column->maxwidth 
	- iterate_composite_column->visible_width;
      fprintf (ipf->output_fp, "%s", iterate_composite_column->text);
      if (iterate_composite_column->short_line_divider)
	{
	  fputs(" / ",ipf->output_fp);
	  i -= 2;
	}
      else
	if ('\n' != *iterate_composite_column->delim)
	  {
	    fprintf (ipf->output_fp, "%s", iterate_composite_column->delim);
#if 0
	    if (*iterate_composite_column->delim)
	      --i;
#endif
	  }
      pad(i);
    }
}

static void
align_between_composite_columns()
{
}

static void
align_before_reconstructed()
{
}

static void
align_after_reconstructed()
{
  fprintf (ipf->output_fp, "\n");
}

static void
align_each_reconstructed_column()
{
#if 0
  if (0 == iterate_reconstructed_column->index)
    {
      fprintf (ipf->output_fp, "%s\t", iterate_reconstructed_column->text);
    }
  else
#endif
    {
      int i = iterate_reconstructed_column->maxwidth 
	- iterate_reconstructed_column->visible_width;
      fprintf (ipf->output_fp, "%s", iterate_reconstructed_column->text);
      if ('\n' != *iterate_reconstructed_column->delim)
	{
	  fprintf (ipf->output_fp, "%s", iterate_reconstructed_column->delim);
	}
      pad(i);
    }
}

static void
align_between_reconstructed_columns()
{
}

static void
align_before_source()
{
}

static void
align_after_source()
{
  fprintf (ipf->output_fp, "\n");
}

static char *
align_make_explicit (Source_column *sp)
{
  static char buf[4];
  sprintf (buf, "&%d", sp->explicit_column_count);
  return buf;
}

static char *
align_make_delim (Source_column *sp)
{
  static char buf[3];
  sprintf (buf, "\\%s", sp->delim);
  return buf;
}

static void
align_each_source_column()
{
  if (iterate_source_column->filler_flag)
    return;
  if (iterate_source_column->done)
    return;
  if (iterate_source_column->apocopation_flag)
    {
      if (iterate_source_column->index > 1 
	  || (iterate_source_column->text && *iterate_source_column->text))
	fprintf (ipf->output_fp, "%c%s", iterate_source_column->apocopation_flag, 
		 iterate_source_column->text);
    }
  else if ('\0' == *iterate_source_column->text)
    return;
  else if (0 == iterate_source_column->index)
    {
      int i = global_col0_maxwidth;
      fprintf (ipf->output_fp, "%s", iterate_source_column->text);
      i -= iterate_source_column->visible_width + 1;
      if (*iterate_line->altsig)
	{
	  fprintf (ipf->output_fp, "(%s)", iterate_line->altsig);
	}
      if (*iterate_line->tabloc->compressed)
	{
	  fprintf (ipf->output_fp, ",%s", iterate_line->tabloc->compressed);
	  /* i -= strlen (iterate_line->tabloc->compressed) + 1; */
	}
      fprintf (ipf->output_fp, ":");
      pad(i);
    }
  else
    {
      Source_column *sp = NULL;
      int i;
      if (0 == iterate_source_column->columns_occupied)
	return;
      else if (FALSE == iterate_source_column->text_entry_flag
	       || 1 == iterate_source_column->columns_occupied)
	i = (iterate_source_column->composite->maxwidth 
	     + (2*iterate_source_column->composite->short_line_divider))
	  - iterate_source_column->visible_width;
      else
	{
	  /* total the maxwidth of all the columns this text entry
	     is supposed to take up, and calculate the padding from
	     that */
	  Composite_column *cp;
	  Source_column *sp2;
	  int w;

	  for (sp = iterate_source_column; sp; sp = sp->right)
	    if (sp->right == NULL
		|| sp->right->complex != iterate_source_column->complex)
	      break;
	  if (sp && sp->columns_occupied > 0)
	    sp = NULL;
	  for (i = iterate_source_column->columns_occupied /*+ (sp ? 1 : 0)*/,
		 w = 0, 
		 cp = iterate_source_column->composite,
		 sp2 = iterate_source_column; 
	       i > 0 && cp && sp2; 
	       --i, cp = cp->right, sp2 = sp2->right)
	    {
	      w += cp->maxwidth;
	      if (i > 1 
		  && (*cp->delim 
		      /* || (sp2->text_entry_flag && sp2->right && sp2->right->text_entry_flag) */ ))
		++w;
	    }
	  i = w - iterate_source_column->visible_width;
	  if (i < 0)
	    {
	      /* we don't need to check this all the time, but
		 essentially fault to this code when it could matter;
		 increment i appropriately if the source column entry
		 ends with &<DIGITS> and recompute i; only warn if it is
		 still too wide */
	      if (iterate_source_column->explicit_ncols)
		i = w - visible_width(iterate_source_column->text);
	      if (i < 0)
		vwarning2 (iterate_source_column->line->file,
			   iterate_source_column->line->linenum,
			 "text entry beginning column %d is wider than its columns (w=%d; i=%d; text=%s)",
			 iterate_source_column->index,w,i,iterate_source_column->text);
	    }
	}
      fprintf (ipf->output_fp, "%s%s%s%s", 
	       iterate_source_column->align_override_flag ? "@" : "",
	       iterate_source_column->text,
	       iterate_source_column->explicit_ncols 
	       ? align_make_explicit(iterate_source_column)
	       : "",
	       *iterate_source_column->delim 
	       ? align_make_delim(iterate_source_column)
	       : "");

#if 0
      /* &-width is handled earlier now */
      /* we are never going to have ba&100 ...*/
      if (iterate_source_column->explicit_ncols)
	i -= 2 + (iterate_source_column->columns_occupied > 10);
#endif

      if (!sp)
	sp = iterate_source_column->right;
      if (sp != NULL)
	{
	  if (sp->columns_occupied == 0 && !sp->apocopation_flag)
	    {
	      while (1)
		{
		  if ('/' == *sp->text || ';' == *sp->text || '%' == *sp->text)
		    {
		      fprintf (ipf->output_fp, " ");
#if 0
		      if (iterate_source_column->composite->short_line_divider)
			--i;
#endif
		    }
		  else if (iterate_source_column->composite->short_line_divider)
		    --i;

		  fprintf (ipf->output_fp, "3%s", sp->text);
		  /* be sure to print multiple insertions */
		  if (sp->right && !sp->right->columns_occupied)
		    sp = sp->right;
		  else
		    {
		      if (*sp->composite->delim)
			fputc (' ', ipf->output_fp);
		      break;
		    }
		}
	    }
	  else
	    {
	      if (*iterate_source_column->composite->delim
		  || (iterate_source_column->text_entry_flag
		      && iterate_source_column->right
		      && iterate_source_column->right->text_entry_flag))
		fputc (' ', ipf->output_fp);
	    }
	  if (i > 0)
	    {
	      while (i--)
		fputc (' ', ipf->output_fp);
	    }
	  else
	    {
	      if (sp->text_entry_flag
		  && *sp->composite->delim == '\0')
		vwarning2 (sp->line->file,
			   sp->line->linenum,
			   "text entry beginning column %d may lack following space",
			   sp->index);
	    }
	}
    }
}

static void
align_between_source_columns()
{
}

static void
align_before_notes()
{
  n_notes = 0;
  fputc ('\n', ipf->output_fp);
}

static void
align_each_note()
{
  if (n_notes && !strncmp(iterate_text,"#note:",6))
    fputc ('\n', ipf->output_fp);
  fprintf (ipf->output_fp, "%s", iterate_text);
  ++n_notes;
}

static void
align_after_notes()
{
  fputc ('\n', ipf->output_fp);
}

Process_functions pf_align = {
NULL, NULL,
align_before_outers,
align_each_outer,
align_between_outers,
align_after_outers,
align_each_text,
align_before_block,
align_after_block,
align_before_sources,
align_between_sources,
align_after_sources,
align_before_composite,
align_after_composite,
align_each_composite_column,
align_between_composite_columns,
align_before_reconstructed,
align_after_reconstructed,
align_each_reconstructed_column,
align_between_reconstructed_columns,
align_before_source,
align_after_source,
align_each_source_column,
align_between_source_columns,
align_before_notes,
align_each_note,
align_after_notes,
};

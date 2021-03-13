#include <psd_base.h>
#include "pf_lib.h"
#include "variants.h"
#include "warning.h"

/* Module to build a map of variants throughout the score; the idea is
   that various other modules that actually do stuff with variants, 
   like print them, can just use the information constructed by this
   module.

   The basic unit is the Variant structure, which gives the text used
   as the key and the variants to it in a single source.  Each block
   has a list of columns containing variants; each column has a list
   of the variants to its text.  Iterating over the score is a matter
   of following the head of the list of blocks; random access is
   enabled by a hash table which can be addressed by `block-name',
   `block-name:column' or `block-name:column:siglum'.

   Building all this is rather simple.  We just let the iterate module
   carry us over the score, setting static variables to current block,
   column and siglum, and filling in anything that is not broken or
   present (it might be desirable to mark breaks in the future, but I
   think this is really white noise).
*/

#define xmalloc malloc

Boolean pf_vars_collapse_variants = FALSE;
static Uchar *curr_block_name = NULL;
static Uchar *curr_sig_name = NULL;

static void
vars_before_outers()
{
}

static void
vars_each_outer ()
{
  if (iterate_outer->type == OUTER_MATRIX)
    vars_init ();
}

static void
vars_between_outers()
{
}

static void
vars_after_outers()
{
}

static void
vars_each_text()
{
}

static void
vars_before_block()
{
}

static void
vars_after_block()
{
}

static void
vars_before_sources()
{
}

static void
vars_between_sources()
{
}

static void
vars_after_sources()
{
}

static void
vars_before_composite()
{
}

static void
vars_after_composite()
{
}

static void
vars_each_composite_column()
{
}

static void
vars_between_composite_columns()
{
  if (0 == iterate_composite_column->index)
    curr_block_name = iterate_line->name;
}

static void
vars_before_reconstructed()
{
}

static void
vars_after_reconstructed()
{
}

static void
vars_each_reconstructed_column()
{
}

static void
vars_between_reconstructed_columns()
{
}

static void
vars_before_source()
{
}

static void
vars_after_source()
{
}

static Variant *
new_variant ()
{
  Variant *tmp = xmalloc (sizeof (Variant));

  list_add (variants_list, tmp);
  tmp->block_name = curr_block_name;
  tmp->siglum_name = curr_sig_name;
  tmp->begin = iterate_source_column->index;
  tmp->end = tmp->begin + iterate_source_column->columns_occupied;
  tmp->sp = tmp->sp_end = iterate_source_column;
  tmp->next = NULL;
  tmp->prev = vars_assq3 (tmp);
  if (!tmp->prev)
    vars_register (tmp);
  return tmp;
}

static void
force_complex_end_phantom (Source_column *sp)
{
  Composite_column *cp = sp->composite;

  while (1)
    {
      if (!cp->right || cp->complex_index != cp->right->complex_index)
	break;
      else
	cp = cp->right;
    }
  if (!cp->needs_phantom)
    {
      Source_column *sp2;
      cp->needs_phantom = TRUE;
      for (sp2 = cp->down; sp2; sp2 = sp2->down)
	{
	  if (sp2->columns_occupied || sp2->filler_flag)
	    sp2->needs_phantom = TRUE;
	  else
	    {
	      Source_column *sp3;
	      for (sp3 = sp2; sp3->left && 0 == sp3->columns_occupied; sp3 = sp3->left)
		;
	      if (sp3->index)
		sp3->needs_phantom = TRUE;
	      else
		{
		  vwarning2((const char *)iterate_line->file,iterate_line->linenum,"%s",
			  "unable to migrate phantom leftwards");
		  sp2->needs_phantom = TRUE;
		}
	    }
	}
    }
}

static void
vars_each_source_column()
{
  if (0 == iterate_source_column->index)
    {
      curr_sig_name = iterate_line->name;
    }
  else
    {
      if (((*iterate_source_column->text == '-'
	    && !iterate_source_column->composite->zero_flag)
	   || iterate_source_column->text_entry_flag))
#if 0
	  && (!pf_vars_collapse_variants
	      || (!iterate_source_column->left
		  || (*iterate_source_column->left->text != '-'
		      && !iterate_source_column->left->text_entry_flag))))
#endif
	{
	  Variant *vp = new_variant();
	  if (iterate_source_column->complex && !iterate_source_column->complex->done)
	    {
	      if (iterate_source_column->text_entry_flag
		  || expand_needs_expansion(iterate_source_column))
		{
		  if (iterate_source_column->complex)
		    iterate_source_column->complex->composite->complex->complex_has_variant
		      = iterate_source_column->complex->complex_has_variant = TRUE;
		  else
		    vwarning2 ((const char *)iterate_line->file, iterate_line->linenum, "%s",
			     "attempt to dereference null complex pointer");
		  force_complex_end_phantom (iterate_source_column);
		}
	      iterate_source_column->complex->done = TRUE;
	    }
	  if (pf_vars_collapse_variants)
	    {
	      Source_column *sp;
	      for (sp = iterate_source_column; sp; sp = sp->right)
		if (!sp->right || (*sp->right->text != '-' && !sp->right->text_entry_flag))
		  break;
	      if (sp && sp != vp->sp)
		{
		  vp->end = sp->index + sp->columns_occupied;
		  vp->sp_end = sp;
		}
	    }
	}
    }
}

static void
vars_between_source_columns()
{
}

static void
vars_before_notes()
{
}

static void
vars_each_note()
{
}

static void
vars_after_notes()
{
}

Process_functions pf_vars = {
NULL, NULL,
vars_before_outers,
vars_each_outer,
vars_between_outers,
vars_after_outers,
vars_each_text,
vars_before_block,
vars_after_block,
vars_before_sources,
vars_between_sources,
vars_after_sources,
vars_before_composite,
vars_after_composite,
vars_each_composite_column,
vars_between_composite_columns,
vars_before_reconstructed,
vars_after_reconstructed,
vars_each_reconstructed_column,
vars_between_reconstructed_columns,
vars_before_source,
vars_after_source,
vars_each_source_column,
vars_between_source_columns,
vars_before_notes,
vars_each_note,
vars_after_notes,
};

#include <psd_base.h>
#include "pf_lib.h"

#define xmalloc  malloc
#define xrealloc realloc

Outer *iterate_outer;
Uchar *iterate_text;
Block *iterate_block;
Line *iterate_line;
Composite_column *iterate_composite_column,
   *iterate_reconstructed_column;
Source_column *iterate_source_column;

Process_functions *ipf;

static void process_block (void *vp);
static void process_composite (void);
static void process_composite_columns (void *vp);
static void process_reconstructed (void);
static void process_reconstructed_columns (void *vp);
static void process_note (void *vp);
static void process_outer (void *vp);
static void process_source (void *vp);
static void process_source_columns (void *vp);
static void process_text (void *vp);

void
iterate (Process_functions *pf, List *base)
{
  ipf = pf;
  ipf->output_fn = output_fn;
  if ('\0' == *output_fn || !strcmp(output_fn, "-"))
    {
      ipf->output_fp = stdout;
      ipf->output_fn = "<stdout>";
    }
  else
    {
      ipf->output_fp = xfopen (output_fn, "w");
    }
  pf->before_outers ();
  list_exec (base, process_outer);
  pf->after_outers ();
  if (ipf->output_fp != stdout)
    xfclose (ipf->output_fn, ipf->output_fp);
}

static void
process_outer (void *vp)
{
  iterate_outer = vp;
  ipf->each_outer ();
  if (OUTER_TEXT == iterate_outer->type)
    list_exec (iterate_outer->stuff, process_text);
  else if (OUTER_MATRIX == iterate_outer->type)
    {
      list_exec (iterate_outer->stuff, process_block);
    }
  if (iterate_outer->parent->next != NULL)
    ipf->between_outers ();
}

static void
process_text (void *vp)
{
  iterate_text = vp;
  ipf->each_text ();
}

static void
process_block (void *vp)
{
  iterate_block = vp;
  ipf->before_block ();

  process_composite ();

  ipf->before_sources ();
  if (do_sort_sources)
    {
      Line **ssp, **sorted_sources = sort_sources (iterate_block->sources);
      for (ssp = sorted_sources; *ssp; ++ssp)
	process_source (*ssp);
    }
  else
    list_exec (iterate_block->sources, process_source);
  ipf->after_sources ();

  ipf->after_block ();
}

static void
process_composite ()
{
  iterate_line = iterate_block->composite;
  ipf->before_composite ();
  if (iterate_block->composite != iterate_block->reconstructed)
    process_reconstructed ();
  list_exec (iterate_line->columns, process_composite_columns);
  ipf->after_composite ();

  if (list_len (iterate_line->notes))
    {
      ipf->before_notes ();
      list_exec (iterate_line->notes, process_note);
      ipf->after_notes ();
    }
}

static void
process_composite_columns (void *vp)
{
  iterate_composite_column = vp;
  ipf->each_composite_column ();
  if (iterate_composite_column->right != NULL)
    ipf->between_composite_columns ();
}

static
void process_reconstructed ()
{
  Line *saved_line = iterate_line;
  iterate_line = iterate_block->reconstructed;
  ipf->before_reconstructed ();
  list_exec (iterate_line->columns, process_reconstructed_columns);
  ipf->after_reconstructed ();
  if (list_len (iterate_line->notes))
    {
      ipf->before_notes ();
      list_exec (iterate_line->notes, process_note);
      ipf->after_notes ();
    }
  iterate_line = saved_line;
}

static void
process_reconstructed_columns (void *vp)
{
  iterate_reconstructed_column = vp;
  ipf->each_reconstructed_column ();
  if (iterate_reconstructed_column->right != NULL)
    ipf->between_reconstructed_columns ();
}

static void
process_source (void *vp)
{
  iterate_line = vp;

  if (do_minimal && *iterate_line->name == ':')
    return;

  ipf->before_source ();
  list_exec (iterate_line->columns, process_source_columns);
  ipf->after_source ();
  if (list_len (iterate_line->notes))
    {
      ipf->before_notes ();
      list_exec (iterate_line->notes, process_note);
      ipf->after_notes ();
    }
  if (iterate_line->parent->next != NULL)
    ipf->between_sources ();
}

static void
process_source_columns (void *vp)
{
  iterate_source_column = vp;
  ipf->each_source_column ();
  if (iterate_source_column->right != NULL)
    ipf->between_source_columns ();
}

static Uchar *note_text = NULL;
static int note_text_len = 0;

static void
aggregate_note (Uchar *txt)
{
  if (note_text_len == 0)
    {
      note_text = xmalloc (note_text_len = 8192);
    }
  else if (strlen(note_text) + strlen(txt) + 2 > note_text_len)
    {
      if (note_text_len)
	note_text = xrealloc (note_text, 2*note_text_len);
    }
  strcat (note_text, txt);
  strcat (note_text, "\n");
}

static void
clear_aggregate_note ()
{
  *note_text = 0;
}

static void
process_note (void *vp)
{
#if 1
  iterate_text = vp;
  if (*iterate_text)
    ipf->each_note ();
#else
  if (*(Uchar*)vp)
    aggregate_note (vp);
  else
    {
      iterate_text = note_text;
      ipf->each_note ();
      clear_aggregate_note ();
    }
#endif
}

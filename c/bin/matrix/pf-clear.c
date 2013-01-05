#include <psd_base.h>
#include "pf_lib.h"

static void
clear_before_outers()
{
}

static void
clear_each_outer ()
{
}

static void
clear_between_outers()
{
}

static void
clear_after_outers()
{
}

static void
clear_each_text()
{
}

static void
clear_before_block()
{
}

static void
clear_after_block()
{
}

static void
clear_before_sources()
{
}

static void
clear_between_sources()
{
}

static void
clear_after_sources()
{
}

static void
clear_before_composite()
{
}

static void
clear_after_composite()
{
}

static void
clear_each_composite_column()
{
  iterate_composite_column->done = FALSE;
}

static void
clear_between_composite_columns()
{
}

static void
clear_before_reconstructed()
{
}

static void
clear_after_reconstructed()
{
}

static void
clear_each_reconstructed_column()
{
  iterate_reconstructed_column->done = FALSE;
}

static void
clear_between_reconstructed_columns()
{
}

static void
clear_before_source()
{
}

static void
clear_after_source()
{
}

static void
clear_each_source_column()
{
  iterate_source_column->done = FALSE;
}

static void
clear_between_source_columns()
{
}

static void
clear_before_notes()
{
}

static void
clear_each_note()
{
}

static void
clear_after_notes()
{
}

Process_functions pf_clear = {
NULL, NULL,
clear_before_outers,
clear_each_outer,
clear_between_outers,
clear_after_outers,
clear_each_text,
clear_before_block,
clear_after_block,
clear_before_sources,
clear_between_sources,
clear_after_sources,
clear_before_composite,
clear_after_composite,
clear_each_composite_column,
clear_between_composite_columns,
clear_before_reconstructed,
clear_after_reconstructed,
clear_each_reconstructed_column,
clear_between_reconstructed_columns,
clear_before_source,
clear_after_source,
clear_each_source_column,
clear_between_source_columns,
clear_before_notes,
clear_each_note,
clear_after_notes,
};

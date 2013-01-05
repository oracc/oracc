#include <psd_base.h>
#include "pf_lib.h"

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))

static void
skel_before_outers()
{
}

static void
skel_each_outer ()
{
}

static void
skel_between_outers()
{
}

static void
skel_after_outers()
{
}

static void
skel_each_text()
{
}

static void
skel_before_block()
{
}

static void
skel_after_block()
{
}

static void
skel_before_sources()
{
}

static void
skel_between_sources()
{
}

static void
skel_after_sources()
{
}

static void
skel_before_composite()
{
}

static void
skel_after_composite()
{
}

static void
skel_each_composite_column()
{
}

static void
skel_between_composite_columns()
{
}

static void
skel_before_reconstructed()
{
}

static void
skel_after_reconstructed()
{
}

static void
skel_each_reconstructed_column()
{
}

static void
skel_between_reconstructed_columns()
{
}

static void
skel_before_source()
{
}

static void
skel_after_source()
{
}

static void
skel_each_source_column()
{
}

static void
skel_between_source_columns()
{
}

static void
skel_before_notes()
{
}

static void
skel_each_note()
{
}

static void
skel_after_notes()
{
}

Process_functions pf_skel = {
NULL, NULL,
skel_before_outers,
skel_each_outer,
skel_between_outers,
skel_after_outers,
skel_each_text,
skel_before_block,
skel_after_block,
skel_before_sources,
skel_between_sources,
skel_after_sources,
skel_before_composite,
skel_after_composite,
skel_each_composite_column,
skel_between_composite_columns,
skel_before_reconstructed,
skel_after_reconstructed,
skel_each_reconstructed_column,
skel_between_reconstructed_columns,
skel_before_source,
skel_after_source,
skel_each_source_column,
skel_between_source_columns,
skel_before_notes,
skel_each_note,
skel_after_notes,
};

#include <psd_base.h>
#include <collate.h>
#include "matrix.h"

static int da94_cmp (const void *, const void *);

#define xmalloc malloc

Line **
sort_sources (List *srcs)
{
  static Line **lines = NULL;
  static int lines_alloced = 0;
  Line *lp;
  int i = 0;

  if (lines_alloced < list_len(srcs)+1)
    {
      lines_alloced = 2*list_len(srcs);
      lines = xmalloc (lines_alloced * sizeof (Line*));
    }

  if (list_len(srcs) == 0)
    lines[0] = NULL;
  else if (list_len(srcs) == 1)
    {
      lines[0] = list_first(srcs);
      lines[1] = NULL;
    }
  else
    {
      for (lp = list_first(srcs); lp; lp = list_next(srcs))
	lines[i++] = lp;
      lines[i] = NULL;
      qsort (lines, i, sizeof (Line*), da94_cmp);
    }

  return lines;
}

static int
da94_cmp (const void *a, const void *b)
{
  if (do_aka_primary)
    {
      int diff = strlen((*(Line**)a)->name) - strlen((*(Line**)b)->name);
      if (diff)
	return diff;
    }
  return collate_cmp_graphemes ((*(Line**)a)->name, (*(Line**)b)->name);
}

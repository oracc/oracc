#include <psd_base.h>
#include <hash.h>
#include "collate.h"
#include "matrix.h"
#include "sources.h"
#include "gdl.h"
#include "warning.h"

#define xmalloc malloc

struct SRCS_RANGE
{
  Uchar *location;
  Uchar *lines;
};
typedef struct SRCS_RANGE Srcs_range;

struct SRCS_PRESENCE
{
  Uchar *name;
  int val;
  int subval;
  Line *present;
};
typedef struct SRCS_PRESENCE Srcs_presence;

struct SRCS_ARRAY
{
  Uchar *name;
  List *lines;
  List *ranges;
};
typedef struct SRCS_ARRAY Srcs_array;

Hash_table *sources_hash;
int sources_count = 0;
Srcs_array *sources_array = NULL;
Srcs_presence *sources_presence = NULL;
static void srcs_make_array_sub (Uchar *k, void *v);
static void srcs_make_array (void);
static void srcs_make_presence (void);
static void srcs_clear_presence (void);
static int da94_cmp (const void *a, const void *b);

void
srcs_init ()
{
  sources_hash = hash_create(0);
}

void
srcs_add_name (Uchar *name, Line*line)
{
  List *lp;
    
  lp = hash_find (sources_hash, name);
  if (lp == NULL)
    {
      lp = list_create (LIST_DOUBLE);
      hash_add (sources_hash, name, lp);
      ++sources_count;
    }
  list_add (lp, line);
}

static void
srcs_set_presence (void *vp)
{
  ((Line*)vp)->block->composite->presence->present = vp;
}

void
srcs_in_lines ()
{
  Srcs_array *sp;
  const Uchar *last_location = (const Uchar *)"";

  if (NULL == sources_array)
    srcs_make_array ();

  if (NULL == sources_presence)
    srcs_make_presence ();

  for (sp = sources_array; sp->name; ++sp)
    {
      Srcs_presence *presp;
      Srcs_range *rp;
      int i;

      srcs_clear_presence ();
      list_exec (sp->lines, srcs_set_presence);
      sp->ranges = list_create(LIST_SINGLE);
      presp = sources_presence; 
      while (1)
	{
	  while (presp->name && !presp->present)
	    ++presp;
	  if (presp->name)
	    {
	      Srcs_presence *first = presp;
	      while (presp->name && presp->present)
		{
		  if (NULL == (presp+1)->name
		      || NULL == (presp+1)->present
		      || ((presp+1)->present
			  && tabloc_transition(presp->present->tabloc, 
					       (presp+1)->present->tabloc)))
		    {
		      Srcs_range *range = xmalloc (sizeof(Srcs_range));
		      Srcs_presence *last = presp;
		      range->location = tabloc_location_noline (presp->present->tabloc);
		      if (first == last)
			range->lines = (Uchar*)xstrdup ((const char *)first->name);
		      else
			{
			  range->lines = xmalloc (strlen((const char *)first->name) 
						  + strlen((const char *)last->name) + 2);
			  sprintf ((char*)range->lines, "%s-%s", 
				   first->name, last->name);
			}
		      list_add (sp->ranges, range);
		      first = presp+1;
		    }
		  ++presp;
		}
	    }
	  else
	    break;
	}
      printf ("%s: ", sp->name);
      for (rp = list_first(sp->ranges), i = 0; rp; rp = list_next(sp->ranges), ++i)
	{
	  if (i)
	    printf("; ");
	  if (*rp->location && strcmp((const char *)rp->location, (const char *)last_location))
	    {
	      printf("%s = %s", rp->location, rp->lines);
	      last_location = rp->location;
	    }
	  else
	    printf ("%s", rp->lines);
	}
      printf (".\n");
    }
}

static int counter;
static void
srcs_make_array_sub (Uchar *k, void *v)
{
  sources_array[counter].name = k;
  sources_array[counter++].lines = v;
}

static void
srcs_make_array ()
{
  sources_array = xmalloc (sizeof(Srcs_array) * (sources_count+1));
  counter = 0;
  hash_exec2 (sources_hash, (void (*)(const unsigned char *, void *))srcs_make_array_sub);
  sources_array[counter].name = NULL;
  sources_array[counter].lines = NULL;
  qsort (sources_array, counter, sizeof (Srcs_array), da94_cmp);
}

void
srcs_init_presence (void *vp)
{
  Block *bp = vp;
  Uchar *ltxt;
  int how_far;

  ltxt = bp->composite->name;
  bp->composite->presence = &sources_presence[counter];
  sources_presence[counter].name = ltxt;
  if (sscanf ((const char*)ltxt, "%d%n", &sources_presence[counter].val, &how_far) < 1)
    {
      vwarning2 (bp->composite->file,bp->composite->linenum,
	     "bad line format `%s'", ltxt);
    }
  if (ltxt[how_far])
    {
      if (ltxt[how_far] >= 'a' && ltxt[how_far] <= 'z'
	  || ltxt[how_far] >= 'A' && ltxt[how_far] <= 'Z'
	  )
	{
	  sources_presence[counter].subval = ltxt[how_far] - 'a' + 1;
	}
      else
	vwarning2 (((Block*)vp)->composite->file,((Block*)vp)->composite->linenum,
	       "bad line format (non alpha subscript) `%s'", ltxt);
    }
  ++counter;
}

static void
srcs_make_presence ()
{
  sources_presence = calloc(list_len (curr_matrix) + 1, sizeof(Srcs_presence));
  counter = 0;
  list_exec (curr_matrix, srcs_init_presence);
  sources_presence[counter].present = NULL;
}

static int
da94_cmp (const void *a, const void *b)
{
  return collate_cmp_graphemes (((Srcs_array*)a)->name, ((Srcs_array*)b)->name);
}

static void
srcs_clear_presence ()
{
  Srcs_presence *sp;
  for (sp = sources_presence; sp->name; ++sp)
    sp->present = NULL;
}

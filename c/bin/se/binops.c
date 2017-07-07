/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: operator.c,v 0.5 1998/12/19 06:29:41 s Exp $
*/
/* operator driver module for SF4 Query program */

#include <psd_base.h>
#include "se.h"

struct Field_range_info
{
  int use;
  int min;
  int max;
};
typedef struct Field_range_info Field_range_info;
#define USE_EXACT	0 /* fields must be same */
#define USE_RANGE	1 /* fields must be within range */
#define USE_LAST	2 /* as above, but indicates the last significant field */
#define USE_IGNORE	3 /* ignore field in compares */
#define TEXT 		0
#define RECORD 		1
#define BRANCH 		2
#define WORD 		3
#define GRAPHEME 	4
#define LAST_FIELD	5
Field_range_info ranges[LAST_FIELD];

extern const char *se_toks_names[];
typedef struct Datum (*Boolfunc)(struct Datum*,struct Datum*);
static Boolfunc binop_tab[n_bad_lev];
static Boolfunc dtype_tab[d_NONE];

/* variables for binary operations */
static struct Datum d1, d2, new;
static int matches;
static struct location8 **lft_begin, **lft_end, **lft;
static struct location8 **rt_begin, **rt_end, **rt;
static struct location8 **record_list;
static size_t record_list_count;
static Boolean op_invert;
static int range_field, range_adjust;

static int l16;

static enum datum_type binop_result_type;
static size_t binop_result_size;

static int op_range_min, op_range_max;
static Boolean op_min_given, op_max_given;
static void op_init (struct near *);
static void op_exact_upto (int r_fld);
static void op_ignore (int range_fld);
static void op_range (int r_fld, int r_min, int r_max);
static void op_last (int r_fld, int r_min, int r_max);
static struct Datum op_driver (struct Datum *,struct Datum*);
static void datum_free (struct Datum *dp);
static void binop_setup (struct Datum *,struct Datum *);
static struct Datum binop_term (void);

static struct Datum record(struct Datum *,struct Datum *);
static struct Datum field(struct Datum *,struct Datum *);
static struct Datum word(struct Datum *,struct Datum *);
static struct Datum sign(struct Datum *,struct Datum *);
static struct Datum merge(struct Datum *,struct Datum *);
static struct Datum space(struct Datum *,struct Datum *);
static struct Datum hyphen(struct Datum *,struct Datum *);
static struct Datum any(struct Datum *,struct Datum *);

extern void debug_results(const char *label, struct Datum*);

void
binop_init()
{
  binop_tab[n_record]  = record;
  binop_tab[n_field]   = field;
  binop_tab[n_word]    = word;
  binop_tab[n_grapheme]= sign;
  binop_tab[se_or]     = merge;
  binop_tab[se_space]  = space;
  binop_tab[se_hyphen] = hyphen;
  binop_tab[se_dot] = hyphen;
  binop_tab[se_tilde]  = any;

  /* IS THIS OK, not to set dtype_tab for most indexes? */
  dtype_tab[d_cat]  = record;
  dtype_tab[d_lem]  = word;
}

struct Datum
binop(enum se_toks bool, struct near *range, struct Datum d1, struct Datum d2)
{
  op_init(range);
  progress("se binop: %s %s %s\n", 
	   datum_type_names[d1.type],
	   se_toks_names[bool],
	   datum_type_names[d2.type]);
  if (doing_debug) /*(MSG_ISSET(MSG_DEBUG))*/
    {
      debug_results("d1 before binop", &d1);
      debug_results("d2 before binop", &d2);
    }
  switch (bool)
    {
    case se_sans:
    case se_not:
      op_invert = TRUE;
    case se_near:
    case se_and:
      if (d1.type == d2.type)
	return (binop_tab[range->lev])(&d1, &d2);
      else
	return dtype_tab[min(d1.type,d2.type)](&d1,&d2);
      break;
    default:
      return (binop_tab[bool])(&d1, &d2);
      break;
    }
}

struct Datum
space (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (BRANCH);
  if (d1->data_size == sizeof(struct location8))
    op_last(WORD, 1, 1);
  else
    {
      op_range (WORD, 1, 1);
      op_last (GRAPHEME, 1, 1);
    }

  return op_driver(d1,d2);
}

struct Datum
any (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (BRANCH);
  if (d1->data_size == sizeof(struct location8))
    op_last(WORD, 1, 1);
  else
    {
      op_range (WORD, 0, 1);
      op_last (GRAPHEME, 0, 1);
    }
  return op_driver(d1,d2);
}

struct Datum
variant (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (BRANCH);
  op_ignore (WORD);
  op_last (GRAPHEME, op_range_min, op_range_max);
  return op_driver(d1,d2);
}

struct Datum
hyphen (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (BRANCH);
  if (d1->data_size == sizeof(struct location8))
    op_last(WORD, 0, 1);
  else
    {
      op_range (WORD, 0, 0);
      op_last (GRAPHEME, op_min_given ? op_range_min : 0, 1);
    }
  return op_driver(d1,d2);
}

struct Datum
sign (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (BRANCH);
  op_ignore (WORD);
  op_last (GRAPHEME,
	   op_min_given ? op_range_min : 0,
	   op_max_given ? op_range_max : 1);
  return op_driver(d1,d2);
}

struct Datum
word (struct Datum *d1, struct Datum *d2)
{
  if (d1->type == d_lem || d2->type == d_lem)
    {
      op_ignore(BRANCH);
      op_last (WORD, op_range_min, op_range_max);
    }
  else
    {
      op_exact_upto (BRANCH);
      if (d1->data_size == sizeof(struct location8))
	op_last (WORD, op_range_min, op_range_max);
      else
	{
	  op_range (WORD, op_range_min, op_range_max);
	  op_last (GRAPHEME,
		   op_min_given ? -START_COLUMN_MAX : 0,
		   START_COLUMN_MAX);
	}
   }
 
  return op_driver(d1,d2);
}

struct Datum
source (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (RECORD);
  op_range (BRANCH, op_range_min, op_range_max);
  op_ignore (WORD);
  op_last (GRAPHEME,
	   op_min_given ? -START_COLUMN_MAX : 0,
	   START_COLUMN_MAX);
  return op_driver(d1,d2);
}

struct Datum
field (struct Datum *d1, struct Datum *d2)
{
  op_exact_upto (TEXT);
  if (op_min_given && op_max_given)
    op_range (RECORD, op_range_min, op_range_max);
  else if (op_min_given && !op_max_given)
    op_range (RECORD, op_range_min, 0);
  else
    /* if the range is within +1 lines set the minimum to 1, else to 0 */
    op_range (RECORD, (op_max_given && op_range_max) ? 1 : 0, op_range_max);

#if 0
  /* Surely this is bogus? If a line constraint is given
     there is no way that lower-level constraints can be
     relevant */
  if (d1->data_size == sizeof(struct location8)
      || d2->data_size == sizeof(struct location8))
    ranges[RECORD].use = USE_LAST;
  else
    {
      op_ignore (BRANCH);
      op_ignore (WORD);
      op_last (GRAPHEME,
	       op_min_given ? -START_COLUMN_MAX : 0,
	       START_COLUMN_MAX);
    }
#else
  ranges[RECORD].use = USE_LAST;
#endif
  return op_driver(d1,d2);
}

struct Datum
record (struct Datum *d1, struct Datum *d2)
{
  if (op_range_max - op_range_min)
    error (NULL, "ranges not permitted on RECORD operators 't' or 'r'");
  op_last (TEXT, 0, 0);
  return op_driver(d1,d2);
}

struct Datum
merge (struct Datum *d1, struct Datum *d2)
{
  struct Datum *d3;
  d3 = merge_data_pair (d1, d2);
  datum_free (d1);
  datum_free (d2);
  return *d3;
}

void
datum_free (struct Datum *dp)
{
  if (dp->l.l8p)
    free(dp->l.l8p);
  if (dp->r.l8p)
    free(dp->r.l8p);
  dp->l.l8p = dp->r.l8p = NULL;
}

/******************************************************************
 *
 * FUNCTIONS FOR THE BINARY OPERATOR API
 *
 ******************************************************************/
#define range np->range
static void
op_init (struct near *np)
{
  if (np->dir == n_after)
    {
      op_range_min = np->redup;
      op_range_max = range;
      op_max_given = TRUE;
      op_min_given = np->redup ? TRUE : FALSE;
    }
  else if (np->dir == n_before)
    {
      op_range_min = -range;
      op_range_max = 0;
      op_max_given = FALSE;
      op_min_given = TRUE;
    }
  else 
    {
      op_range_min = -range;
      op_range_max = range;
      op_min_given = op_max_given = TRUE; /* should this be conditional on whether
					     the range was explicit or implicit? */
    }
  memset (ranges, '\0', LAST_FIELD * sizeof (Field_range_info));
}

static void
op_exact_upto (int r_fld)
{
  int i;
  for (i = 0; i <= r_fld; ++i)
    ranges[i].use = USE_EXACT;
}

static void
op_ignore (int r_fld)
{
  ranges[r_fld].use = USE_IGNORE;
}

static void
op_range (int r_fld, int r_min, int r_max)
{
  if (!r_min && !r_max)
    {
      ranges[r_fld].use = USE_EXACT;
    }
  else
    {
      ranges[r_fld].use = USE_RANGE;
      ranges[r_fld].min = r_min;
      ranges[r_fld].max = r_max;
    }
}

static void
op_last (int r_fld, int r_min, int r_max)
{
  ranges[r_fld].use = USE_LAST;
  ranges[r_fld].min = r_min;
  ranges[r_fld].max = r_max;
  range_field = r_fld;
  range_adjust = r_max; /* used for resetting the right bound on 
			   matches to inverted searches */
}

/* operator driver */

/* The record list array sets a non-null value for each match.
   Only matching lft values are non-null; when inverting the actual
   value is ignored, because we select only the NULL slots. */
static void
record_match (void)
{
  extern void debug_message(const char*);
  if (doing_debug)
    {
      debug_message("record match");
      if (d1.data_size == sizeof(struct location8))
	debug_location8(*lft, *rt);
      else
	debug_location16((struct location16*)*lft, (struct location16*)*rt);
      debug_message("record match end");
    }
  record_list[lft-lft_begin] = *rt;
  ++matches;
}

int
same_word(struct location16 *l, struct location16 *r)
{
  return l && r 
    && l->text_id == r->text_id
    && l->unit_id == r->unit_id
    && l->word_id == r->word_id;
}

int
same_but_prop(struct location16 *l, struct location16 *r)
{
  return l && r 
    && l->text_id == r->text_id
    && l->unit_id == r->unit_id
    && l->word_id == r->word_id
    && mask_sc(l->start_column) == mask_sc(r->start_column);
}

#define l16c(l8p) ((struct location16*)(l8p[0]))

#define different(r,l) \
  (mask_sc(l16c(r)->start_column) < mask_sc(l16c(l)->start_column)	\
   || logo_mask(l16c(r)->end_column) > logo_mask(l16c(l)->end_column))

/* Interaction between branch_id, start_column and end_column
   is more complex in SF4 than it was in SF3, and this routine
   needs more testing to ensure that it takes account of all 
   aspects of that. */
static struct Datum
op_driver (struct Datum *d1, struct Datum *d2)
{
  struct location8 **rt_base, **orig_rt_base, **orig_lft;

  binop_setup (d1,d2);
  for (orig_rt_base = rt_base = rt, orig_lft = lft; lft < lft_end; ++lft)
    {
      /* found_match = 0; */
      for (rt = rt_base; rt < rt_end; ++rt)
	{
	retry_rt:
	  if (PQVal(rt[0]->text_id) < PQVal(lft[0]->text_id))
	    continue;
	  if (idVal(rt[0]->text_id) < idVal(lft[0]->text_id))
	    continue;
	  else if (idVal(rt[0]->text_id) > idVal(lft[0]->text_id))
	    goto move_left;
	  else if (USE_LAST == ranges[TEXT].use)
	    {
	      record_match ();
	      goto move_left;
	    }
	  else
	    {
	      if (idVal(rt_base[0]->text_id) < idVal(rt[0]->text_id))
		rt_base = rt;
	    }
	  switch (ranges[RECORD].use)
	    {
	    case USE_EXACT:
	      if (rt[0]->unit_id < lft[0]->unit_id)
		continue;
	      else if (rt[0]->unit_id > lft[0]->unit_id)
		goto move_left;
	      break;
	    case USE_RANGE:
	      if (((int)(rt[0]->unit_id - lft[0]->unit_id)) 
		  < ranges[RECORD].min)
		continue;
	      else if (((int)(rt[0]->unit_id - lft[0]->unit_id)) 
		       > ranges[RECORD].max)
		goto move_left;
	      break;
	    case USE_LAST:
	      if (((int)(rt[0]->unit_id - lft[0]->unit_id))
		  < ranges[RECORD].min)
		continue;
	      else if (((int)(rt[0]->unit_id - lft[0]->unit_id))
		       > ranges[RECORD].max)
		goto move_left;
	      else
		{
		  record_match ();
		  goto move_left;
		}
	    case USE_IGNORE: /* in fact, we don't use this for RECORD */
	      break;
	    }
	  if (l16)
	    switch (ranges[BRANCH].use)
	      {
	      case USE_IGNORE:
		break;
	      case USE_EXACT:
		/* negative branch_id's are used for optional graphemes,
		   like determinatives, alternatives etc. */
		if (l16c(rt)->branch_id == -l16c(lft)->branch_id)
		  break;
		if (l16c(rt)->branch_id < l16c(lft)->branch_id)
		  continue;
		else if (l16c(rt)->branch_id > l16c(lft)->branch_id)
		  goto move_left;
		break;
	      case USE_LAST:
		if (l16c(rt)->branch_id == -l16c(lft)->branch_id)
		  {
		    record_match ();
		    goto move_left;
		  }
		if (l16c(rt)->branch_id - l16c(lft)->branch_id < ranges[BRANCH].min)
		  continue;
		else if (l16c(rt)->branch_id - l16c(lft)->branch_id > ranges[BRANCH].max)
		  goto move_left;
		else
		  {
		    record_match ();
		    goto move_left;
		  }
		break;
	      case USE_RANGE:
		if (l16c(rt)->branch_id - l16c(lft)->branch_id < ranges[BRANCH].min)
		  continue;
		else if (l16c(rt)->branch_id - l16c(lft)->branch_id > ranges[BRANCH].max)
		  goto move_left;
		break;
	      }
	  switch (ranges[WORD].use)
	    {
	    case USE_IGNORE:
	      break;
	    case USE_EXACT:
	      if (rt[0]->word_id < lft[0]->word_id)
		continue;
	      else if (rt[0]->word_id > lft[0]->word_id)
		goto move_left;
	      break;
	    case USE_LAST:
	      if ((int)rt[0]->word_id - (int)lft[0]->word_id < ranges[WORD].min)
		continue;
	      else if ((int)rt[0]->word_id - (int)lft[0]->word_id > ranges[WORD].max)
		goto move_left;
	      else
		{
		  record_match ();
		  goto move_left;
		}
	      break;
	    case USE_RANGE:
	      if ((int)rt[0]->word_id - (int)lft[0]->word_id < ranges[WORD].min)
		continue;
	      else if ((int)rt[0]->word_id - (int)lft[0]->word_id > ranges[WORD].max)
		goto move_left;
	      break;
	    }
	  if (l16)
	    switch (ranges[GRAPHEME].use)
	      {
	      case USE_LAST:
		/* this still needs rewriting to fit properly with the SF4
		   location information */
		{
		  struct location16 *l16l = (struct location16*)lft[0];
		  struct location16 *l16r = (struct location16*)rt[0];
		  int diff = 0;
		  int propcmp = 0;

		  if (d1->expr_id > 0 && d2->expr_id > 0
		      && d1->expr_id == d2->expr_id)
		    {
		      propcmp = prop_sc(l16l->start_column) == prop_sc(l16r->start_column);
		      diff = (int)mask_sc(l16r->start_column)
			- (int)logo_mask(l16l->end_column);
		    }
		  else
		    {
		      /* in 'dumu N5=ak' we ensure that range matching and
			 prop checking are always successful */
		      if (prop_sc(l16l->start_column) || prop_sc(l16r->start_column))
			diff = 0;
		      else
			diff = (int)mask_sc(l16r->start_column)
			  - (int)logo_mask(l16l->end_column);
		      propcmp = 1;
		    }

		  if (diff >= ranges[GRAPHEME].min && diff <= ranges[GRAPHEME].max
		      && ((l16r->branch_id == l16l->branch_id && different(rt,lft))
			  || l16r->branch_id == -l16l->branch_id))
		    {
		      if (propcmp)
			{
			  record_match ();
			  goto move_left;
			}
		      else
			{
			  /* If this rt vs lft compare failed only on properties,
			   * and if the next lft is the same except for properties,
			   * try all of the rt nodes again against the next lft.
			   */
			  if ((lft+1) < lft_end && same_but_prop((struct location16*)*lft,
								 (struct location16*)*(lft+1)))
			    {
			      ++lft;
			      rt = rt_base = orig_rt_base;
			      goto retry_rt;
			    }
			}
		    }
		}

		break;
	      case USE_EXACT:
	      case USE_RANGE:
	      case USE_IGNORE:
		fatal ();
		break;
	      }
	}
    move_left:
#if 0
      if (l16)
	{
	  while ((lft+1) < lft_end && same_word((struct location16*)*lft,
						(struct location16*)*(lft+1)))
	    ++lft;
	}
#else
      if ((lft+1) < lft_end
	  && (mask_sc(((struct location16*)lft[1])->start_column)
	      < mask_sc(((struct location16*)lft[0])->start_column)))
	rt_base = orig_rt_base;
#endif
    }
  return binop_term ();
}

static void
binop_setup (struct Datum *leftargp, struct Datum *rightargp)
{
#if 1
  d1 = *leftargp;
  d2 = *rightargp;
#else
  /* d2 being the left arg goes back to a stack-based code evaluator
     from which this routine was ported. */
  d1 = *rightargp;
  d2 = *leftargp;
#endif

  l16 = (d1.data_size == 16 && d2.data_size == 16);

  /* the left side uses the right end of its range */
  if (d1.r.l8p != NULL)
    lft = lft_begin = d1.r.l8p;
  else
    lft = lft_begin = d1.l.l8p;

  /* the right side uses the left end of its range */
  if (d2.l.l8p != NULL)
    rt = rt_begin = d2.l.l8p;
  else
    rt = rt_begin = d2.r.l8p;

  /* set the end to just after the last element of each side */
  lft_end = &lft_begin[d1.count];
  rt_end = &rt_begin[d2.count];

  /* can't have more matches than items on left hand side */
  record_list_count = d1.count /* max (d1.count, d2.count) */;
  if (record_list_count)
    {
      /* size_t i; */
      record_list = calloc (record_list_count, sizeof (struct location8 *));
#if 0
      for (i = 0; i < record_list_count; ++i)
	record_list[i] = NULL;
#endif
    }
  else
    {
      record_list = NULL;
    }

  /* the result datum type is the lowest granularity */
  binop_result_type = max(d1.type,d2.type);
  binop_result_size = min(d1.data_size,d2.data_size);

#if 0
  if (doing_debug) /*(MSG_ISSET(MSG_DEBUG))*/
    {
      debug_results("lft at binop_setup exit", &d1);
      debug_results("rt at binop_setup exit", &d2);
    }
#endif
  
  matches = 0;
}

#define adjust(_val,_bound) \
	if ((Unsigned32)_val + range_adjust > _bound) \
	  _val = _bound; \
	else if ((Unsigned32)_val + range_adjust < 0) \
	  _val = 0; \
	else \
	  _val += range_adjust

static struct Datum
binop_term ()
{
  if (op_invert)
    {
      matches = record_list_count - matches;
      if (matches)
	{
	  size_t new_index, i;
	  new.l.l8p = malloc (sizeof(struct location8 *) * matches);
	  new.r.l8p = malloc (sizeof(struct location8 *) * matches);
	  for (new_index = i = 0; i < record_list_count; ++i)
	    {
	      if (NULL == record_list[i])
		{
		  new.l.l8p[new_index] = new.r.l8p[new_index] = lft_begin[i];
		  switch (range_field)
		    {
		    case TEXT:
		      adjust (new.r.l8p[new_index]->text_id, TEXT_ID_MAX);
#if 0
		      if (l16)
			set_property (new.r.l16p[new_index]->properties, PROP_WE);
#endif
		      break;
		    case RECORD:
		      adjust (new.r.l8p[new_index]->unit_id, UNIT_ID_MAX);
		      break;
		    case BRANCH:
		      adjust (new.r.l16p[new_index]->branch_id, BRANCH_ID_MAX);
		      break;
		    case WORD:
		      adjust (new.r.l8p[new_index]->word_id, WORD_ID_MAX);
		      break;
		    case GRAPHEME:
		      adjust (new.r.l16p[new_index]->start_column, START_COLUMN_MAX);
		      break;
		    default:
		      fatal ();
		      break;
		    }
		  ++new_index;
		}
	    }
	}
      else
	new.l.l8p = new.r.l8p = NULL;
    }
  else
    {
      if (matches)
	{
	  size_t new_index, i;
	  new.l.l8p = malloc (sizeof (struct location8) * matches);
	  new.r.l8p = malloc (sizeof (struct location8) * matches);
	  for (new_index = i = 0; i < record_list_count; ++i)
	    {
	      if (NULL != record_list[i])
		{
		  new.l.l8p[new_index] = lft_begin[i];
		  new.r.l8p[new_index] = record_list[i];
		  ++new_index;
		}
	    }
	}
      else
	new.l.l8p = new.r.l8p = NULL;
    }

  new.expr_id = d1.expr_id;
  new.type = binop_result_type;
  new.data_size = binop_result_size;

  if (matches)
    {
      new.count = matches;
      new.l.l8p = realloc (new.l.l8p, sizeof (struct location8) * matches);
      new.r.l8p = realloc (new.r.l8p, sizeof (struct location8) * matches);
    }
  else
    {
      new.count = 0;
      if (new.l.l8p)
	free (new.l.l8p);
      if (new.r.l8p)
	free (new.r.l8p);
      new.l.l8p = NULL;
      new.r.l8p = NULL;
    }
  datum_free (&d1);
  datum_free (&d2);
  free(record_list);
  record_list = NULL;
  return new;
}

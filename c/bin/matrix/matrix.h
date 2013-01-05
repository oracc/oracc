#ifndef MATRIX_H_
#define MATRIX_H_

#include <list.h>

enum Outer_type_e { OUTER_TEXT, OUTER_MATRIX };
enum Line_type_e { LINE_COMPOSITE, LINE_SOURCE };

struct OUTER 
{
  enum Outer_type_e type;
  int index;
  List *stuff;		/* This is either a list of text-lines 
			   or a list of struct BLOCK's*/
  List_node *parent;
};
typedef struct OUTER Outer;

struct TABLOC
{
  Uchar *compressed;  /* e.g. oii15' */
  Uchar *expanded;    /* e.g. obv. ii 15' */
  struct SRCS_TABLOC *encoded; /* a structure in which tabloc features
				  are encoded as numeric values for
				  sorting and comparing */
};
typedef struct TABLOC Tabloc;
extern Tabloc empty_tabloc;

struct LINE
{
  Uchar *file;
  Unsigned32 linenum;
  enum Line_type_e type;
  int index;
  Uchar *line;
  Uchar *name;
  Uchar *altsig;
  Tabloc *tabloc; 	/* used for absolute tablet locators in siglum column */
  List *columns; 	/* list of struct COMPOSITE_COLUMN or SOURCE_COLUMN's */
#if 0
  List *complexes; 	/* as above, but pointers to the head of each complex */
#endif
  List *notes; 		/* list of Uchar *'s for interlinear notes */
  int column_count;
  List_node *parent;
  struct BLOCK *block;
  struct SRCS_PRESENCE *presence;
  Boolean col_count_err_given;
};
typedef struct LINE Line;

struct BLOCK
{
  int index;
  Line *composite;
  Line *reconstructed;
  List *sources; /* list of struct LINE's */
  List_node *parent;
};
typedef struct BLOCK Block;

struct COMPOSITE_COLUMN
{
  Uchar *text;
  Uchar delim[2];
  Uchar *complex_text;
  Uchar *notemark;
  int index;
  int complex_index;
  int complex_columns;
  int complex_zeroes;
  int complex_phantoms;
  int width;
  int maxwidth;
  int visible_width;
  struct COMPOSITE_COLUMN *left;
  struct COMPOSITE_COLUMN *right;
  struct SOURCE_COLUMN *down;
  struct COMPOSITE_COLUMN *complex;
  List_node *parent;
  Line *line;
  Boolean done;
  Boolean zero_flag;
  Boolean space_flag;
  Boolean needs_phantom;
  Boolean digit_phantom;
  Boolean complex_has_variant;
  int short_line_divider; /* always 0 or 1 */
};
typedef struct COMPOSITE_COLUMN Composite_column;

struct SOURCE_COLUMN 
{
  Uchar *text;
  Uchar *complex_text;
  Uchar *notemark;
  int index;
  int cindex;
  int columns_occupied;
  int visible_width;
  int explicit_column_count;
  int ncomplexes;
  struct SOURCE_COLUMN *left;
  struct SOURCE_COLUMN *right;
  struct SOURCE_COLUMN *up;
  struct SOURCE_COLUMN *down;
  struct SOURCE_COLUMN *complex;
  struct COMPOSITE_COLUMN *composite;
  struct COMPOSITE_COLUMN *composite_end; /* when a source column spans more than one 
					     composite column, this pointer gives the
					     last composite column that corresponds to
					     this source column */
  List_node *parent;
  Line *line;
  const Uchar *delim;
  char apocopation_flag;
  Boolean text_entry_flag;
  Boolean align_override_flag;
  Boolean done;
  Boolean zero_flag;
  Boolean space_flag;
  Boolean filler_flag;
  Boolean needs_phantom;
  Boolean digit_phantom;
  Boolean complex_has_variant;
  Boolean explicit_delim;
  Boolean explicit_ncols;
};
typedef struct SOURCE_COLUMN Source_column;

extern Boolean do_aka_primary;
extern Boolean do_encapsulate;
extern Boolean do_expand;
extern Boolean do_minimal;
extern Boolean do_sort_sources;
extern Boolean do_square_brackets;
extern Boolean pf_vars_collapse_variants;
extern Uchar *output_fn;
extern List *curr_matrix;
extern Line *curr_line;
extern struct File *matrix_location (void);
extern Tabloc *do_tabloc (Uchar *t);
extern Boolean tabloc_transition (Tabloc *a, Tabloc *b);
extern Uchar *tabloc_location_noline (Tabloc *tp);
extern Uchar *expand_complex (Source_column *sp);
extern Boolean expand_needs_expansion (Source_column *sp);
extern void phantom_prepare_phantoms (void *vp);
extern Line **sort_sources (List *srcs);
extern void kill_explicit_count (Uchar *s);
extern int global_col0_maxwidth;
extern int visible_width (Uchar *s);
#endif /*!MATRIX_H_*/

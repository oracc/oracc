#include <psd_base.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include "atf.h"
#include "oracclocale.h"
#include "collate.h"
#include "psd_da94.h"
#include "pf_lib.h"
#include "sources.h"
#include "warning.h"
#include "options.h"

#define MTX_ISDELIM(c) ((c)=='-'||(c)=='.'||(c)==' '||(c)=='\t'||(c)=='\n')

#define prev(ptr) (ptr)->parent->prev
#define next(ptr) (ptr)->parent->next
#define RECONSTRUCTED_LINE(s) ('.' == (s)[0] && ':' == (s)[1])

#define meta_info_char(s) (/*RAISED_BANG == (s) || RAISED_QUERY == (s) ||*/ \
                           '!' == (s) || '?' == (s) || '*' == (s) \
                           || '\'' == (s))

#define xcalloc calloc
#define xmalloc malloc

/*int ods_cols, ods_mode; */ /* for bad architecture of libgdl */
const char *input_file = NULL;
const char *project = NULL;

List *outer; /* list of struct OUTER's */
List *curr_stuff, *curr_matrix, *curr_sources, *curr_notes;
List *multi_column_entries;
Block *curr_block;
Line *curr_composite, *curr_line;
Composite_column *curr_comp_col;
Source_column *curr_sou_col;
int block_index, composite_index, source_index, non_counting_columns, 
  global_col0_maxwidth, cindex;
Uchar *output_fn = "";
char *error_file = NULL;
long error_line = -1;

Boolean need_vars = FALSE;
Boolean do_align = FALSE;
Boolean do_aka_primary = FALSE;
Boolean do_check = FALSE;
Boolean do_cmp = FALSE;
Boolean do_debug = FALSE;
Boolean do_encapsulate = FALSE;
Boolean do_expand = FALSE;
Boolean do_lines_in_srcs = FALSE;
Boolean do_minimal = FALSE;
Boolean do_raw = FALSE;
Boolean do_sf4 = FALSE;
Boolean do_sort_sources = FALSE;
Boolean do_square_brackets = FALSE;
Boolean do_stdin = FALSE;
Boolean do_tex = FALSE;
Boolean do_warn_blanks = FALSE;
Boolean do_xml = FALSE;
Boolean fragments = FALSE;

int matrices_found = 0;

extern void * list_nth (List *lp, int n);
static Uchar * get_composite_entry (Uchar **delimp);
static Uchar * get_source_entry (Boolean *t_e_flag, char *apoc_flag, Uchar *explicit_delim);
static int blank_line (Uchar *s);
static int explicit_column_count (Uchar *s);
static int is_multi_col (Source_column*sp);
static int matches_end_matrix (Uchar *lp);
static int matches_matrix (Uchar *lp);
#if 0
static int safe_sou_index (void);
#endif
static void apocopation_setup (char apoc);
static void basic_source_setup (Uchar *ep, Boolean *t_e_flag);
static void get_entry_init (Uchar *lp);
static void new_block (List *listptr);
static void new_composite (Block *bp, Uchar *lp);
static void new_composite_column (void);
static void new_line (enum Line_type_e type, Uchar *lp);
static void new_outer (List *listptr, enum Outer_type_e type);
static void new_source (Block *bp, Uchar *lp);
static void new_source_column (void);
static void scan_input (int ac, char **av);

static void find_terminal_maxwidth (void *vp);
static void fm_make_list (void *vp);
static void adjust_multis (void);
static void fix_multis (void);
static void add_filler (void);

static Source_column *corresponding_sou_col (List *sp, int n);
static Composite_column *corresponding_cmp_col (List *cp, int n);

extern void gdlif_init(void);
extern void gdlif_term(void);

static void
strip_trailing_white (Uchar *lp)
{
  Uchar *elp = lp + strlen((const char *)lp);
  while (elp > lp && (elp[-1] == ' ' || elp[-1] == '\t'))
    --elp;
  if (*elp != '\n')
    {
      elp[0] = '\n';
      elp[1] = '\0';
    }
}

int
main (int argc, char **argv) 
{
  setlocale(LC_ALL,ORACC_LOCALE);
  options (argc, argv, "abcdE:efklmo:prstvxX4");
  srcs_init ();
  gdlif_init();
  collate_init ("unicode");
  scan_input (argc, argv);
  ++global_col0_maxwidth; /* ensure at least one space after widest col0 */

  if (need_vars)
    iterate (&pf_vars, outer);

  if (do_align)
    iterate (&pf_align, outer);
  else if (do_cmp)
    iterate (&pf_cmp, outer);
  else if (do_debug)
    {
      iterate (&pf_clear, outer);
      iterate (&pf_expand, outer);
      iterate (&pf_debug, outer);
    }
  else if (do_lines_in_srcs)
    srcs_in_lines ();
#if 0
  else if (do_sf4)
    iterate (&pf_sf4, outer);
#endif
  else if (do_tex)
    {
      iterate (&pf_clear, outer);
      iterate (&pf_expand, outer);
      iterate (&pf_clear, outer);
      iterate (&pf_tex, outer);
    }
  else if (do_xml)
    {
      iterate (&pf_clear, outer);
      iterate (&pf_expand, outer);
      iterate (&pf_clear, outer);
      iterate (&pf_mtx, outer);
    }
  gdlif_term();
  if (history)
    return history;
  else if (!matrices_found)
    {
      warning ("no matrices found.");
      return EXIT_WARNING;
    }
  return 0;
}

#if 0
static Uchar *
strip_b (Uchar *src)
{
  Uchar *dest, *ret;
  Boolean skip_next_n = FALSE;

  dest = ret = src;
  while (*src)
    {
#if 0
      if (CONTROL_B == *src)
	{
	  skip_next_n = TRUE;
	  ++src;
	}
      else if (CONTROL_N == *src && skip_next_n)
	{
	  skip_next_n = FALSE;
	  ++src;
	}
      else
#endif
	*dest++ = *src++;
    }
  *dest = '\0';
  return ret;
}
#endif

int outer_index = 0;
static void
scan_input (int ac, char **av)
{
  extern int optind;
  Boolean last_was_note = FALSE;
  outer = list_create (LIST_DOUBLE);
  new_outer (outer, OUTER_TEXT);

  while (optind < ac || do_stdin)
    {
      Uchar *fname = av[optind++];
      Uchar *lp;
      Boolean in_matrix = FALSE, in_block = FALSE;

      if (do_stdin)
	{
	  file_open("", "r");
	  do_stdin = FALSE;
	}
      else
	{
	  extern const char *file;
	  extern FILE *f_log;
	  f_log = stderr;
	  file_open (fname, "r");
	  file = fname;
	}
      if (fragments)
	{
	  in_matrix = TRUE;
	  new_outer (outer, OUTER_MATRIX);
	  ++matrices_found;
	}
      while (NULL != (lp = getline (curr_file->fp)))
	{
	  ++curr_file->line;
	  strip_trailing_white (lp);
	  if (matches_matrix(lp)) 
	    {
	      if (in_matrix)
		{
		  struct FileLine fl = matrix_location();
		  vwarning2 (fl.f, fl.l,"%s",
			   "new matrix began before end of old one");
		}
	      list_add (curr_stuff, xstrdup (lp));
	      new_outer (outer, OUTER_MATRIX);
	      in_matrix = TRUE;
	      ++matrices_found;
	    }
	  else if (matches_end_matrix (lp))
	    {
	      if (list_len (multi_column_entries))
		fix_multis ();
	      new_outer (outer, OUTER_TEXT);
	      list_add (curr_stuff, xstrdup (lp));
	      in_matrix = in_block = FALSE;
	    }
	  else if (in_matrix)
	    {
	      if (blank_line (lp))
		{
		  if (last_was_note)
		    {
		      list_add (curr_notes, xstrdup (""));
		      last_was_note = FALSE;
		    }
		  continue;
		}
	      else if (isdigit (*lp))
		{
		  /* lp = strip_b (lp); */
		  last_was_note = FALSE;
		  if (list_len (multi_column_entries))
		    fix_multis ();
		  multi_column_entries = list_create (LIST_DOUBLE);
		  in_block = 1;
		  new_block (curr_matrix);
		  new_composite (curr_block, lp);
		  curr_block->reconstructed = curr_block->composite;
		}
	      else if (in_block) 
		{
		  if (isspace (*lp) || '#' == *lp)
		    {
		      list_add (curr_notes, xstrdup (lp));
		      last_was_note = TRUE;
		    }
		  else if (RECONSTRUCTED_LINE(lp))
		    {
		      /* lp = strip_b (lp); */
		      if (last_was_note)
			{
			  list_add (curr_notes, xstrdup (""));
			  last_was_note = FALSE;
			}
		      new_composite (curr_block, lp);
		      last_was_note = FALSE;
		    }
		  else
		    {
		      /* lp = strip_b (lp); */
		      if (last_was_note)
			{
			  list_add (curr_notes, xstrdup (""));
			  last_was_note = FALSE;
			}
		      new_source (curr_block, lp);
		      last_was_note = FALSE;
		    }
		}
	      else
		{
		  struct FileLine fl = matrix_location();
		  vwarning2 (fl.f,fl.l,"%s",
			   "junk in matrix, outside of block");
		}
	    }
	  else
	    {
	      list_add (curr_stuff, xstrdup (lp));
	    }
	}
      /* in case we didn't have a </matrix> tag */
      if (list_len (multi_column_entries))
	fix_multis ();
      file_close ();
    }
}

static void
new_outer (List *listptr, enum Outer_type_e type)
{
  Outer *new = xcalloc (1,sizeof (Outer));
  new->index = outer_index++;
  new->type = type;
  new->stuff = list_create (LIST_DOUBLE);
  list_add (listptr, new);
  new->parent = listptr->last;
  curr_stuff = new->stuff;
  if (type == OUTER_MATRIX)
    {
      curr_matrix = new->stuff;
    }
  block_index = composite_index = source_index = 0;
}

static void
new_block (List *listptr)
{
  Block *new = xcalloc (1,sizeof (Block));
  new->reconstructed = new->composite = NULL;
  new->index = block_index++;
  curr_block = new;
  curr_sources = new->sources = list_create (LIST_DOUBLE);
  list_add (listptr, new);
  new->parent = listptr->last;
}

static void
new_composite (Block *bp, Uchar *lp)
{
  Uchar *s, *dp = NULL, *ep;
  Composite_column *curr_complex = NULL;
  int comp_col_index, complex_index;

  new_line (LINE_COMPOSITE, lp);
  curr_line->index = composite_index++;
  source_index = 0;
  bp->composite = curr_line;
  curr_line->block = bp;
  curr_line->parent = NULL;
  curr_composite = curr_line;
  if (RECONSTRUCTED_LINE(lp))
    {
      curr_line->name = xstrdup("");
    }
  else 
    {
      for (s = lp; *s && *s != '.'; ++s)
	;
      if (*s == '.')
	{
	  Uchar save = *s;
	  *s = '\0';
	  curr_line->name = xstrdup (lp);
	  *s = save;
	}
      else
	{
	  struct FileLine fl = matrix_location();
	  vwarning2 (fl.f, fl.l, "%s", 
		   "line number should be followed by `.'");
	  curr_line->name = "unknown";
	}
    }
  
  get_entry_init (curr_line->line);
  comp_col_index = complex_index = 0;
  while (NULL != (ep = get_composite_entry (&dp)))
    {
      if (*ep == '/')
	{
	  if (curr_comp_col)
	    curr_comp_col->short_line_divider = TRUE;
	  else
	    {
	      struct FileLine fl = matrix_location();
	      vwarning2(fl.f, fl.l, "%s", "must have previous column before /");
	    }
	  continue;
	}

      new_composite_column ();
      curr_comp_col->index = comp_col_index++;
      if (NULL == curr_complex)
	curr_complex = curr_comp_col;

      if (!curr_comp_col->index)
	curr_comp_col->text = xstrdup(ep);
      else
	curr_comp_col->text = xstrdup(atf2utf(ep,0));

      if (*curr_comp_col->text == '#')
	curr_comp_col->space_flag = TRUE;
      else
	curr_comp_col->space_flag = FALSE;

      curr_comp_col->width = strlen (ep);
      *curr_comp_col->delim = *dp;
      ++curr_line->column_count;
      curr_comp_col->visible_width 
	= curr_comp_col->maxwidth 
	= visible_width(curr_comp_col->text);

      if (!curr_comp_col->index 
	  && (curr_comp_col->visible_width+1+(curr_comp_col->short_line_divider*2)) 
	  > global_col0_maxwidth)
	global_col0_maxwidth = curr_comp_col->visible_width+1+(curr_comp_col->short_line_divider*2);

#if 0
      fprintf(stderr,"%d: global_col0_maxwidth = %d\n",curr_file->line,global_col0_maxwidth);
#endif
      list_add (curr_line->columns, curr_comp_col);
      curr_comp_col->parent = curr_line->columns->last;
      if (curr_comp_col->index)
	{
	  curr_comp_col->left = prev(curr_comp_col)->data;
	  curr_comp_col->left->right = curr_comp_col;
	}
      else
	curr_comp_col->left = NULL;
      if (curr_comp_col->left && isspace(*curr_comp_col->left->delim))
	{
	  curr_complex->complex_columns = 
	    curr_comp_col->index - curr_complex->index;
	  curr_comp_col->complex_index = ++complex_index;
	  curr_complex = curr_comp_col->complex = curr_comp_col;
	}
      else
	{
	  curr_comp_col->complex = curr_complex;
	  curr_comp_col->complex_index = complex_index;
	  curr_comp_col->complex_columns = 0;
	}
      if (*curr_comp_col->text == '0')
	{
	  curr_comp_col->zero_flag = TRUE;
	  ++curr_comp_col->complex->complex_zeroes;
	}
      else
	curr_comp_col->zero_flag = FALSE;
	
    }
  curr_complex->complex_columns = 
    1 + curr_comp_col->index - curr_complex->index;
}

static void
new_composite_column ()
{
  Composite_column *new = xcalloc(1,sizeof (Composite_column));
  new->line = curr_line;
  curr_comp_col = new;
}

#define single_char_token(e) \
  ((e) == '+' || (e) == '-' || (e) == '.' || (e) == ',' \
   || (e) == ';' || (e) == '/' || (e) == '%' || (e) == '|')

#define non_counting_text(t) (*(t) == '/' || *(t) == ';' || *(t) == '%' || *(t) == '~')

#define simple_source_column_setup() \
  if (curr_sou_col->visible_width > curr_sou_col->composite->maxwidth) \
    curr_sou_col->composite->maxwidth = curr_sou_col->visible_width; \
  curr_sou_col->columns_occupied = 1; \
  curr_sou_col->complex = NULL; \
  ++curr_line->column_count;

#if 0
  curr_sou_col->zero_flag = curr_sou_col->composite->zero_flag;
#endif

#define set_complex() \
  if (curr_sou_col->left) \
    { \
      if (curr_sou_col->cindex == curr_sou_col->left->cindex) \
	curr_sou_col->complex = curr_sou_col->left->complex; \
      else if (' ' == *curr_sou_col->left->composite->delim) \
	curr_sou_col->complex = curr_sou_col; \
      else \
	curr_sou_col->complex = curr_sou_col->left->complex; \
    }

static const Uchar *
explicit_delim (Uchar d)
{
  switch (d)
    {
    case '_':
    case ' ':
      return " ";
    case '-':
      return "-";
    case '.':
      return ".";
    default:
      fatal();
    }
  return "";
}

static int curr_index;
static void
new_source (Block *bp, Uchar *lp)
{
  Uchar *s, *ep, explicit_delim_char;
  Boolean t_e_flag = FALSE;
  char apoc_flag = '\0';

  new_line (LINE_SOURCE, lp);
  curr_line->block = bp;
  curr_line->index = source_index++;
  list_add (curr_sources, curr_line);
  curr_line->parent = curr_sources->last;
  non_counting_columns = curr_index = cindex = 0;

  for (s = lp; *s && !isspace(*s); ++s)
    if (*s == ':')
      break;

  if (*s == ':')
    {
      /*      Uchar save = *s; */
      Uchar *t = lp;
      *s = '\0';

      while (*t && '(' != *t && ',' != *t)
	++t;

      if ('(' == *t) /* we have an alternate siglum */
	{
	  Uchar *sigp;
	  *t++ = '\0';
	  sigp = t;
	  while (*t && ')' != *t)
	    ++t;
	  if ('\0' == *t)
	    {
	      struct FileLine fl = matrix_location();
	      vwarning2 (fl.f, fl.l, "%s", "mismatched parens in alternate siglum notation");
	    }
	  else
	    *t++ = '\0';
	  curr_line->altsig = xstrdup(sigp);
	}
      else
	curr_line->altsig = "";

      if (',' == *t) /* we have a tablet locator */
	{
	  *t++ = '\0';
	  curr_line->tabloc = do_tabloc (t);
	}
      else
	curr_line->tabloc = &empty_tabloc;

      if (':' == s[1])
	{
	  curr_line->name = xstrdup (":");
	  lp += 2;
	  while (isspace(*lp))
	    ++lp;
	  basic_source_setup (curr_line->name, NULL);
	  simple_source_column_setup ();
	  t_e_flag = TRUE;
	  basic_source_setup (lp, &t_e_flag);
	  apocopation_setup ('^');
	  return;
	}
      else
	{
	  curr_line->name = xstrdup (lp);
	  srcs_add_name (curr_line->name, curr_line);
	}
      if (do_aka_primary && curr_line->altsig && *curr_line->altsig)
	{
	  Uchar *tmp = curr_line->altsig;
	  curr_line->altsig = curr_line->name;
	  curr_line->name = tmp;
	}
    }
  else
    {
      struct FileLine fl = matrix_location();
      vwarning2 (fl.f, fl.l, "%s",
		 "siglum should be followed by `:'");
      curr_line->name = "unknown";
      curr_line->altsig = "";
      curr_line->tabloc = &empty_tabloc;
    }

  get_entry_init (s + 1);
  basic_source_setup (curr_line->name, &t_e_flag);
  simple_source_column_setup ();
  ++cindex;

#if 0
  fprintf(stderr,"curr_sou_col->visible_width = %d\n",curr_sou_col->visible_width);
#endif

  if (curr_sou_col->visible_width+1 > global_col0_maxwidth)
    global_col0_maxwidth = curr_sou_col->visible_width+1;

  while (NULL != (ep = get_source_entry (&t_e_flag, &apoc_flag, &explicit_delim_char)))
    {
      basic_source_setup (ep, &t_e_flag);
      if (non_counting_text (curr_sou_col->text))
	{
	  Source_column *sp;
	  curr_sou_col->columns_occupied = 0;
	  ++non_counting_columns;
	  if ('~' == *curr_sou_col->text)
	    {
	      Uchar *tmp;
	      curr_sou_col->text_entry_flag = TRUE;
	      if (NULL != (tmp = strchr(".-_", curr_sou_col->text[1])))
		{
		  curr_sou_col->delim = explicit_delim(*tmp);
		  while (tmp[1])
		    *tmp = *++tmp;
		  *tmp = '\0';
		}
	      else
		curr_sou_col->delim = explicit_delim('-');
		  
	      if (curr_sou_col->left
		  && curr_sou_col->left->text_entry_flag
		  && '~' == *curr_sou_col->left->text)
		{
		  struct FileLine fl = matrix_location();
		  vwarning2 (fl.f, fl.l, "%s",
			     "use of multiple sequential tilde-inserts is not recommended");
		}
	    }
	  else if ('/' == *curr_sou_col->text 
		   || ';' == *curr_sou_col->text 
		   || '%' == *curr_sou_col->text)
	    {
	      curr_sou_col->visible_width += 1; /* for a space before */
	    }

	  for (sp = curr_sou_col->left; sp; sp = sp->left)
	    if (!sp->filler_flag && sp->columns_occupied)
	      break;
	  sp->visible_width += curr_sou_col->visible_width;
	  if (sp->columns_occupied == 1 && sp->visible_width > sp->composite->maxwidth)
	    sp->composite->maxwidth = sp->visible_width;
	}
      else if (apoc_flag) 
	{
	  apocopation_setup (apoc_flag);
	  apoc_flag = '\0';
	}
      else if (!curr_sou_col->text_entry_flag)
	{
	  simple_source_column_setup ();
	  if (explicit_delim_char)
	    curr_sou_col->delim = explicit_delim(explicit_delim_char);
	  ++cindex;
	}
      else if (!is_multi_col (curr_sou_col)) /* processing for text entries that span 
						a single column */
	{
	  simple_source_column_setup ();
	  ++cindex;
	  if (!do_square_brackets)
	    list_add (multi_column_entries, curr_sou_col);
	}
      else /* processing for text entries that span more than one column */
	{
	  curr_sou_col->columns_occupied = curr_sou_col->explicit_column_count;
	  if (curr_sou_col->columns_occupied <= 0)
	    {
	      /* the text entry corresponds to an entire complex;
	         check that the composite column is the start of a complex */
	      if (curr_sou_col->composite->complex != curr_sou_col->composite)
		{
		  struct FileLine fl = matrix_location();
		  vwarning2 (fl.f, fl.l,
			   "text entry should begin where complex begins (col %d not %d)",
			   curr_sou_col->composite->complex->index,
			   curr_sou_col->composite->index);
		}
	      curr_sou_col->columns_occupied = 
		curr_sou_col->composite->complex->complex_columns;
	    }
	  curr_line->column_count += curr_sou_col->columns_occupied;
#if 1
	  /* allow for the &[digits] */
	  if (curr_sou_col->explicit_column_count > 0)
	    curr_sou_col->visible_width 
	      += (1 + ((curr_sou_col->explicit_column_count > 10)
		       ? 2 : 1));
#endif

	  ++cindex;
	  /* we'll come back and do maxwidth calculations later on this one */
	  list_add (multi_column_entries, curr_sou_col);
	  set_complex ();
	  /* remember, add_filler moves curr_sou_col */
	  add_filler ();
	}
      if (curr_sou_col->composite->down == NULL)
	curr_sou_col->composite->down = curr_sou_col;
      set_complex ();
    }
  if (1 == curr_index) /* an empty line is an implicit apocopation */
    {
      if (do_warn_blanks)
	{
	  struct FileLine fl = matrix_location();
	  vwarning2 (fl.f, fl.l, "%s,%s is not filled in",
		   curr_line->name, curr_line->tabloc->compressed);
	}
      basic_source_setup (NULL, NULL);
      apocopation_setup ('^');
    }
  if (curr_line->column_count != curr_composite->column_count)
    {
      struct FileLine fl = matrix_location();
      vwarning2 (fl.f, fl.l, "%s", 
		 "column count mismatch: %d in composite; %d in matrix",
		 curr_composite->column_count,
		 curr_line->column_count);
      curr_composite->col_count_err_given = 
	curr_line->col_count_err_given = TRUE;
    }
}

static void
basic_source_setup (Uchar *ep, Boolean *t_e_flag)
{
  Uchar *carat = NULL;
  new_source_column ();
  curr_sou_col->index = curr_index++;
  if (t_e_flag)
    {
      curr_sou_col->text_entry_flag = *t_e_flag;
      *t_e_flag = FALSE;
    }
  else
    curr_sou_col->text_entry_flag = FALSE;
  if (ep)
    {
      Uchar *bs;


      if ((carat = strchr(ep,'^')) && isdigit(carat[1]))
	{
	  Uchar *end = carat+1;
	  while (isalnum(*end)) /* FIXME: define notemark contents */
	    ++end;
	  if ('^' == *end)
	    {
	      *carat++ = *end = '\0';
	      curr_sou_col->notemark = xstrdup(carat); /* FIXME: multiple notemarks? */
	    }
	}

      /* we assume that visible_width is for use by the pf-align module */

      if (NULL != (bs = strrchr(ep, '\\')))
	{
	  Uchar *dest, *src;
	  curr_sou_col->delim = explicit_delim(bs[1]);
	  curr_sou_col->explicit_delim = TRUE;
	  for (dest = bs, src = bs + 2; *src; ++src)
	    *dest++ = *src;
	  *dest = '\0';
	}

      if (-1 != (curr_sou_col->explicit_column_count = explicit_column_count(ep)))
	curr_sou_col->explicit_ncols = TRUE;
      else
	curr_sou_col->explicit_ncols = FALSE;

      if ('\n' == ep[strlen(ep)-1])
	ep[strlen(ep)-1] = '\0';

      if ('@' == *ep)
	{
	  curr_sou_col->align_override_flag = TRUE;
	  curr_sou_col->text = xstrdup(atf2utf(ep + 1,0));
	}
      else
	{
	  if (!curr_sou_col->index)
	    curr_sou_col->text = xstrdup(ep);
	  else
	    curr_sou_col->text = xstrdup(atf2utf(ep,0));
	}

      curr_sou_col->visible_width = visible_width (curr_sou_col->text);

      if (!curr_sou_col->index)
	{
	  if (curr_line->altsig && *curr_line->altsig)
	    curr_sou_col->visible_width += strlen(curr_line->altsig) + 2;
	  if (*curr_line->tabloc->compressed)
	    curr_sou_col->visible_width += strlen(curr_line->tabloc->compressed) + 1;
	}
    }
  else
    {
      curr_sou_col->text = xstrdup ("");
      curr_sou_col->visible_width = 0;
    }
  list_add (curr_line->columns, curr_sou_col);
  curr_sou_col->parent = curr_line->columns->last;
  if (curr_sou_col->index)
    {
      curr_sou_col->left = prev(curr_sou_col)->data;
      curr_sou_col->left->right = curr_sou_col;
    }
  else
    curr_sou_col->left = NULL;

  if (non_counting_text (curr_sou_col->text))
    curr_sou_col->cindex = curr_sou_col->left->cindex;
  else
    curr_sou_col->cindex = cindex;

  if (list_len (curr_sources) > 1)
    curr_sou_col->up 
      = corresponding_sou_col (((Line *)(curr_sources->last->prev->data))->columns,
			       curr_sou_col->cindex);
  if (curr_sou_col->up)
    {
      curr_sou_col->up->down = curr_sou_col;
      /* if the next column after the up is a non-counting one, set its
	 down pointer to the current source column; this might be reset
	 again if the current column is followed by a non-counting one */
      if (curr_sou_col->up->right
	  && curr_sou_col->up->cindex == curr_sou_col->up->right->cindex)
	curr_sou_col->up->right->down = curr_sou_col;
    }

  curr_sou_col->composite
    = corresponding_cmp_col (curr_block->composite->columns, 
			     curr_sou_col->cindex);
  if (curr_sou_col->explicit_column_count > 1)
    {
      curr_sou_col->composite_end
	= corresponding_cmp_col (curr_block->composite->columns,
				 curr_sou_col->cindex 
				 + curr_sou_col->explicit_column_count - 1);
    }
  else
    curr_sou_col->composite_end = curr_sou_col->composite;
  if (curr_sou_col->composite->down == NULL)
    curr_sou_col->composite->down = curr_sou_col;

  if (*curr_sou_col->text == '#')
    curr_sou_col->space_flag = TRUE;
  else
    curr_sou_col->space_flag = FALSE;
  
  if (*curr_sou_col->text == '0')
    curr_sou_col->zero_flag = TRUE;
  else
    curr_sou_col->zero_flag = FALSE;

  if (!curr_sou_col->explicit_delim)
    {
      /* precompensate for an extra space that we'll insert in aligning
	 if two text entries are separated by a zero-width delimiter */
      if (curr_sou_col->text_entry_flag
	  && curr_sou_col->left
	  && curr_sou_col->left->index > 0
	  && curr_sou_col->left->text_entry_flag
	  && !*curr_sou_col->left->composite->delim)
	{
	  if (++curr_sou_col->left->visible_width 
	      > curr_sou_col->left->composite->maxwidth)
	    ++curr_sou_col->left->composite->maxwidth;
	}
    }
}

static void
apocopation_setup (char apoc)
{
  curr_sou_col->apocopation_flag = apoc;
  curr_sou_col->columns_occupied = 
    curr_composite->column_count - curr_line->column_count;
  curr_line->column_count = curr_composite->column_count;
  curr_sou_col->complex = curr_sou_col;
  ++cindex;
  add_filler ();
}

/* FIXME: this should be able to be merged with the relevant code from
   basic_source_setup, perhaps as a subr which is called by both of
   them */
static void
add_filler ()
{
  int i, todo;

  /* insert empty columns to compensate for the multiple one */
  for (i = 1, todo = curr_sou_col->columns_occupied; i < todo; ++i)
    {
      /* remember, this loop moves curr_sou_col! */
      new_source_column ();
      curr_sou_col->filler_flag = TRUE;
      curr_sou_col->text_entry_flag = TRUE;
      curr_sou_col->text = xstrdup ("");
      curr_sou_col->index = curr_index++;
      list_add (curr_line->columns, curr_sou_col);
      curr_sou_col->parent = curr_line->columns->last;
      curr_sou_col->left = prev(curr_sou_col)->data;
      curr_sou_col->cindex = cindex++;
      curr_sou_col->left->right = curr_sou_col;
      if (list_len (curr_sources) > 1)
	curr_sou_col->up = 
	  corresponding_sou_col (((Line *)(curr_sources->last->prev->data))->columns,
				 curr_sou_col->cindex);
      if (curr_sou_col->up)
	{
	  curr_sou_col->up->down = curr_sou_col;
	  /* if the next column after the up is a non-counting one, set its
	     down pointer to the current source column; this might be reset
	     again if the current column is followed by a non-counting one */
	  if (curr_sou_col->up->right
	      && curr_sou_col->up->cindex == curr_sou_col->up->right->cindex)
	    curr_sou_col->up->right->down = curr_sou_col;
	}

      curr_sou_col->composite = curr_sou_col->composite_end = 
	corresponding_cmp_col (curr_block->composite->columns, 
			       curr_sou_col->cindex);
      if (curr_sou_col->composite->down == NULL)
	curr_sou_col->composite->down = curr_sou_col;
      curr_sou_col->complex = curr_sou_col->left->complex;
    }
}
#if 0
static int
safe_sou_index ()
{
  int i;
  for (i = curr_sou_col->index - non_counting_columns; 
       i > curr_block->composite->column_count - 1; 
       --i)
    ;
  return i;
}
#endif

/* If a text entry ends with `&<DIGITS>' it spans that number of 
   columns; otherwise it spans one column if it is single grapheme,
   or the entire complex otherwise.
 */
static int
is_multi_col (Source_column *sp)
{
  Uchar *s;

  if (sp->explicit_column_count > 1)
    return 1;
  else
    s = sp->text;

  while (*s)
    if (*s == '.' || *s == '-' || *s == '_' || *s == ':')
      return 1;
    else
      ++s;
  return 0;
}

static int
explicit_column_count (Uchar *s)
{
  if (NULL != (s = strrchr (s, '&')) && isdigit (*(s+1)))
    {
      kill_explicit_count(s);
      return atoi (s+1);
    }
  return -1;
}

void
kill_explicit_count (Uchar *s)
{
  if (NULL != (s = strrchr (s, '&')) && isdigit (*(s+1)))
    *s = '\0';
}

static void
new_source_column ()
{
  Source_column *new = xcalloc (1,sizeof (Source_column));
  new->left = new->right = new->up = new->down = new->complex = NULL;
  new->composite = NULL;
  new->text = new->complex_text = NULL;
  new->index = new->cindex = new->columns_occupied = new->visible_width = 0;
  new->line = curr_line;
  new->delim = "";
  new->text_entry_flag 
    = new->apocopation_flag 
    = new->align_override_flag 
    = new->done
    = new->zero_flag
    = new->space_flag
    = new->filler_flag 
    = new->needs_phantom 
    = new->digit_phantom 
    = new->complex_has_variant
    = new->explicit_delim
    = new->explicit_ncols = FALSE;
  curr_sou_col = new;
}

static void
new_line (enum Line_type_e type, Uchar *lp)
{
  Line *new = xcalloc (1,sizeof (Line));

  new->file = error_file ? error_file : curr_file->name;
  new->linenum = curr_file->line
		 + ((error_line > -1) ? error_line : 0);
  new->type = type;
  new->line = xstrdup (lp);
  new->columns = list_create (LIST_DOUBLE);
  new->column_count = 0;
  new->col_count_err_given = FALSE;
  curr_notes = new->notes = list_create (LIST_DOUBLE);
  curr_line = new;
}

static Uchar *_entryp;
static Uchar *savep, saved;
static void
get_entry_init (Uchar *lp)
{
  _entryp = lp;
  savep = NULL;
  saved = '\0';
}

static Uchar *
get_composite_entry (Uchar **delimp)
{
  static Uchar delim;
#if 0
  Boolean quit_on_control_n = FALSE;
#else
  Boolean quit_on_close_curly = FALSE;
#endif

  /* special case for start of line */
  if (NULL == savep)
    {
      savep = _entryp;
      while (*savep && !isspace(*savep))
	++savep;
      delim = ' ';
    }
  else
    {
      if (saved)
	*savep = saved;
      _entryp = savep;

      /* Special-case ellipsis */
      if (strlen(_entryp) > 2 && !strncmp(_entryp, "...", 3))
	{
	  ;
	}
      else
	{
	  while (*_entryp && (MTX_ISDELIM(*_entryp) || '\'' == *_entryp))
	    ++_entryp;
	  
	  if ('\0' == *_entryp)
	    {
	      *delimp = NULL;
	      return NULL;
	    }
	  
	  if ('/' == *_entryp && isspace(_entryp[-1]) && isspace(_entryp[1]))
	    {
	      savep = _entryp + 2;
	      saved = *savep;
	      *savep = '\0';
	      *delimp = &delim;
	      return "/";
	    }
	}

      savep = _entryp;

      while (/**savep == LHB || *savep == RHB ||*/
	     *savep == '[' || *savep == ']')
	++savep;

      if (*savep == '{')
	quit_on_close_curly = TRUE;

      /* Move savep to the end of the composite column's token */
      if (strlen(savep) > 2 && !strncmp(savep, "...", 3))
	{
	  /* Special-case ellipsis */
	  savep += 4;
	}
      else
	{
	  while (*savep && (!MTX_ISDELIM (*savep) && '\'' != *savep))
	    {
	      if (*savep == '{' && !quit_on_close_curly)
		break;
	      if (*savep == '}' && quit_on_close_curly)
		{
		  if ('}' == savep[1])
		    ++savep;
		  if ('-' == savep[1] || isspace(savep[1]))
		    ++savep;
		  break;
		}
	      ++savep;
	    }
	}
      if (*savep == '}')
	{
	  ++savep;
	  delim = '\0';
	}
      else if (*savep == '{')
	delim = '\0';
      else
	{
	  if ('\0' == *savep)
	    delim = '\n';
	  else
	    delim = *savep;
	}
    }

  saved = *savep;
  *savep = '\0';
  *delimp = &delim;
  return _entryp;
}

/* FIXME: both this and get_composite_entry should understand
   {...} diri notations */
static Uchar *
get_source_entry (Boolean *t_e_flag, char *apoc_flag, Uchar *explicit_delim_charp)
{
  if (NULL != savep)
    {
      *savep = saved;
      _entryp = savep;
    }

  while (isspace(*_entryp))
    ++_entryp;

  if ('\0' == *_entryp)
    return NULL;

  savep = _entryp+1;

  if (!single_char_token(*_entryp))
    {
      *t_e_flag = TRUE;
      if ('^' == *_entryp || '$' == *_entryp)
	{
	  int note_flag = 0;
	  if (isdigit(_entryp[1]))
	    {
	      Uchar *note = &_entryp[1];
	      Uchar *tmp = &_entryp[2];
	      while (isdigit(*tmp))
		++tmp;
	      if ('^' == *tmp)
		{
		  note_flag = 1;
		  *tmp = '\0';
		  _entryp = tmp+1;
		  curr_sou_col->notemark = xstrdup(note);
		}
	    }
	  *apoc_flag = *_entryp;
	  ++_entryp;
	  while (*savep)
	    ++savep;
	}
      else
	{
	grab_more:
	  while (*savep && !isspace(*savep) && !single_char_token(*savep))
	    ++savep;
	  /* if it's '\\', grab the next delim also; we will have stopped
	     the delimiter itself */
	  if ('\\' == savep[-1])
	    ++savep;
	  /* if it's gal-zu or GAL.ZU or DI/KI grab the next token as well */
	  else if (*savep 
		   && ('-' == *savep || '.' == *savep || '/' == *savep)
		   && !isspace(savep[1]) && !single_char_token(savep[1]))
	    {
	      savep += 2;
	      goto grab_more;
	    }
	}
    }
  else
    {
      while (*savep && meta_info_char(*savep))
	++savep;
      if ('\\' == *savep)
	{
	  *savep++ = '\0';
	  *explicit_delim_charp = *savep++;
	}
      else
	*explicit_delim_charp = '\0';
    }
  
  saved = *savep;
  *savep = '\0';
  return _entryp;
}

static int
matches_matrix (Uchar *lp)
{
  while (*lp && isspace(*lp))
    ++lp;
  if (!strncmp (lp, "@matrix", 7))
    return 1;
  else if (!strncmp (lp, "<matrix>", 8))
    return 1;
  else if (!strncmp (lp, "@begin{matrix}", strlen("@begin{matrix}")))
    return 1;
  else
    return 0;
}

static int
matches_end_matrix (Uchar *lp)
{
  while (*lp && isspace(*lp))
    ++lp;
  if (!strncmp (lp, "@end matrix", 11))
    return 1;
  else if (!strncmp (lp, "</matrix>", 8))
    return 1;
  else if (!strncmp (lp, "@end{matrix}", strlen("@end{matrix}")))
    return 1;
  else
    return 0;
}

static int
blank_line (Uchar *s)
{
  while (*s && isspace(*s))
    ++s;
  return *s == '\0';
}

static Source_column *
corresponding_sou_col (List *lp, int n)
{
  Source_column *sp;
  for (sp = list_first(lp); sp; sp = list_next(lp))
    {
      if (sp->cindex == n)
	break;
    }
  if (!sp)
    return list_last(lp);
  else
    return sp;
}

static Composite_column *
corresponding_cmp_col (List *lp, int n)
{
  Composite_column *cp;
  for (cp = list_first(lp); cp; cp = list_next(lp))
    {
      if (cp->index == n)
	break;
    }
  if (!cp)
    return list_last(lp);
  else
    return cp;
}

void *
list_nth (List *lp, int n)
{
  if (NULL == lp)
    return NULL;
  if (n < 0)
    {
      if (lp->type == LIST_SINGLE)
	fatal ();
      for (lp->rover = lp->last; n++ && lp->rover; lp->rover = lp->rover->prev)
	;
    }
  else
    {
      for (lp->rover = lp->first; n-- && lp->rover; lp->rover = lp->rover->next)
	;
    }
  return lp->rover ? lp->rover->data : NULL;
}

int
visible_width (Uchar *s)
{
#if 1
  /*fprintf(stderr,"visible width of %s\n",s);*/
  return mbstowcs(NULL,s,0);
#else
  int i;
  for (i = 0; *s; ++s)
    {
      if (*s > 31)
	++i;
    }
  return i;
#endif
}

static Composite_column *fm_comp_col;
static List *fm_list;
static int terminal_maxwidth = 0;

struct FM
{
  int last_col;
  int width_left;
  Source_column *source;
};
typedef struct FM Fm;

static void
fix_multis ()
{
  Source_column *cp = multi_column_entries->first->data;

  fm_list = list_create (LIST_DOUBLE);
  list_exec (multi_column_entries, fm_make_list);

  for (fm_comp_col = (Composite_column *)(cp->composite->line->columns->first->data);
       fm_comp_col != NULL && list_len (fm_list);
       fm_comp_col = fm_comp_col->right)
    {
      /* find the max length of the final segment of all multis that
	 end in this column; set the composite col maxwidth to that if
	 it's greater than the existing maxwidth; subtract the composite 
	 col maxwidth from any remaining multi's that do not finish in 
	 this column */
      terminal_maxwidth = 0;
      list_exec (fm_list, find_terminal_maxwidth);
      /* if delim is empty, we want to ensure a maxwidth that includes a
	 following space of padding for the text entry */
      if (terminal_maxwidth > fm_comp_col->maxwidth - ('\0' == *fm_comp_col->delim))
	fm_comp_col->maxwidth = terminal_maxwidth;
      adjust_multis ();
    }
  list_free (multi_column_entries, NULL);
  multi_column_entries = NULL;
}
static void
fm_make_list (void *vp)
{
  Source_column *sp = vp, *sp2;
  Fm *fmp = xcalloc (1,sizeof (Fm));
  for (sp2 = sp; sp2; sp2 = sp2->right)
    if (sp2->right == NULL
	|| sp2->right->composite->complex_index != sp->composite->complex_index)
      break;
  fmp->last_col = sp2->cindex;
  fmp->width_left = sp->visible_width;
  fmp->source = sp;
  list_add (fm_list, fmp);
}

static void
find_terminal_maxwidth (void *vp)
{
  Fm *fmp = vp;
  if (fmp->last_col == fm_comp_col->index)
    {
      if (fmp->width_left > terminal_maxwidth)
	terminal_maxwidth = fmp->width_left;
#if 0
      if (!fm_comp_col->right || fm_comp_col->complex != fm_comp_col->right->complex)
#endif
	/* I don't understand why this could ever be right now ... */
	/*	fm_comp_col->needs_phantom = TRUE; */
    }
}

static void
adjust_multis ()
{
  List_node *lp, *nextlp;

  for (lp = fm_list->first; lp != NULL; )
    {
      Fm *fmp = lp->data;
      if (fmp->last_col == fm_comp_col->index)
	{
	  nextlp = lp->next;
	  list_delete (fm_list, lp, list_xfree);
	  lp = nextlp;
	}
      else
	{
	  if (fmp->source->index <= fm_comp_col->index)
	    fmp->width_left -= 
	      fm_comp_col->maxwidth + (*fm_comp_col->delim != '\0');
	  lp = lp->next;
	}
    }
}

struct FileLine
matrix_location ()
{
  struct FileLine fl;
  fl.f = error_file ? error_file : curr_file->name;
  fl.l = curr_file->line + ((error_line > -1) ? error_line : 0);
}

/********************************************************
 * stuff for DA library
 */
int
opts (int o, char *c)
{
  int ret = 0;
  switch (o)
    {
    case 'a':
      do_align = TRUE;
      break;
    case 'b':
      do_warn_blanks = TRUE;
      break;
    case 'c':
      do_cmp = TRUE;
      break;
    case 'd':
      need_vars = do_debug = TRUE;
      pf_vars_collapse_variants = TRUE;
      break;
    case 'e':
      do_encapsulate = TRUE;
      break;
    case 'E':
	error_file = xstrdup (c);
	{
	  char *tmp;
	  if (NULL != (tmp = strstr (error_file, "::")))
	    {
	      *tmp = '\0';
	      tmp += 2;
	      error_line = atol (tmp);
	    }
	}
	break;
    case 'f':
      fragments = TRUE;
      break;
    case 'k':
      do_aka_primary = TRUE;
      break;
    case 'l':
      do_lines_in_srcs = TRUE;
      break;
    case 'm':
      do_minimal = TRUE;
      break;
    case 'o':
      output_fn = xstrdup (c);
      break;
    case 'p':
      do_stdin = TRUE;
      break;
    case 'r':
      do_raw = TRUE;
      break;
    case 's':
      do_sort_sources = TRUE;
      break;
    case 't':
      need_vars = do_tex = TRUE;
      break;
    case 'v':
      do_check = TRUE;
      break;
    case 'x':
      need_vars = do_expand = TRUE;
      break;
    case 'X':
      do_xml = TRUE;
      break;
#if 0
    case '4':
      need_vars = do_sf4 = TRUE;
      pf_vars_collapse_variants = FALSE;
      break;
#endif
    default:
      ret = 1;
      break;
    }
  return ret;
}

void
help ()
{
  printf ("  a  align\n");
  printf ("  b  warn about blank entries\n");
  printf ("  c  composite text output\n");
  printf ("  d  debug\n");
  printf ("  E<file>::<line>  initialize error-reporting file/line to given values\n");
  printf ("  e  encapsulate output (e.g., add `\\bye' if doing TeX output)\n");
  printf ("  f  assume all input is fragments of a matrix\n");
  printf ("  l  list lines in sources\n");
  printf ("  k  make alternative source primary and vice-versa\n");
  printf ("  m  minimal matrix (no reconstructed texts etc.)\n");
  printf ("  o<file> output file name is <file>\n");
  printf ("  p  read from pipe/stdin\n");
  printf ("  r  raw output\n");
  printf ("  s  sort sources\n");
  printf ("  t  tex output\n");
  printf ("  v  verify matrix and exit (default)\n");
  printf ("  x  expand positive variants\n");
  printf ("  X  XML output (mtx namespace/vocabulary)\n");
  printf ("  4  sf4 composite with variants\n");
}

const char *prog = "matrix";
int major_version = 1;
int minor_version = 1;
const char *usage_string = "[-acdrt] <files>";

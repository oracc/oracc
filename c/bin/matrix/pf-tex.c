#include <psd_base.h>
#if 0
#include <charname.h>
#include <dactype.h>
#endif
#include "pf_lib.h"
#include "variants.h"
#include "warning.h"
#include <ctype.h>

#define xmalloc malloc
#define xrealloc realloc

#define DAC_ISVOWEL(x) (strchr("aeiuAEIU",x))
#define DAC_ISALNUM(x) isalnum(x)

/** IMPORTANT NOTE: this module uses the `done' flag in columns, but
    only where necessary; the code should really be revised to use
    `done' systematically---until then one can't assume that a FALSE
    done flag means the column hasn't been printed */

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))
#define P4(f,a1,a2,a3,a4) fprintf(ipf->output_fp,(f),(a1),(a2),(a3),(a4))

#define breakage_string() (breakage_crosses_boundary && Lbreakage ? "LR" \
                           : (breakage_crosses_boundary ? "L" \
			      : (Lbreakage ? "R" : "")))

static int tex_output_col_count, tex_template_phantoms, 
  tex_cmp_phantoms, tex_src_phantoms, tex_zeroes_skipped;
static void tex_template_columns (void *vp);
static Uchar *tex_cic_width (Uchar *dp);
static Uchar *tex_hack_tabloc (Uchar *t);

static Uchar *
tex_unhash (Uchar *txt)
{
  Uchar *src,*dest;
  src = dest = txt;
  while (*src)
    {
      if (*src == '#')
	++src;
      else if (src > dest)
	*dest++ = *src++;
      else
	{
	  ++dest;
	  ++src;
	}
    }
  *dest = '\0';
  return txt;
}

static void
tex_before_outers()
{
  P0("\\macrofile{matrix}\n");
  P0("\\begin{score}\\singlespace\n");
}

static void
tex_each_outer()
{
}

static void
tex_between_outers()
{
}

static void
tex_after_outers()
{
  P0("\\end{score}\n"); /*FIXME: should be done in tex_between_outers */
  if (do_encapsulate)
    P0("\\bye\n");
}

static void
tex_each_text()
{
}

static void
tex_before_block()
{
  P0("\\BeginBlock\n");
}

static void
tex_after_block()
{
  P0("\\EndBlock\n");
}

static void
tex_before_sources()
{
}

static void
tex_between_sources()
{
}

static void
tex_after_sources()
{
}

static void
tex_template_columns (void *vp)
{
  static Composite_column*cp;
  static Uchar *tmp;
  cp = vp;

  if (!cp->index)
    return;

  if (cp->zero_flag)
    {
      Composite_column*cp_complex = cp->complex;
      
      if (cp_complex->complex_has_variant)
	{
	  if (cp_complex->complex_columns > 1)
	    {
	      if (cp->needs_phantom)
		{
		  P0("&#\\phantom\\@\\Ta");
		  ++tex_template_phantoms;
		}
	      else
		{
		  P0("&#\\@\\Th");
		}
	      return;
	    }
	}
    }

  if (cp->right
      && cp->right->zero_flag
      && cp->right->complex->complex_has_variant
      && cp->right->complex_columns == 1
      && *cp->delim == ' ')
    tmp = "\\@\\Tn";
  else
  switch (*cp->delim)
    {
    case '\'':
    case '-':
    case '+':
      tmp = "\\@\\Th";
      break;
    case ' ':
      tmp = "\\@\\Ts";
      break;
    case '\n':
      tmp = "\\@\\Te";
      break;
    case '.':
      tmp = "\\@\\Td";
      break;
    case ':':
      tmp = "\\@\\Tc";
      break;
    case '\0':
      tmp = "\\@\\Ta";
      break;
    default:
      fatal();
      break;
    }

  P1("&#%s",tmp);

  if (cp->needs_phantom)
    {
      P0("&#\\phantom\\@\\Ta");
      ++tex_template_phantoms;
    }
}

static void
tex_before_composite()
{
  tex_template_phantoms = tex_cmp_phantoms = 0;
  P0("\\Template#");
  list_exec (iterate_line->columns, tex_template_columns);
  P0("\\cr\n");
}

static void
tex_after_composite()
{
  if (iterate_composite_column->needs_phantom)
    {
      if (!iterate_composite_column->zero_flag
	  || (iterate_composite_column->complex->complex_has_variant
	      && iterate_composite_column->complex->complex_columns == 1))
	P0("&");
      P0("\\phantom{}");
      ++tex_cmp_phantoms;
    }
  P0("\\cr\n");
  if (tex_template_phantoms != tex_cmp_phantoms)
    {
      vwarning2(iterate_line->file,iterate_line->linenum,
	       "%s: wrong number of phantoms in composite line (has %d, not %d)",
	       *iterate_block->composite->name 
	       ? (Uchar*)(iterate_block->composite->name) : (Uchar*)".",
	       tex_cmp_phantoms, tex_template_phantoms);
    }
}

static Uchar *
tex_composite_delim (Composite_column*cp)
{
  static Uchar *ret;
  if (cp->zero_flag)
    {
#if 1
      ret = "";
#else
      if (cp->complex->complex_has_variant == FALSE 
	  || cp->complex->complex_columns != 1)
	ret = "";
      else
	ret = "\\Rs";
#endif
    }
  else if (cp->right && cp->right->zero_flag 
	   && (' ' == *cp->right->delim || '\n' == *cp->right->delim))
    {
      if (cp->right->complex->complex_has_variant 
	  && cp->right->complex->complex_columns != 1)
	ret = "";
      else
#if 1
	ret = "";
#else
	ret = "\\Rs";
#endif
    }
  else
  switch (*cp->delim)
    {
    case '\'':
    case '-':
    case '+':
      ret = "\\Rh";
      break;
    case ' ':
      ret = "\\Rs";
      break;
    case '\n':
      ret = "\\Re";
      break;
    case '.':
      ret = "\\Rd";
      break;
    case ':':
      ret = "\\Rc";
      break;
    case '\0':
      ret = "\\Ra";
      break;
    default:
      fatal();
      break;
    }
  return ret;
}

static void
tex_each_composite_column()
{
  if (!iterate_composite_column->index)
    {
      /* only print the line number if there is no reconstructed line */
      if (iterate_line->block->composite == iterate_line->block->reconstructed)
	P1("\\Num{%s}", iterate_line->name);
    }
  else
    {
      if (iterate_composite_column->space_flag)
	P0("\\quad");
      else if (!iterate_composite_column->zero_flag
	       || (iterate_composite_column->complex->complex_has_variant
		   && iterate_composite_column->complex->complex_columns == 1))
	{
	  if (!iterate_composite_column->zero_flag)
	    P1("%s", tex_unhash(iterate_composite_column->text));
	  P0(tex_composite_delim(iterate_composite_column));
	}
    }
}

static void
tex_between_composite_columns()
{
  if (!iterate_composite_column->zero_flag 
      || (iterate_composite_column->complex->complex_has_variant
	  && iterate_composite_column->complex_columns == 1))
    P0("&");
  if (iterate_composite_column->needs_phantom)
    {
      if (iterate_composite_column->index == 0)
	fatal();
      P0("&\\phantom{}");
      ++tex_cmp_phantoms;
    }
}

static void
tex_before_reconstructed()
{
  tex_cmp_phantoms = 0;
}

static void
tex_after_reconstructed()
{
  P0("\\cr\n");
  if (tex_template_phantoms != tex_cmp_phantoms)
    {
      vwarning2(iterate_line->file,iterate_line->linenum,
	       "%s: wrong number of phantoms in reconstructed line (has %d, not %d)",
	       iterate_block->composite->name,
	       tex_cmp_phantoms, tex_template_phantoms);
    }
  tex_cmp_phantoms = 0;
}

static Composite_column*
get_corresponding_complex (List *cols, int nth)
{
  Composite_column*cp;
  for (cp = list_first(cols); cp; cp = list_next(cols))
    if (nth == cp->complex_index)
      break;
  return cp;
}

static void
tex_each_reconstructed_column()
{
  if (!iterate_reconstructed_column->index)
    {
      P1("\\Num{%s}", iterate_line->block->reconstructed->name);
    }
  else
    {
      /* print a multispan opening with same # of cols as complex in 
	 composite; print relevant number of phantoms;
	 print all columns belonging to current complex;
	 print end-of-multispan */
      if (*iterate_reconstructed_column->left->delim == ' ')
	{
	  Composite_column*ccomplex 
	    = get_corresponding_complex (iterate_line->block->composite->columns,
					 iterate_reconstructed_column->complex_index);
	  if (NULL == ccomplex)
	    {
	      vwarning2(iterate_line->file,iterate_line->linenum,"%s",
		       "reconstructed text has more columns than composite");
	      return;
	    }
	  P1("&\\Ms{%d}", ccomplex->complex_columns);
	  if (ccomplex->complex_phantoms)
	    {
	      P1("\\phantoms{%d}", ccomplex->complex_phantoms);
	      tex_cmp_phantoms += ccomplex->complex_phantoms;
	    }
	  while (1)
	    {
	      P1("%s", tex_unhash(iterate_reconstructed_column->text));
	      if (*iterate_reconstructed_column->delim != ' '
		  && *iterate_reconstructed_column->delim != '\n')
		P1("%s", iterate_reconstructed_column->delim);
	      if (*iterate_reconstructed_column->delim != ' '
		  && *iterate_reconstructed_column->delim != '\n')
		iterate_reconstructed_column = iterate_reconstructed_column->right;
	      else
		break;
	    }
	  P0(tex_composite_delim(iterate_reconstructed_column));
	  P0("\\Hw");
	}
    }
}

static void
tex_between_reconstructed_columns()
{
}

static void
tex_before_source()
{
  tex_src_phantoms = tex_output_col_count = tex_zeroes_skipped = 0;
}

static void
tex_after_source()
{
  P0("\\cr\n");
  if (tex_output_col_count + tex_zeroes_skipped != iterate_block->composite->column_count)
    vwarning2(iterate_line->file,iterate_line->linenum,
	     "%s:%s: TeX columns output != columns in composite line (%d != %d)",
	     iterate_block->composite->name, iterate_line->name,
	     tex_output_col_count + tex_zeroes_skipped, 
	     iterate_block->composite->column_count);
  if (tex_template_phantoms != tex_src_phantoms
      && !iterate_block->composite->col_count_err_given)
    vwarning2(iterate_line->file,iterate_line->linenum,
	     "%s:%s: wrong number of phantoms in source line (has %d, not %d)",
	     iterate_block->composite->name, iterate_line->name,
	     tex_src_phantoms, tex_cmp_phantoms);
}

static Uchar *
tex_source_col_content (Uchar *s)
{
  Uchar *tmp = s;
  switch (*s)
    {
    case '+':
      tmp = "\\Sprs";
      break;
    case '\'':
    case '-':
      tmp = "\\Sabs";
      break;
    case '*':
      tmp = "\\Sdam";
      break;
    case ',':
      tmp = "\\Strc";
      break;
    case '.':
      tmp = "\\Sbrk";
      break;
    case '\0':
      tmp = "\\Sempty";
      break;
    default:
      fatal();
      break;
    }
  return tmp;
}

static Uchar *
tex_cic_width (Uchar *dp)
{
  Uchar *ret = "0pt";
  switch (*dp)
    {
    case ' ':
      ret = "\\spacewidth";
      break;
    case '\'':
    case '-':
    case '+':
      ret = "\\hyphwidth";
      break;
    case ':':
      ret = "\\colonwidth";
      break;
    case '.':
      ret = "\\dotwidth";
      break;
    case '\0':
    case '\n':
      ret = "0pt";
      break;
    default:
      fatal ();
      break;
    }
  return ret;
}

static Uchar *
tex_meta (Uchar c)
{
  static Uchar ret[2];
  switch (c)
    {
    case '/':
      return "\\MTXnl{}";
      break;
    case '?':
      return "\xBD";
      break;
    case '!':
      return "\xBE";
      break;
    default:
      ret[0] = c;
      ret[1] = '\0';
      break;
    }
  return ret;
}

static Uchar tex_hack_text_buf[1024];
#define tht_is_meta(c)  ((c)=='\'' || (c)=='"' || (c) == '?' || (c) == '!')
#define tht_is_delim(c) ((c)=='-' || (c)=='.')

static Uchar *
tht_set_pointers (Uchar *s, Uchar **txt_ret, Uchar **meta_ret, Uchar **delim_ret)
{
  static Uchar meta[8], delim[2], save = '\0', *to_zero;
  Uchar *entry = s;
  Uchar *dest;

  *meta = *delim = '\0';

  if (save)
    *to_zero = save;
  to_zero = NULL;

  while (*s && !tht_is_meta(*s) && !tht_is_delim(*s))
    ++s;
  if (tht_is_meta(*s))
    {
      to_zero = s;
      for (dest = meta; tht_is_meta(*s);)
	*dest++ = *s++;
      *dest = '\0';
    }
  if (NULL == to_zero)
    to_zero = s;
  if (tht_is_delim(*s))
    {
      delim[0] = *s;
      delim[1] = '\0';
      ++s;
    }
  else
    delim[0] = '\0';

  save = *to_zero;
  *to_zero = '\0';
  *txt_ret = entry;
  *meta_ret = meta;
  *delim_ret = delim;
  return s;
}

static Uchar*
tex_hack_text (Uchar *s)
{
  Uchar *tht_txt, *tht_meta, *tht_delim, *buf;
  *tex_hack_text_buf = '\0';
  buf = tex_hack_text_buf;
  while (1)
    {
      s = tht_set_pointers (s, &tht_txt, &tht_meta, &tht_delim);
      if (*tht_txt)
	{
	  int breakage = 0;
	  Uchar *tmp, *dest;
	  for (dest = tmp = tht_meta; *tmp; ++tmp)
	    {
	      if ('\'' == *tmp)
		breakage = 1;
	      else if ('"' == *tmp)
		breakage = 2;
	      else
		*dest++ = *tmp;
	    }
	  *dest = '\0';
	  if (breakage == 1)
	    {
	      Uchar *dst, *tmp;
	      strcat(buf, "\\[");
	      dst = tex_hack_text_buf + strlen(tex_hack_text_buf);
	      for (tmp = tht_txt; !DAC_ISVOWEL(*tmp); ++tmp)
		*dst++ = *tmp;
	      strcpy(dst, "\\\\");
	      dst += strlen(dst);
	      *dst++ = *tmp++;
	      strcpy(dst, "");
	      strcat(dst, tmp);
	      strcat(dst,"\\]");
	    }
	  else if (breakage == 2)
	    {
	      strcat(buf, "\\brackets{");
	      strcat (buf, tht_txt);
	    }
	  else
	    {
	      strcat (buf, tht_txt);
	    }
	  for (tmp = tht_meta; *tmp; ++tmp)
	    strcat (buf, tex_meta(*tmp));
	  if (breakage == 2)
	    strcat (buf, "}");
	  strcat (buf, tht_delim);
	}
      else
	break;
    }
  return tex_hack_text_buf;
}

static char
tex_ic_alignment (Uchar c)
{
  if (DAC_ISALNUM(c))
    return 'L';
  else
    return 'C';
}

static Uchar *
tex_build_ic (Source_column*sp, Uchar *cs)
{
  static Uchar ic[128];
  
  if ('~' == *sp->text)
    ++sp->text;
  if (cs)
    sprintf (ic, "\\%s{%s}", cs, sp->text);
  else
    {
      sprintf (ic, "\\%cic{%s}{\\hss %s\\hss}",
	       tex_ic_alignment(*sp->text),
	       tex_cic_width(sp->left->composite->delim),
	       sp->text);
    }
  return ic;
}

static Uchar expand_pre[1024];
static void
tex_expand_pre (int cols, int phantoms, Uchar *txt)
{
  if ('[' == *txt)
    {
      Uchar *delim = txt+1;
      if ('-' == *delim)
	delim = "\\HY";
      else if ('\n' == *delim)
	delim = "";
      sprintf (expand_pre + strlen(expand_pre),
	       "&\\Ms{%d}\\phantoms{%d}\\iBreakage{%s}",
	       cols+phantoms, phantoms, delim);
    }
  else
    {
      sprintf (expand_pre + strlen(expand_pre),
	       "&\\Ms{%d}\\phantoms{%d}",
	       cols+phantoms, phantoms);
    }
}

static void
tex_each_source_column()
{
  Boolean ends_with_phantom = FALSE;
  int complex_phantoms = -1, complex_filler = 0;
  static Boolean breakage_crosses_boundary = FALSE;
  int expand_cols_done = 0, expand_phantoms = 0, expand_zeroes = 0;

  if (iterate_source_column->done || iterate_source_column->filler_flag)
    return;

  if (do_expand 
      && iterate_source_column->complex == iterate_source_column
      && iterate_source_column->complex_has_variant
      && iterate_source_column->complex_text)
    {
      Source_column *sp = NULL;
      Uchar *endp;

      /* rewrite the source column to have the complex as its text, text_entry_flag
	 to true and columns occupied to be the same as that of the composite complex;
	 also mark all subsequent columns belonging to this complex as done */
      iterate_source_column->text_entry_flag = TRUE;
      if (breakage_crosses_boundary)
	{
	  if ('[' != *iterate_source_column->complex_text)
	    {
	      vwarning2(iterate_line->file, iterate_line->linenum,
		       "inconsistent text: `%s' should begin with '['",
		       print_escaped(iterate_source_column->complex_text));
	      iterate_source_column->text = iterate_source_column->complex_text;
	    }
	  else
	    iterate_source_column->text = iterate_source_column->complex_text + 1;
	  breakage_crosses_boundary = FALSE;
	}
      else
	{
	  Uchar *s = iterate_source_column->complex_text;
	  *expand_pre = '\0';
	  while (!strncmp(s, "\\X", 2))
	    {
	      int cols, phant, zeroes, n;
	      Uchar *txt;
	      s += 2;
	      sscanf(s,"%d/%d/%d/%n", &cols, &phant, &zeroes, &n);
	      s += n;
	      txt = s;
	      if ('[' == *s)
		while (*s && ']' != *s++)
		  ;
	      if (*s)
		{
		  s[-1] = '\0';
		  *s++ = '\0';
		}
	      tex_expand_pre(cols, phant, txt);
	      expand_cols_done += cols;
	      expand_phantoms += phant;
	      expand_zeroes += zeroes;
	    }
	  iterate_source_column->text = s;
	}

#if 0
      if (iterate_source_column->apocopation_flag)
	{
	  /* use a simpler algorithm here: just add the remaining
	     number of phantoms from here on to complex_phantoms */
	  if (complex_phantoms == -1)
	    complex_phantoms = 0;
	  for (sp = iterate_source_column; sp; sp = sp->right)
	    {
	      if (sp->needs_phantom)
		++complex_phantoms;
	      sp->done = TRUE;
	    }
	}
      else
#endif
	{
	  int target_complex_index;
	  for (complex_phantoms = 0, sp = iterate_source_column, 
		 target_complex_index = -1,
		 iterate_source_column->columns_occupied = 0;
	       sp;
	       sp = sp->right)
	    {
	      sp->done = TRUE;
	      if (sp->filler_flag)
		++complex_filler;
	      if (sp->needs_phantom)
		++complex_phantoms;
	      if (sp->composite && target_complex_index != sp->composite->complex_index)
		{
		  iterate_source_column->columns_occupied
		    += (sp->composite->complex_columns - sp->composite->complex_zeroes);
		  tex_zeroes_skipped += sp->composite->complex_zeroes;
		  target_complex_index = sp->composite->complex_index;
		}
	      if (!sp->right
		  || (!iterate_source_column->apocopation_flag
		      && sp->right->complex != iterate_source_column->complex))
		break;
	    }
	  if (sp->needs_phantom && do_square_brackets)
	    {
	      --complex_phantoms;
	      ends_with_phantom = TRUE;
	    }
	  endp = iterate_source_column->text + strlen(iterate_source_column->text) - 2;
	  if (sp && '.' == *sp->text && ']' == *endp)
	    {
	      *endp = '\0';
	      breakage_crosses_boundary = TRUE;
	    }
	  if (iterate_source_column->columns_occupied < 1)
	    {
	      iterate_source_column->columns_occupied = 1; /* even if composite has "0" */
	      if (tex_zeroes_skipped)
		--tex_zeroes_skipped;
	    }
	  if (expand_cols_done || expand_phantoms)
	    {
	      iterate_source_column->columns_occupied -= expand_cols_done;
	      complex_phantoms -= expand_phantoms;
	      assert(complex_phantoms >= 0);
	      if (iterate_source_column->columns_occupied == 0)
		{
		}
	      else if (iterate_source_column->columns_occupied < 0)
		{
		  vwarning2(iterate_line->file,iterate_line->linenum,"%s",
			    "wrong column count in expand");
		}
	      if (complex_phantoms < 0)
		{
		  vwarning2(iterate_line->file,iterate_line->linenum,"%s",
			    "wrong phantom count in expand");
		  complex_phantoms = 0;
		}
	    }
	}
    }
  /* special processing for the siglum column */
  if (0 == iterate_source_column->index)
    {
      if (!iterate_line->altsig || !*iterate_line->altsig)
	P2("\\Sig{%s}{%s}", 
	   iterate_line->name, 
	   tex_hack_tabloc (iterate_line->tabloc->expanded));
      else
	{
	  if (*iterate_line->tabloc->expanded)
	    P3("\\Sig{%s}{(%s) %s}", 
	       iterate_line->name, 
	       iterate_line->altsig, 
	       tex_hack_tabloc (iterate_line->tabloc->expanded));
	  else
	    P2("\\Sig{%s}{(%s)}", 
	       iterate_line->name, 
	       iterate_line->altsig);
	}
      ++tex_output_col_count;
    }
  else if ((iterate_source_column->composite->zero_flag
	    || iterate_source_column->composite->space_flag)
	   && ('+' == *iterate_source_column->text
	       || '-' == *iterate_source_column->text
	       || (!do_square_brackets && '.' == *iterate_source_column->text)))
    {
      if (iterate_source_column->complex->composite->complex_has_variant
	  && iterate_source_column->complex->composite->complex_columns != 1)
	{
	  ++tex_zeroes_skipped;
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	}
      else
	{
	  P0("&");
	  ++tex_output_col_count;
	  iterate_source_column->done = TRUE;
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	}
    }
  else if (0 == iterate_source_column->columns_occupied)
    {
      if (*expand_pre)
	{
	  if (complex_phantoms > 0)
	    {
	      int cols, phant, n;
	      sscanf (expand_pre, "&\\Ms{%d}\\phantoms{%d}%n", &cols, &phant, &n);
	      cols += complex_phantoms;
	      phant += complex_phantoms;
	      tex_src_phantoms += complex_phantoms;
	      complex_phantoms = 0;
	      P3("&\\Ms{%d}\\phantoms{%d}%s", cols, phant, expand_pre + n);
	    }
	  else
	    {
	      P0(expand_pre);
	    }
	  P1("\\sp{}%s", iterate_source_column->text);
	  tex_output_col_count += expand_cols_done;
	  tex_src_phantoms += expand_phantoms;
	  if (complex_phantoms > 0)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	      if (complex_phantoms > 1)
		vwarning2(iterate_line->file,iterate_line->linenum,
			"too many complex_phantoms (%d; expected at most 1)",
			complex_phantoms);
	    }
	}
      else
	{
	  Uchar *tmp = tex_build_ic(iterate_source_column, NULL);
	  while (*tmp)
	    P1("%s", tex_meta(*tmp++));
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	}
    }
  else
    {
      if (iterate_source_column->text_entry_flag)
	{
	  Uchar *append = "";
	  Boolean right_needs_phantom = FALSE;

	  if (!do_expand
	      && iterate_source_column->right
	      && !iterate_source_column->right->columns_occupied
	      && !iterate_source_column->right->filler_flag)
	    {
	      append = tex_build_ic (iterate_source_column->right, "Mappend");
	      iterate_source_column->right->done = TRUE;
	      if (iterate_source_column->right->needs_phantom)
		right_needs_phantom = TRUE;
	    }

#if 0
	  if (iterate_source_column->apocopation_flag)
	    {
	      Source_column*sp;
	      iterate_source_column->columns_occupied = 0;
	      for (sp = iterate_source_column; sp; sp = sp->right)
		{
		  if (sp->composite->complex == sp->composite)
		    {
		      iterate_source_column->columns_occupied 
			+= (sp->composite->complex->complex_columns
			    - sp->composite->complex->complex_zeroes);
		      tex_zeroes_skipped += sp->composite->complex_zeroes;
		    }
		}
	    }
#endif

	  if (*expand_pre)
	    P0(expand_pre);
	  if (complex_phantoms > 0)
	    {
	      P2("&\\Ms{%d}\\phantoms{%d}", 
		 iterate_source_column->columns_occupied + complex_phantoms,
		 complex_phantoms/*, append*/);
	      tex_src_phantoms += complex_phantoms;
	    }
	  else if (iterate_source_column->needs_phantom || right_needs_phantom)
	    {
	      P1("&\\Ms{%d}\\phantom{}", iterate_source_column->columns_occupied + 1);
	      ++tex_src_phantoms;
	    }
	  else
	    P1("&\\Ms{%d}", iterate_source_column->columns_occupied);

	  if (iterate_source_column->composite->zero_flag 
	      || iterate_source_column->apocopation_flag
#if 1
	      || iterate_source_column->columns_occupied >= 1
#else
	      || (iterate_source_column->columns_occupied > 1
		  && !iterate_source_column->align_override_flag)
#endif
	      )
	    P1("%s", tex_hack_text(tex_unhash(iterate_source_column->text)));
	  else
	    P1("\\Hw %s", tex_hack_text(tex_unhash(iterate_source_column->text)));

	  if (iterate_source_column->apocopation_flag)
	    P0("\\Hw");

	  if (*append)
	    P0(append);
#if 0
	  if (!iterate_source_column->composite->zero_flag
	      && iterate_source_column->columns_occupied < 2)
	    P0("\\Hw");
#endif
	  tex_output_col_count += iterate_source_column->columns_occupied;
	  tex_output_col_count += expand_cols_done;
	  tex_src_phantoms += expand_phantoms;
	  if (ends_with_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	}
      else if ('.' == *iterate_source_column->text && do_square_brackets)
	{
	  int ncols = 0;
	  Uchar *append = "";
	  Source_column*cp;
	  int nphantoms = 0;
	  Boolean Lbreakage = FALSE;

	  if (breakage_crosses_boundary)
	    {
	      Lbreakage = TRUE;
	      breakage_crosses_boundary = FALSE;
	    }

	  /* set cp to the last column with a break */
	  for (cp = iterate_source_column; cp; cp = cp->right)
	    {
	      if ('.' == *cp->text
		  && cp->complex != cp->left->complex
		  && cp->complex->complex_has_variant)
		{
		  breakage_crosses_boundary = TRUE;
		  cp = cp->left;
		  break;
		}
	      /* DOCUMENT ME: deleting 0-columns on print means that it is
		 better to express omissions guaged by lack of space in notes
		 rather than by using '-' in a 0-column */
	      if ('.' != *cp->text && !cp->composite->zero_flag)
		{
		  cp = cp->left;
		  break;
		}
	      if (cp->composite->zero_flag 
		  && cp->composite->complex->complex_has_variant
		  && cp->composite->complex_columns != 1)
		{
		  ++tex_zeroes_skipped;
		  if (cp->needs_phantom)
		      ++nphantoms;
		}
	      else
		{
		  if (cp->needs_phantom)
		      ++nphantoms;
		  ++ncols;
		}
	      cp->done = TRUE;
	      if (!cp->right)
		break;
	    }
	  if (cp->needs_phantom
	      && do_square_brackets 
	      && (!cp->composite->zero_flag 
		  || cp->composite->complex->complex_columns == 1
		  || !cp->composite->complex->complex_has_variant))
	    {
	      --nphantoms;
	      ends_with_phantom = TRUE;
	    }
	  if (cp->right
	      && !cp->right->columns_occupied
	      && !cp->right->filler_flag)
	    {
	      append = cp->right->text;
	      if ('~' == *append)
		++append;
	      if (cp->right->needs_phantom)
		{
		  if (ends_with_phantom)
		    ++nphantoms;
		  else
		    ends_with_phantom = TRUE;
		}
	      cp->right->done = TRUE;
	    }
	  if (nphantoms)
	    {
	      P4("&\\Ms{%d}\\phantoms{%d}\\Breakage%s{%s}", 
		 ncols + nphantoms, nphantoms, 
		 breakage_string(),
		 append);
	      tex_src_phantoms += nphantoms;
	    }
	  else
	    P3("&\\Ms{%d}\\Breakage%s{%s}", ncols, 
	       breakage_string(),
	       append);
	  if (ends_with_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	  tex_output_col_count += ncols;
	}
      else
	{
	  Uchar *tmp = tex_source_col_content(iterate_source_column->text);
	  Boolean right_needs_phantom = FALSE;

	  P1("&%s{", tmp);
	  for (tmp = &iterate_source_column->text[1]; *tmp; ++tmp)
	    P1("%s", tex_meta(*tmp));

	  if (iterate_source_column->right
	      && !iterate_source_column->right->columns_occupied
	      && !iterate_source_column->right->filler_flag)
	    {
	      Boolean did_curly = FALSE;
	      Uchar ch = *iterate_source_column->right->text;
	      
	      if (ch == '/' || ch == ';' || ch == '%')
		{
		  tmp = iterate_source_column->right->text;
		  P0("\\MTXthinsp{}");
		  while (*tmp)
		    P1("%s", tex_meta(*tmp++));
		}
	      else
		{
		  P0("}");
		  did_curly = TRUE;
		  tmp = tex_build_ic(iterate_source_column->right, NULL);
		  while (*tmp)
		    P1("%s", tex_meta(*tmp++));
		}
	      if (!did_curly)
		P0("}");
	      iterate_source_column->right->done = TRUE;
	      if (iterate_source_column->right->needs_phantom)
		right_needs_phantom = TRUE;
	    }
	  else
	    P0("}");
	  if (iterate_source_column->needs_phantom || right_needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++tex_src_phantoms;
	    }
	  tex_output_col_count += iterate_source_column->columns_occupied;
	}
    }
  *expand_pre = '\0';
}

static void
tex_between_source_columns()
{
}

static void
tex_before_notes()
{
}

static Uchar *sig, *loc, *txt;
static Boolean
begins_with_siglum (Uchar *s)
{
  while (isspace(*s))
    ++s;
  sig = s;
  while (!isspace(*s))
    ++s;
  if (s[-1] == ':')
    {
      *s++ = '\0';
      while (isspace(*s))
	++s;
      txt = s;
      loc = sig;
      while (*loc)
	{
	  if (',' == *loc)
	    break;
	  else
	    ++loc;
	}
      if (*loc)
	*loc++ = '\0';
      return TRUE;
    }
  else
    return FALSE;
}

static void
tex_each_note()
{
  if (begins_with_siglum(iterate_text))
    {
      Tabloc *tp = do_tabloc (loc);
      P2("\\Sig{%s}{%s}", sig, tex_hack_tabloc (tp->expanded));
      P1("&\\Ms{%d}", iterate_line->column_count - 1);
      P1("%s\\Hw\\cr\n", txt);
    }
  else 
    {
      P0("\\scorenote{%\n");
      P0(iterate_text);
      P0("}\n");
    }
}

static void
tex_after_notes()
{
}

static Uchar *
tex_hack_tabloc (Uchar *s)
{
  static Uchar *hack_buf = NULL;
  static int hack_buf_len = 0;
  Uchar *dest;

  if (!hack_buf)
    {
      hack_buf_len = 128;
      hack_buf = xmalloc (hack_buf_len);
    }
  while (1+(2*strlen(s)) > hack_buf_len)
    {
      hack_buf_len *= 2;
      hack_buf = xrealloc (hack_buf, hack_buf_len);
    }
  dest = hack_buf;
  while (*s)
    {
      switch (*s)
	{
	case '-':
	  *dest++ = '-';
	  *dest++ = '-';
	  ++s;
	  while (isspace(*s))
	    ++s;
	  break;
	case '\'':
	  *dest++ = 0xb6;
	  ++s;
	  break;
	default:
	  *dest++ = *s++;
	  break;
	}
    }
  *dest = '\0';
  return hack_buf;
}

Process_functions pf_tex = {
NULL, NULL,
tex_before_outers,
tex_each_outer,
tex_between_outers,
tex_after_outers,
tex_each_text,
tex_before_block,
tex_after_block,
tex_before_sources,
tex_between_sources,
tex_after_sources,
tex_before_composite,
tex_after_composite,
tex_each_composite_column,
tex_between_composite_columns,
tex_before_reconstructed,
tex_after_reconstructed,
tex_each_reconstructed_column,
tex_between_reconstructed_columns,
tex_before_source,
tex_after_source,
tex_each_source_column,
tex_between_source_columns,
tex_before_notes,
tex_each_note,
tex_after_notes,
};

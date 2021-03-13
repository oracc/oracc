/* 
   Generate an XML version of the score for hosting in an XTF
   environment.  This is intended for use only in fragments mode.
 */
#include <psd_base.h>
#include <ctype.h>
#include "pf_lib.h"
#include "variants.h"
#include "gdl.h"

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))
#define P4(f,a1,a2,a3,a4) fprintf(ipf->output_fp,(f),(a1),(a2),(a3),(a4))
#define Pattr(a,v)	fprintf(ipf->output_fp," %s=\"%s\"",(a),(v))
#define PattrInt(a,v)	fprintf(ipf->output_fp," %s=\"%d\"",(a),(v))
#define Pempty()	fprintf(ipf->output_fp,"/>")

#define xmalloc malloc
#define xrealloc realloc

#define breakage_string() (breakage_crosses_boundary && Lbreakage ? "LR" \
                           : (breakage_crosses_boundary ? "L" \
			      : (Lbreakage ? "R" : "")))

static int curr_complex, curr_s_complex;
static int mtx_output_col_count, mtx_template_phantoms, 
  mtx_cmp_phantoms, mtx_src_phantoms, mtx_zeroes_skipped;
static Uchar *mtx_cic_width (Uchar *dp);
static Uchar *mtx_hack_tabloc (Uchar *t);
extern void gdl_print(FILE*fp,const unsigned char *gdlinput);

static Uchar *
mtx_unhash (Uchar *txt)
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
mtx_before_outers()
{
  P0("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  P0("<matrix xmlns=\"http://oracc.org/ns/mtx/1.0\" xmlns:mtx=\"http://oracc.org/ns/mtx/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\">");
}

static void
mtx_each_outer()
{
}

static void
mtx_between_outers()
{
}

static void
mtx_after_outers()
{
  P0("</matrix>");
}

static void
mtx_each_text()
{
}

static void
mtx_before_block()
{
  P0("<b>");
}

static void
mtx_after_block()
{
  P0("</b>\n");
}

static void
mtx_before_sources()
{
}

static void
mtx_between_sources()
{
}

static void
mtx_after_sources()
{
}

static void
mtx_before_composite()
{
  curr_complex = 0;
  P0("<l>");
}

static void
mtx_after_composite()
{
  if (curr_complex)
    P0("</w>");
  P0("</l>\n");
}

static void
mtx_each_composite_column()
{

  if (curr_complex != iterate_composite_column->complex_index)
    {
      if (curr_complex)
	P0("</w>");
      P1("<w form=\"%s\">",iterate_composite_column->complex_text);
      curr_complex = iterate_composite_column->complex_index;
    }

  P0("<c");

  if (!iterate_composite_column->index)
    {
      Pattr("n", iterate_line->name);
      Pempty();
    }
  else
    {
      /*CHECKME: should delim be used when type is zero or space?*/
      Pattr("delim",iterate_composite_column->delim);
      if (iterate_composite_column->space_flag)
	{
	  Pattr("type","space");
	  Pempty();
	}
      else if (iterate_composite_column->zero_flag)
	{
	  Pattr("type","zero");
	  Pempty();
	}
      else
	{
	  Pattr("type","text");
	  P0(">");
	  if (!iterate_composite_column->zero_flag)
	    {
	      gdl_print(ipf->output_fp,iterate_composite_column->text);
	      /*P1("%s", iterate_composite_column->text);*/
	    }
	  P0("</c>");
	}
      
    }
}

static void
mtx_between_composite_columns()
{
}

static void
mtx_before_reconstructed()
{
  P0("<r>");
}

static void
mtx_after_reconstructed()
{
  P0("</r>");
}

static void
mtx_each_reconstructed_column()
{
  P0("<c");

  if (!iterate_composite_column->index)
    {
      Pattr("n", iterate_line->block->reconstructed->name);
      Pempty();
    }
  else
    {
      /*CHECKME: should delim be used when type is zero or space?*/
      Pattr("delim",iterate_reconstructed_column->delim);
      if (iterate_reconstructed_column->space_flag)
	{
	  Pattr("type","space");
	  Pempty();
	}
      else if (iterate_reconstructed_column->zero_flag)
	{
	  Pattr("type","zero");
	  Pempty();
	}
      else
	{
	  Pattr("type","text");
	  P0(">");
	  if (!iterate_reconstructed_column->zero_flag)
	    P1("%s", iterate_reconstructed_column->text);
	  P0("</c>");
	}
      
    }
}

static void
mtx_between_reconstructed_columns()
{
}

static void
mtx_before_source()
{
  curr_s_complex = -1;
  P0("<s>");
}

static void
mtx_after_source()
{
  P0("</s>");
}

static Uchar *
mtx_source_col_content (Uchar *s)
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
mtx_cic_width (Uchar *dp)
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
mtx_meta (Uchar c)
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

static char
mtx_ic_alignment (Uchar c)
{
#if 1
  if (isalnum(c)) /* FIXME: UNICODE */
#else
  if (DAC_ISALNUM(c))
#endif
    return 'L';
  else
    return 'C';
}

static Uchar *
mtx_build_ic (Source_column*sp, Uchar *cs)
{
  static Uchar ic[128];
  
  if ('~' == *sp->text)
    ++sp->text;
  if (cs)
    sprintf (ic, "\\%s{%s}", cs, sp->text);
  else
    {
      sprintf (ic, "\\%cic{%s}{\\hss %s\\hss}",
	       mtx_ic_alignment(*sp->text),
	       mtx_cic_width(sp->left->composite->delim),
	       sp->text);
    }
  return ic;
}

static Uchar expand_pre[1024];
static void
mtx_expand_pre (int cols, int phantoms, Uchar *txt)
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
mtx_each_source_column()
{
  Boolean ends_with_phantom = FALSE;
  int complex_phantoms = -1, complex_filler = 0;
  static Boolean breakage_crosses_boundary = FALSE;
  int expand_cols_done = 0, expand_phantoms = 0, expand_zeroes = 0;

  if (iterate_source_column->done || iterate_source_column->filler_flag)
    return;

#if 0
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
	      warning (ewfile(iterate_line->file, iterate_line->linenum),
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
	      mtx_expand_pre(cols, phant, txt);
	      expand_cols_done += cols;
	      expand_phantoms += phant;
	      expand_zeroes += zeroes;
	    }
	  iterate_source_column->text = s;
	}

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
		  mtx_zeroes_skipped += sp->composite->complex_zeroes;
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
	      if (mtx_zeroes_skipped)
		--mtx_zeroes_skipped;
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
		  warning(ewfile(iterate_line->file,iterate_line->linenum),
			  "wrong column count in expand");
		}
	      if (complex_phantoms < 0)
		{
		  warning(ewfile(iterate_line->file,iterate_line->linenum),
			  "wrong phantom count in expand");
		  complex_phantoms = 0;
		}
	    }
	}
    }
#endif 

  /* special processing for the siglum column */
  if (0 == iterate_source_column->index)
    {
      if (!iterate_line->altsig || !*iterate_line->altsig)
	{
	  P0("<c");
	  Pattr("n",iterate_line->name);
	  Pattr("loc",iterate_line->tabloc->expanded);
	}
      else
	{
	  if (*iterate_line->tabloc->expanded)
	    {
	      P0("<c");
	      Pattr("n",iterate_line->name);
	      Pattr("alt",iterate_line->altsig);
	      Pattr("loc",iterate_line->tabloc->expanded);
	    }
	  else
	    {
	      P0("<c");
	      Pattr("n",iterate_line->name);
	      Pattr("alt",iterate_line->altsig);
	    }
	}
      P0("/>");
      ++mtx_output_col_count;
    }
  else
    {
      if (curr_s_complex >= 0)
	{
	  if (curr_s_complex != iterate_source_column->composite->complex_index)
	    {
	      P0("</w>");
	      curr_s_complex = -1;
	    }
	}
      if (iterate_source_column->complex_has_variant)
	{
	  P1("<w form=\"%s\">",iterate_source_column->complex_text);
	  curr_s_complex = iterate_source_column->composite->complex_index;
	}
      P1("<c cols=\"%d\">",iterate_source_column->columns_occupied);
      if (iterate_source_column->text_entry_flag)
	gdl_print(ipf->output_fp, iterate_source_column->text);
      else
	P0(iterate_source_column->text);
      P0("</c>");
      ++mtx_output_col_count;
    }
#if 0
  else if ((iterate_source_column->composite->zero_flag
	    || iterate_source_column->composite->space_flag)
	   && ('+' == *iterate_source_column->text
	       || '-' == *iterate_source_column->text
	       || (!do_square_brackets && '.' == *iterate_source_column->text)))
    {
      if (iterate_source_column->complex->composite->complex_has_variant
	  && iterate_source_column->complex->composite->complex_columns != 1)
	{
	  ++mtx_zeroes_skipped;
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
	    }
	}
      else
	{
	  P0("&");
	  ++mtx_output_col_count;
	  iterate_source_column->done = TRUE;
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
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
	      mtx_src_phantoms += complex_phantoms;
	      complex_phantoms = 0;
	      P3("&\\Ms{%d}\\phantoms{%d}%s", cols, phant, expand_pre + n);
	    }
	  else
	    {
	      P0(expand_pre);
	    }
	  P1("\\sp{}%s", iterate_source_column->text);
	  mtx_output_col_count += expand_cols_done;
	  mtx_src_phantoms += expand_phantoms;
	  if (complex_phantoms > 0)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
	      if (complex_phantoms > 1)
		warning(ewfile(iterate_line->file,iterate_line->linenum),
			"too many complex_phantoms (%d; expected at most 1)",
			complex_phantoms);
	    }
	}
      else
	{
	  Uchar *tmp = mtx_build_ic(iterate_source_column, NULL);
	  while (*tmp)
	    P1("%s", mtx_meta(*tmp++));
	  if (iterate_source_column->needs_phantom)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
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
	      append = mtx_build_ic (iterate_source_column->right, "Mappend");
	      iterate_source_column->right->done = TRUE;
	      if (iterate_source_column->right->needs_phantom)
		right_needs_phantom = TRUE;
	    }

	  if (*expand_pre)
	    P0(expand_pre);
	  if (complex_phantoms > 0)
	    {
	      P2("&\\Ms{%d}\\phantoms{%d}", 
		 iterate_source_column->columns_occupied + complex_phantoms,
		 complex_phantoms/*, append*/);
	      mtx_src_phantoms += complex_phantoms;
	    }
	  else if (iterate_source_column->needs_phantom || right_needs_phantom)
	    {
	      P1("&\\Ms{%d}\\phantom{}", iterate_source_column->columns_occupied + 1);
	      ++mtx_src_phantoms;
	    }
	  else
	    P1("&\\Ms{%d}", iterate_source_column->columns_occupied);

	  if (iterate_source_column->composite->zero_flag 
	      || iterate_source_column->apocopation_flag
	      || iterate_source_column->columns_occupied >= 1
	      )
	    P1("%s", mtx_hack_text(mtx_unhash(iterate_source_column->text)));
	  else
	    P1("\\Hw %s", mtx_hack_text(mtx_unhash(iterate_source_column->text)));

	  if (iterate_source_column->apocopation_flag)
	    P0("\\Hw");

	  if (*append)
	    P0(append);
	  mtx_output_col_count += iterate_source_column->columns_occupied;
	  mtx_output_col_count += expand_cols_done;
	  mtx_src_phantoms += expand_phantoms;
	  if (ends_with_phantom)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
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
		  ++mtx_zeroes_skipped;
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
	      mtx_src_phantoms += nphantoms;
	    }
	  else
	    P3("&\\Ms{%d}\\Breakage%s{%s}", ncols, 
	       breakage_string(),
	       append);
	  if (ends_with_phantom)
	    {
	      P0("&\\phantom{}");
	      ++mtx_src_phantoms;
	    }
	  mtx_output_col_count += ncols;
	}
      else
	{
	  Uchar *tmp = mtx_source_col_content(iterate_source_column->text);
	  Boolean right_needs_phantom = FALSE;

	  P1("&%s{", tmp);
	  for (tmp = &iterate_source_column->text[1]; *tmp; ++tmp)
	    P1("%s", mtx_meta(*tmp));

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
		    P1("%s", mtx_meta(*tmp++));
		}
	      else
		{
		  P0("}");
		  did_curly = TRUE;
		  tmp = mtx_build_ic(iterate_source_column->right, NULL);
		  while (*tmp)
		    P1("%s", mtx_meta(*tmp++));
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
	      ++mtx_src_phantoms;
	    }
	  mtx_output_col_count += iterate_source_column->columns_occupied;
	}
    }
#endif
  *expand_pre = '\0';
}

static void
mtx_between_source_columns()
{
}

static void
mtx_before_notes()
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
mtx_each_note()
{
  if (begins_with_siglum(iterate_text))
    {
      Tabloc *tp = do_tabloc (loc);
      P2("\\Sig{%s}{%s}", sig, mtx_hack_tabloc (tp->expanded));
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
mtx_after_notes()
{
}

static Uchar *
mtx_hack_tabloc (Uchar *s)
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

Process_functions pf_mtx = {
NULL, NULL,
mtx_before_outers,
mtx_each_outer,
mtx_between_outers,
mtx_after_outers,
mtx_each_text,
mtx_before_block,
mtx_after_block,
mtx_before_sources,
mtx_between_sources,
mtx_after_sources,
mtx_before_composite,
mtx_after_composite,
mtx_each_composite_column,
mtx_between_composite_columns,
mtx_before_reconstructed,
mtx_after_reconstructed,
mtx_each_reconstructed_column,
mtx_between_reconstructed_columns,
mtx_before_source,
mtx_after_source,
mtx_each_source_column,
mtx_between_source_columns,
mtx_before_notes,
mtx_each_note,
mtx_after_notes,
};

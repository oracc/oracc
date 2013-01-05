#include <psd_base.h>
#include "matrix.h"

#define xmalloc  malloc
#define xrealloc realloc
extern Boolean do_tex;
static Uchar *expand_buf = NULL;
static int expand_buf_len = 0;

Boolean
expand_needs_expansion (Source_column *sp)
{
  Boolean has_absence = FALSE, has_presence = FALSE;
  sp = sp->complex;
  while (1)
    {
      if (sp->text_entry_flag)
	return TRUE;
      else if ('-' == *sp->text)
	has_absence = TRUE;
      else if ('+' == *sp->text || '*' == *sp->text)
	has_presence = TRUE;
      if (!sp->right || sp->right->complex != sp->complex)
	break;
      else
	sp = sp->right;
    }
  if (has_absence && has_presence)
    return TRUE;
  else
    return FALSE;
}

static int init_absences, init_breaks, init_phantoms, init_zeroes, so_far = 0;
static Source_column *sp2;
static Composite_column *cp2;

static void
expand_init_absences ()
{
  int new_len = strlen("\\X00/00/00/ ");
  while (expand_buf_len <= so_far + new_len + 1)
    {
      expand_buf_len += 128;
      expand_buf = xrealloc (expand_buf, expand_buf_len);
    }
  sprintf (expand_buf + so_far, "\\X%d/%d/%d/ ", 
	   init_absences, init_phantoms, init_zeroes);
  so_far = strlen(expand_buf);
  init_absences = init_phantoms = init_zeroes = 0;
}

static void
expand_init_breaks ()
{
  int new_len = strlen("\\X00/00/00/[\\sp{}] ");
  while (expand_buf_len <= so_far + new_len + 1)
    {
      expand_buf_len += 128;
      expand_buf = xrealloc (expand_buf, expand_buf_len);
    }
  sprintf (expand_buf + so_far, 
	   "\\X%d/%d/%d/[%s] ", 
	   init_breaks, init_phantoms, init_zeroes,
	   sp2 ? sp2->left->composite->delim : (const Uchar *)"");
  so_far = strlen(expand_buf);
  init_breaks = init_phantoms = init_zeroes = 0;
}

Uchar *
expand_complex (Source_column *sp)
{
  Composite_column *target_complex;
  int new_len, cols_done;
  Uchar *s;
  const Uchar *unused_delim = "";
  static const char *kernTn = NULL;
  static int kernTn_len = 9;
  int state = 0;
  Boolean hyphen_pending = FALSE;

  if (!kernTn)
    kernTn = (do_tex ? "\\kernTn{}" : "");

  so_far = 0;
  if (expand_buf_len)
    *expand_buf = '\0';

  if (!sp || !sp->complex)
    return NULL;

  if (sp != sp->complex)
    {
      if (sp->complex->done)
	return NULL;
      sp = sp->complex;
    }

  if (sp->done)
    return NULL;
  sp->ncomplexes = 1;
  
  sp2 = sp;
  cols_done = 0;
  state = 0;
  init_absences = init_breaks = init_phantoms = init_zeroes = 0;
  for (target_complex = sp->composite->complex; 
       sp2 && sp2->composite->complex == target_complex; 
       sp2 = sp2->right)
    {
      Uchar *txt = "", *txt2 = "", *meta = "";
      const Uchar *delim = "";

      ++cols_done;

      if (init_absences && !state && *sp2->text != '-')
	expand_init_absences();
      else if (init_breaks && !state && *sp2->text != '.')
	expand_init_breaks();

      if (*sp2->text != '-' && *sp2->text != '.' && !state)
	state = 1;

      if (sp2->text_entry_flag)
	{
	  txt = sp2->text;
	  if (*txt == '~')
	    ++txt;
	  delim = *sp2->delim ? sp2->delim : sp2->composite->delim;
	}
      else if (*sp2->text == '+')
	{
	  txt = sp2->composite->text;
	  meta = &sp2->text[1];
	  delim = *sp2->delim ? sp2->delim : sp2->composite->delim;
	}
      else if (*sp2->text == '*')
	{
	  txt = sp2->composite->text;
	  txt2 = "'";
	  meta = &sp2->text[1];
	  delim = *sp2->delim ? sp2->delim : sp2->composite->delim;
	}
      else if (*sp2->text == '-')
	{
	  if (!state)
	    {
	      if (sp2->composite->zero_flag)
		++init_zeroes;
	      else
		++init_absences;
	      if (sp2->needs_phantom)
		++init_phantoms;
	      continue;
	    }
	}
      else if (*sp2->text == ',')
	{
	  txt = "x'";
	  delim = "";
	}
      else if (*sp2->text == '.')
	{
	  if (!state)
	    {
	      if (sp2->composite->zero_flag)
		++init_zeroes;
	      else
		++init_breaks;
	      if (sp2->needs_phantom)
		++init_phantoms;
	      continue;
	    }
	  else
	    {
	      txt = "[\\sp{}]";
	      delim = *sp2->delim ? sp2->delim : sp2->composite->delim;
	    }
	}
      else if (*sp2->text == '/')
	{
	  txt = "/";
	}
      else if (*sp2->text == ';')
	{
	  txt = ";";
	}
      else if (*sp2->text == '%')
	{
	  txt = ",";
	}
      if (*txt == '\0' || *delim == '\n')
	delim = "";

      if (!*delim 
	  && sp2->right && sp2->right->composite->complex == target_complex
	  && *sp2->right->delim)
	delim = sp2->right->delim;
	  
      if (*delim == ' ')
	delim = "\\sp{}";
      new_len = strlen(txt) + strlen(txt2) + strlen(meta) + strlen(delim);
      while (expand_buf_len <= so_far + new_len + 1 + kernTn_len)
	{
	  expand_buf_len += 128;
	  expand_buf = xrealloc (expand_buf, expand_buf_len);
	}
      if ('[' == *txt)
	{
	  Uchar *t;
	  for (t = &expand_buf[so_far];
	       t > expand_buf && (t[-1] == ' ' || t[-1] == '-' || t[-1] == '.');
	       --t)
	    ;
	  if (']' == t[-1])
	    {
	      t[-1] = '\0';
	      so_far = strlen(expand_buf);
	      ++txt;
	      --new_len;
	    }
	}
      if (sp2->composite_end->complex != target_complex)
	{
	  Source_column *sp3, *ocomplex = sp->complex;
	  for (sp3 = sp2; sp3; sp3 = sp3->right)
	    {
	      if (sp3->composite->complex != ocomplex->composite->complex)
		{
		  ocomplex = sp3;
		  sp3->complex = sp->complex;
		}
	      if (sp3->cindex == sp2->composite_end->index)
		break;
	    }
	  while (sp3 && sp3->composite->complex == ocomplex->composite->complex)
	    {
	      sp3->complex = sp->complex;
	      sp3 = sp3->right;
	    }
	  ++sp->ncomplexes;
	  target_complex = sp2->composite_end->complex;
	}
      else if (!sp2->right 
	       || sp2->right->composite->complex != sp2->composite->complex)
	{
	  unused_delim = delim;
	  delim = "";
	}
      sprintf (expand_buf + so_far, "%s%s%s%s", txt, txt2, meta, delim);
      so_far += new_len;
    }

  /* we found only initial absences or breakages */
  if (init_absences && !state)
    expand_init_absences();
  else if (init_breaks && !state)
    expand_init_breaks();
  s = expand_buf + strlen(expand_buf);

  while (s > expand_buf && (s[-1] == ' ' || s[-1] == '-'))
    --s;
  strcpy (s, kernTn);
  return expand_buf;
}

/*FIXME: this can probably be simplified--at the moment it's just a
  hack of the routine above*/
Uchar *
expand_composite_complex (Composite_column *sp)
{
  Composite_column *target_complex;
  int new_len, cols_done;
  Uchar *s;
  const Uchar *unused_delim = "";
  static const char *kernTn = NULL;
  static int kernTn_len = 9;
  int state = 0;
  Boolean hyphen_pending = FALSE;

  if (!kernTn)
    kernTn = (do_tex ? "\\kernTn{}" : "");

  so_far = 0;
  if (expand_buf_len)
    *expand_buf = '\0';

  if (!sp || !sp->complex)
    return NULL;

  if (sp != sp->complex)
    {
      if (sp->complex->done)
	return NULL;
      sp = sp->complex;
    }

  if (sp->done)
    return NULL;
  
  cp2 = sp;
  cols_done = 0;
  state = 0;
  init_absences = init_breaks = init_phantoms = init_zeroes = 0;
  for (target_complex = sp->complex; 
       cp2 && cp2->complex == target_complex; 
       cp2 = cp2->right)
    {
      Uchar *txt = "", *txt2 = "", *meta = "";
      const Uchar *delim = cp2->delim;

      ++cols_done;
      txt = cp2->text;
      if (*txt == '\0' || *delim == '\n')
	delim = "";

      new_len = strlen(txt) + strlen(txt2) + strlen(meta) + strlen(delim);
      while (expand_buf_len <= so_far + new_len + 1 + kernTn_len)
	{
	  expand_buf_len += 128;
	  expand_buf = xrealloc (expand_buf, expand_buf_len);
	}
      if ('[' == *txt)
	{
	  Uchar *t;
	  for (t = &expand_buf[so_far];
	       t > expand_buf && (t[-1] == ' ' || t[-1] == '-' || t[-1] == '.');
	       --t)
	    ;
	  if (']' == t[-1])
	    {
	      t[-1] = '\0';
	      so_far = strlen(expand_buf);
	      ++txt;
	      --new_len;
	    }
	}
#if 0
      if (cp2->composite_end->complex != target_complex)
	{
	  Composite_column *cp3, *ocomplex = sp->complex;
	  for (cp3 = cp2; cp3; cp3 = cp3->right)
	    {
	      if (cp3->complex != ocomplex->complex)
		{
		  ocomplex = cp3;
		  cp3->complex = sp->complex;
		}
	      if (cp3->cindex == cp2->composite_end->index)
		break;
	    }
	  while (cp3 && cp3->complex == ocomplex->complex)
	    {
	      cp3->complex = sp->complex;
	      cp3 = cp3->right;
	    }
	  ++sp->ncomplexes;
	  target_complex = cp2->composite_end->complex;
	}
      else if (!cp2->right 
	       || cp2->right->complex != cp2->complex)
	{
	  unused_delim = delim;
	  delim = "";
	}
#endif
      sprintf (expand_buf + so_far, "%s%s%s%s", txt, txt2, meta, delim);
      so_far += new_len;
    }

  s = expand_buf + strlen(expand_buf);
  while (s > expand_buf && (s[-1] == ' ' || s[-1] == '-'))
    --s;
  *s = '\0';
  return expand_buf;
}

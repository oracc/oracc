#include <psd_base.h>
#include "matrix.h"
#include "sources.h"

#define xmalloc  malloc
#define xrealloc realloc
#define xcalloc  calloc

static Uchar *expandbuf = NULL;

Srcs_tabloc empty_srcs_tabloc = {NULL,NULL,NULL,SFE_NONE,0,0,0,NULL,FALSE,FALSE};
Tabloc empty_tabloc = { "" , "", &empty_srcs_tabloc };

#define TABLOC_TLOC_LEN 10
static char tloc[TABLOC_TLOC_LEN + 1];

#define TABLOC_META_LEN 10
static Uchar meta[TABLOC_META_LEN + 1];

static int expandbuf_alloced = 0;
static Boolean tabloc_warned;
static const char *get_next_bit (Uchar *t, Uchar **pt, Srcs_tabloc *stp);
static Srcs_tabloc *new_srcs_tabloc (void);
static int Arabicised (const char *Roman);

static void
grow_expandbuf ()
{
  expandbuf_alloced += 256;
  expandbuf = xrealloc (expandbuf, expandbuf_alloced);
}

Tabloc *
do_tabloc (Uchar *t)
{
  const char *next_bit;
  Boolean first_bit = TRUE;

  Tabloc *tmp = xmalloc (sizeof (Tabloc));
  tmp->compressed = xstrdup (t);
  tmp->encoded = new_srcs_tabloc ();
  tmp->encoded->is_range_begin = TRUE;
  if (NULL == expandbuf)
    grow_expandbuf ();
  *expandbuf = '\0';
  tabloc_warned = FALSE;
  while (*t)
    {
      next_bit = get_next_bit (t, &t, tmp->encoded);
      if (first_bit)
	{
	  while (' ' == *next_bit)
	    ++next_bit;
	  first_bit = FALSE;
	}
      while (strlen(expandbuf) + strlen(next_bit) + strlen(meta) + 1 
	     >= expandbuf_alloced)
	grow_expandbuf ();
      strcat (expandbuf, next_bit);
      if (*meta)
	strcat (expandbuf, meta);
    }
  tmp->expanded = xstrdup (expandbuf);
  return tmp;
}

static char *tabloc_static_cols[] = 
{ 
  "", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix", "x" 
};
#define TABLOC_MAX_STATIC_COL 10

/*FIXME: meta information on location should get passed to stp also;
  catch is that it isn't parsed until the end, but it has to be passed
  through to the right text component of stp */
static const char *
get_next_bit (Uchar *t, Uchar **pt, Srcs_tabloc *stp)
{
  Uchar *mp = meta;
  const char *ret = "";

  *meta = '\0';

  switch (*t)
    {
    case '-':
      ret = "-";
      if (!stp->is_range_end)
	{
	  stp->range_end = new_srcs_tabloc ();
	  stp = stp->range_end;
	  stp->is_range_end = TRUE;
	}
      ++t;
      break;
    case '/': /* this is used for Akkadian bilingual locators, e.g. Ur2,o1/a */
      {
	int i = 1;
	tloc[0] = '/';
	while (*t && !isspace(*t))
	  {
	    tloc[i++] = *t++;
	    if (i == TABLOC_TLOC_LEN)
	      error (matrix_location(),
		     "excessive locator information (max %d) in tablet locator",
		     TABLOC_TLOC_LEN);
	  }       
	tloc[i] = '\0';
	ret = tloc;
      }
      break;
    case 'i':
    case 'v':
    case 'x':
      {
	int i = 1;
	tloc[0] = ' ';
	while ('i' == *t || 'v' == *t || 'x' == *t)
	  {
	    tloc[i++] = *t++;
	    if (i == TABLOC_TLOC_LEN)
	      error (matrix_location(),
		     "excessive locator information (max %d) in tablet locator",
		     TABLOC_TLOC_LEN);
	  }       
	tloc[i] = '\0';
	stp->column = Arabicised (tloc);
	if (stp->column <= TABLOC_MAX_STATIC_COL)
	  stp->col_text = tabloc_static_cols[stp->column];
	else
	  stp->col_text = xstrdup(tloc);
	ret = tloc;
      }
      break;

    case 'R':
      stp->face_text = ret = " r.e.";
      stp->face = SFE_RT;
      ++t;
      break;
    case 'L':
      stp->face_text = ret = " l.e.";
      stp->face = SFE_LFT;
      ++t;
      break;
    case 'T':
      stp->face_text = ret = " t.e.";
      stp->face = SFE_TOP;
      ++t;
      break;
    case 'B':
      stp->face_text = ret = " b.e.";
      stp->face = SFE_BOT;
      ++t;
      break;

    default:
      if (isalpha(*t))
	{
	  switch (*t)
	    {
	    case 'o':
	      stp->face = SFE_OBV;
	      stp->face_text = "obv.";
	      break;
	    case 'r':
	      stp->face = SFE_REV;
	      stp->face_text = "rev.";
	      break;
	    default:
	      stp->face = SFE_FACE;
	      stp->face_text = NULL;
	      break;
	    }
	  tloc[0] = ' ';
	  tloc[1] = *t++;
	  tloc[2] = '\0';
	  ret = tloc;
	  if (NULL == stp->face_text)
	    stp->face_text = xstrdup (tloc);
	}
      else if (isdigit(*t))
	{
	  int i = 1;
	  tloc[0] = ' ';
	  while (isdigit(*t))
	    {
	      tloc[i++] = *t++;
	      if (i == TABLOC_TLOC_LEN)
		error (matrix_location(),
		       "excessive locator information (max %d) in tablet locator",
		       TABLOC_TLOC_LEN);
	    }       
	  tloc[i] = '\0';
	  ret = tloc;
	}
      else if ('_' == *t || '/' == *t)
	{
	  ++t;
	}
      else
	{
	  if (!tabloc_warned)
	    {
	      warning (matrix_location(),
		       "illegal character `%c' in tablet locator",
		       *t);
	      ++tabloc_warned;
	    }
	  ++t;
	}
      break;
    }
  while ('?' == *t || '!' == *t || '\'' == *t || '"' == *t)
    {
      *mp++ = *t++;
      if (mp - meta == TABLOC_META_LEN)
	error (matrix_location(),
	       "excessive meta-information (max %d) in tablet locator",
	       TABLOC_META_LEN);
    }
  *mp = '\0';
  *pt = &t[0];
  return ret;
}

Boolean
tabloc_transition (Tabloc *a, Tabloc *b)
{
  return a->encoded->face != b->encoded->face 
    || a->encoded->column != b->encoded->column;
}

Uchar *
tabloc_location_noline (Tabloc *tp)
{
  const Uchar *s;
  if (tp->encoded->face != SFE_NONE)
    {
      s = tp->encoded->face_text;
      while (isspace(*s))
	++s;
      strcpy (tloc, s);
    }
  else
    *tloc = '\0';
  if (tp->encoded->column)
    {
      s = tp->encoded->col_text;
      while (isspace(*s))
	++s;
      if (tp->encoded->face != SFE_NONE)
	strcat (tloc, " ");
      strcat (tloc, s);
    }
  return xstrdup (tloc);
}

static Srcs_tabloc *
new_srcs_tabloc ()
{
  Srcs_tabloc *tmp = xcalloc (1, sizeof (Srcs_tabloc));
  return tmp;
}

/* from: http://ftp.cwi.nl/ftp/lambert/fj/views/appRoman.c */
static int
Arabicised (const char *Roman)
{
  const char *rom;
  int v= 0, i, n, roml;
  int bign= 0;
  
  rom= Roman;
  roml= strlen(rom);
  
  if (roml == 0) return 0;
  
  for (i= roml-1; i >= 0; i--) {
    switch (toupper(rom[i])) {
    case 'I': n=    1; break;
    case 'V': n=    5; break;
    case 'X': n=   10; break;
    case 'L': n=   50; break;
    case 'C': n=  100; break;
    case 'D': n=  500; break;
    case 'M': n= 1000; break;
    default:  n=    0; break;
    }
    if (n < bign) v-= n;
    else  {
      v+= n;
      if (n > bign) bign= n;
    }
  }
  return v;
}

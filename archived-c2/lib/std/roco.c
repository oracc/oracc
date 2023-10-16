#include <stdlib.h>
#include <stdio.h>
#include <loadfile.h>
#include <list.h>
#include <roco.h>
#include <xmlify.h>

/* FIXME: MEMORY MANAGEMENT -- keep roco list and delete or use static
   Memo for Rocos */

#define uccp unsigned const char *

const char *roco_colorder = NULL;
const char *roco_format = NULL;
int roco_newline = 0;
int roco_xmlify = 1;

Roco *
roco_create(int rows, int cols)
{
  Roco *r = calloc(1, sizeof(Roco));
  r->nlines = rows;
  r->rows = calloc(rows, sizeof(const char **));
  int i;
  for (i = 0; i < r->nlines; ++i)
    r->rows[i] = calloc(cols, sizeof(const char *));
  return r;
}

Roco *
roco_load(const char *file, int fieldsr1,
	  const char *xtag, const char *rtag, const char *ctag)
{
  Roco *r = calloc(1, sizeof(Roco));
  r->file = file;
  r->lines = loadfile_lines3((uccp)r->file, (size_t*)&r->nlines, &r->mem);
  r->rows = calloc(r->nlines, sizeof(unsigned char **));
  r->fields_from_row1 = fieldsr1;

  r->xmltag = (xtag ? xtag : "x");
  r->rowtag = (rtag ? rtag : "r");
  r->celtag = (ctag ? ctag : "c");

  int i;
  for (i = 0; i < r->nlines; ++i)
    {
      int ncol = 1;
      unsigned char *s = r->lines[i];
      while (*s)
	 {
	  if ('\t' == *s)
	    ++ncol;
	  ++s;
	}
      r->rows[i] = calloc(ncol+1, sizeof(unsigned char *));
      int col;
      for (col = 0, s = r->lines[i]; *s; ++col)
	{
	  if ('\t' == *s)
	    r->rows[i][col] = (unsigned char *)"";
	  else
	    r->rows[i][col] = s;
	  while (*s && '\t' != *s)
	    ++s;
	  if ('\t' == *s)
	    *s++ = '\0';
	}

      while (col < ncol)
	r->rows[i][col++] = (unsigned char *)"";
	
      r->rows[i][col] = NULL;
    }
  return r;
}

void
roco_hash_hash(Hash *h, Roco *r)
{
  size_t i;
  for (i = 0; i < r->nlines; ++i)
    {
      if ('#' != *r->rows[i][0])
	{
	  if (!strcmp((ccp)r->rows[i][0], ".include"))
	    roco_hash_hash(h, roco_load1((ccp)r->rows[i][1]));      
	  else
	    hash_add(h, r->rows[i][0], r->rows[i][1]);
	}
    }
}

Hash *
roco_hash(Roco *r)
{
  Hash *h = hash_create(r->nlines/2);
  roco_hash_hash(h, r);
  return h;
}

static const char *
roco_co_fo()
{
  char buf[strlen(roco_colorder) * 4], *b = buf;
  const char *s;
  for (s = roco_colorder; *s; ++s)
    {
      *b++ = '%';
      *b++ = *s;
      if (s[1])
	*b++ = '\t';
    }
  *b++ = '\n';
  *b = '\0';
  return strdup(buf);
}

void
roco_write(FILE *fp, Roco *r)
{
  size_t i;

  if (roco_colorder)
    roco_format = roco_co_fo();
  
  for (i = 0; i < r->nlines; ++i)
    {
      if (roco_format)
	roco_row_format(fp, (const unsigned char **)r->rows[i]);
      else
	{
	  int j;
	  for (j = 0; r->rows[i][j] != NULL; ++j)
	    {
	      if (j)
		fputc('\t', fp);
	      if (*r->rows[i][j])
		{
		  if (r->linkcells)
		    {
		      Link *lp;
		      for (lp = (Link*)r->rows[i][j]; lp; lp = lp->next)
			{
			  fputs((const char *)lp->data, fp);
			  if (lp->next)
			    fputc('#', fp);
			}
		    }
		  else
		    fputs((const char *)r->rows[i][j], fp);
		}
	    }
	  fputc('\n', fp);
	}      
    }

  if (roco_colorder)
    free((void*)roco_format);
}


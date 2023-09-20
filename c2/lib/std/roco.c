#include <stdlib.h>
#include <stdio.h>
#include <loadfile.h>
#include <roco.h>
#include <xmlify.h>

#define uccp unsigned const char *

const char *roco_format = NULL;
int roco_newline = 0;

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
roco_write(FILE *fp, Roco *r)
{
  size_t i;
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
		fputs((const char *)r->rows[i][j], fp);
	    }
	  fputc('\n', fp);
	}
    }
}


#ifndef ROCO_H_
#define ROCO_H_

/* Row-and-column type
 *
 * These routines support variable numbers of columns per row.
 *
 */
struct roco
{
  unsigned char *mem;    /* memory allocated for input */
  const char *file;      /* file name; - for stdin */
  unsigned char **lines; /* lines in a .tab/.tsv file, created by loadfile_lines3 */
  size_t nlines;	 /* number of lines */
  unsigned char ***rows; /* rows, an array of ptr to unsigned char ** */
  unsigned char **cols;  /* columns, i.e., array of unsigned char * to items after <TAB>;
			    empty string if <TAB><TAB> */
  int fields_from_row1;  /* take field names from row1 for xml output */
  const char *xmltag;    /* tag to wrap XML output */
  const char *rowtag;    /* tag to wrap row output */
  const char *celtag;    /* tag to wrap cell output */
};

typedef struct roco Roco;

extern const char *roco_format;
extern int roco_newline;

/* Keep this on one line so GCC errors show the whole prototype
 */
extern Roco *roco_load(const char *file, int fieldsr1, const char *xtag, const char *rtag, const char *ctag);
extern void roco_row_format(FILE*fp, const unsigned char **row);
extern void roco_write(FILE *fp, Roco *r);
extern void roco_write_trtd(FILE *fp, Roco *r);
extern void roco_write_xml(FILE *fp, Roco *r);

#endif/*ROCO_H_*/

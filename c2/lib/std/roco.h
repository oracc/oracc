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
  const char *file;   /* file name; - for stdin */
  unsigned char **lines; /* lines in a .tab/.tsv file, created by loadfile_lines3 */
  size_t nlines;	 /* number of lines */
  unsigned char ***rows; /* rows, an array of ptr to unsigned char ** */
  unsigned char **cols;  /* columns, i.e., array of unsigned char * to items after <TAB>;
			    empty string if <TAB><TAB> */
};

typedef struct roco Roco;

extern Roco *roco_load(const char *file);
extern void roco_write(FILE *fp, Roco *r);

#endif/*ROCO_H_*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "loadfile.h"

extern const char *prog;

unsigned char **
loadfile_lines3(unsigned const char *fname, size_t *nlines, unsigned char **fmem)
{
  size_t i, j, n, l;
  unsigned char *f = NULL;
  unsigned char **lp;

  if ('-' == *fname && !fname[1])
    f = loadstdin(&n);
  else
    f = loadfile(fname,&n);

  *fmem = f;

  for (i = l = 0; i < n; ++i)
    if ('\n' == f[i])
      ++l;
  lp = malloc((l+1) * sizeof(unsigned char *));
  for (i = j = 0; i < n; ++i)
    {
      lp[j++] = &f[i];
      while (i < n && f[i] != '\n')
	++i;
      f[i] = '\0';
    }
  lp[j] = NULL;
  if (nlines)
    *nlines = l;
  return lp;
}

unsigned char **
loadfile_lines(unsigned const char *fname, size_t *nlines)
{
  size_t i, j, n, l;
  unsigned char *f = loadfile(fname,&n);
  unsigned char **lp;
  
  for (i = l = 0; i < n; ++i)
    if ('\n' == f[i])
      ++l;
  lp = malloc((l+1) * sizeof(unsigned char *));
  for (i = j = 0; i < n; ++i)
    {
      lp[j++] = &f[i];
      while (i < n && f[i] != '\n')
	++i;
      f[i] = '\0';
    }
  lp[j] = NULL;
  if (nlines)
    *nlines = l;
  return lp;
}

unsigned char *
loadfile(unsigned const char *fname, size_t *nbytes)
{
  struct stat finfo;
  unsigned char *ftext;
  size_t fsize;
  int fdesc;

  if (fname == NULL)
    {
      fprintf(stderr,"%s: must give filename argument\n",prog);
      exit(2);
    }
  if (-1 == stat((const char*)fname,&finfo))
    {
      fprintf(stderr,"%s: stat failed on %s\n",prog,fname);
      exit(2);
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"%s: %s not a regular file\n",prog,fname);
      exit(2);
    }
  fsize = finfo.st_size;
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"%s: %s: couldn't malloc %ld bytes\n",
	      prog,fname,(unsigned long)fsize);
      exit(2);
    }
  fdesc = open((const char*)fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret != fsize)
	{
	  fprintf(stderr,"%s: %s: read %ld bytes failed\n", prog, fname, (unsigned long)fsize);
	  exit(2);
	}
      if (nbytes)
	*nbytes = ret;
      ftext[ret] = '\0';
    }
  else
    {
      fprintf(stderr, "%s: %s: open failed (system error %d: %s)\n", prog, fname, errno, strerror(errno));
      exit(2);
    }

  return ftext;
}

unsigned char *
loadstdin(size_t *nbytes)
{
  static unsigned char*buf;
  static int n_read = 0, n_alloced = 0;
  int ch;
  while (EOF != (ch = getchar()))
    {
      if (n_read == n_alloced)
	{
	  n_alloced += 8192;
	  buf = realloc(buf,n_alloced);
	}
      buf[n_read++] = ch;
    }
  *nbytes = n_read;
  return buf;
}


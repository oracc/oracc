#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <psd_base.h>

extern const char *errmsg_fn;
extern const char *file;

unsigned char *
slurp(const char *caller, const char *fname, ssize_t *fsizep)
{
  struct stat finfo;
  int fdesc;
  ssize_t fsize;
  unsigned char *ftext = NULL;
  if (-1 == stat(fname,&finfo))
    {
      fprintf(stderr,"%s: stat failed on %s\n",caller,fname);
      return NULL;
    }
  if (!S_ISREG(finfo.st_mode))
    {
      fprintf(stderr,"%s: %s not a regular file\n",caller,fname);
      return NULL;
    }
  fsize = finfo.st_size;
  if (!fsize)
    {
      fprintf(stderr,"%s: %s: empty file\n",caller,fname);
      return NULL;
    }
  if (NULL == (ftext = malloc(fsize+1)))
    {
      fprintf(stderr,"%s: %s: couldn't malloc %d bytes\n",
              caller,fname,(int)fsize);
      return NULL;
    }

  fdesc = open(fname,O_RDONLY);
  if (fdesc >= 0)
    {
      ssize_t ret = read(fdesc,ftext,fsize);
      close(fdesc);
      if (ret == fsize)
        ftext[fsize] = '\0';
      else
        {
          fprintf(stderr,"%s: read %d bytes failed\n",caller,(int)fsize);
          free(ftext);
          return NULL;
        }
    }
  else
    {
      fprintf(stderr, "%s: %s: open failed\n",caller,fname);
      free(ftext);
      return NULL;
    }

  file = errmsg_fn ? errmsg_fn : fname;
  if (fsizep)
    *fsizep = fsize;
  return ftext;
}

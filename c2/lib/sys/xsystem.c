/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: xsystem.c,v 0.6 1997/09/08 14:50:07 sjt Exp s $
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <xsystem.h>

int
xaccess (const char *path, int amode, Boolean quit)
{
  int ret;
  if (NULL == path)
    {
      if (quit)
	{
	  fprintf (stderr, "unable to access NULL path");
	  exit(1);
	}
      else
        return -1;
    }
  ret = access (path, amode);
  if (ret && quit)
    xperror ("access to '%s' denied", path);

  return ret;
}

void
xfclose (const char *fn, FILE *fp)
{
  if (fclose (fp))
    xperror ("close failed on '%s'", fn);
}

FILE *
xfopen (const char *fn, const char *mode)
{
  FILE *tmp = fopen (fn, mode);
  if (NULL == tmp)
    xperror ("open failed on '%s'", fn);
  return tmp;
}

size_t
xfread (const char *fn, Boolean binflag, const void *buf, size_t size, size_t count, FILE*fp)
{
  size_t ret;
  if (NULL == buf || NULL == fp)
    abort();
  ret = fread ((char*)buf, size, count, fp);
  if ((size_t)EOF == ret || (ret != count && binflag))
    {
      fprintf(stderr, "read failed on '%s'", fn);
      exit(1);
    }
  return ret;
}

void
xfstat (const char *fn, int handle, struct stat *res)
{
  if (fstat (handle, res) < 0)
    xperror ("fstat failed on '%s'", fn);
}

size_t
xfwrite (const char *fn, Boolean binflag, const void *buf, size_t size, size_t count, FILE*fp)
{
  size_t ret;
  if (NULL != fp && NULL != buf)
    ret = fwrite (buf, size, count, fp);
  else
    ret = (size_t)EOF;
  if ((size_t)EOF == ret || (ret != count && binflag))
    {
      fprintf(stderr, "write failed on '%s'", fn);
    }
  return ret;
}

int
xmkdir (const char *path, mode_t mode, Boolean quit)
{
  int ret;
  if ((ret = mkdir (path, mode)) == -1 && quit)
    xperror ("mkdir `%s' failed", path);
  return ret;
}

void
xperror (const char *fmt,...)
{
  char buf[1024];
  if (NULL != fmt)
    {
      va_list va;
      va_start (va, fmt);
      (void) vsprintf (buf, fmt, va);
      va_end (va);
    }
  else
    *buf = '\0';
  (void) sprintf (buf+strlen(buf), "%s%s", 
		  *buf ? ": " : "system error: ", strerror(errno));
  fprintf(stderr, "%s", buf);
}

void
xremove (const char *fn)
{
  if (NULL == fn)
    return;
  if (remove (fn))
    xperror ("remove failed on '%s'", fn);
}

void
xstat (const char *fn, struct stat *res)
{
  if (stat (fn, res) < 0)
    xperror ("stat failed on '%s'", fn);
}

char *
xstrdup (const char * c)
{
  char *tmp = malloc (NULL == c ? 1 : strlen (c) + 1);
  if (NULL == c)
    *tmp = '\0';
  else
    (void)strcpy (tmp, c);
  return tmp;
}

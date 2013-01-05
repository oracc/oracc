/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: fileutil.c,v 0.7 1998/12/19 05:51:26 s Exp $
*/

#include <psd_base.h>

/* return TRUE if fn1 is older than fn2 */
Boolean
fcheckf (const char *fn1, const char *fn2)
{
  struct stat buf1, buf2;
  if (stat (fn1, &buf1) < 0) /* if nonexistent consider fn1 infinitely old */
    return TRUE;
  if (stat (fn2, &buf2) < 0 || buf1.st_mtime < buf2.st_mtime)
    return TRUE;
  else
    return FALSE;
}

Boolean
fcheckf_list (char *fn1, char **fn2s)
{
  struct stat buf1, buf2;
  int i;

  if (stat (fn1, &buf1) < 0) /* if nonexistent consider fn1 infinitely old */
    return TRUE;
  for (i = 0; fn2s[i]; ++i)
    if (stat (fn2s[i], &buf2) < 0 || buf1.st_mtime < buf2.st_mtime)
      return TRUE;
  return FALSE;
}

off_t
fsize (const char *fn, FILE *fp)
{
  struct stat res;
#if 0
  if (NULL != fp)
    xfstat (fn, fileno (fp), &res);
  else
#endif
    xstat (fn, &res);
  return res.st_size;
}

#if defined (MSDOS)
size_t
fsize_t (const char *fn, FILE *fp)
{
  Unsigned32 res = fsize (fn, fp);
  if (res > (unsigned)65511)
    error (NULL, "%s exceeds DOS max file size (65511)", fn);
  return (size_t) res;
}
#endif

/* although currently this routine simply exits on failure, it 
   should really be reworked to trap errors and return a value */
void
filecopy(const char *from, const char *to)
{
  unsigned char *dtabuf;
  FILE*fromfile, *tofile;
  size_t bufsize, bytes_read, bytes_written;

#if (UNIX | OS2 | __EMX__)
  bufsize = 32768;
  dtabuf = malloc (bufsize);
#else
  for (bufsize = 32767; NULL == (dtabuf = malloc(bufsize)); bufsize -= 2048)
    if (bufsize < 2564)
      error (NULL, "out of memory trying to copy file");
#endif
  fromfile = xfopen (from, "rb");
  tofile = xfopen (to, "wb");
  do
    {
      /* use FALSE as binflag argument here to suppress requirement that
	 as bytes_read == bufsize */
      bytes_read = xfread (from, FALSE, dtabuf, 1, bufsize, fromfile);
      bytes_written = xfwrite (to, TRUE, dtabuf, 1, bytes_read, tofile);
    }
  while (!feof(fromfile));
  xfclose (from, fromfile);
  xfclose (from, tofile);
  free (dtabuf);
}

/* Return the basename of a pathname.
   This file is in the public domain. */

/*
NAME
	basename -- return pointer to last component of a pathname

SYNOPSIS
	char *basename (const char *name)

DESCRIPTION
	Given a pointer to a string containing a typical pathname
	(/usr/src/cmd/ls/ls.c for example), returns a pointer to the
	last component of the pathname ("ls.c" in this case).

BUGS
	Presumes a UNIX or DOS/Windows style path with UNIX or DOS/Windows 
	style separators.
*/

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR '/'
#endif

#if defined (_WIN32) || defined (__MSDOS__) || defined (__DJGPP__) || \
  defined (__OS2__)
#define HAVE_DOS_BASED_FILE_SYSTEM
#ifndef DIR_SEPARATOR_2 
#define DIR_SEPARATOR_2 '\\'
#endif
#endif

/* Define IS_DIR_SEPARATOR.  */
#ifndef DIR_SEPARATOR_2
# define IS_DIR_SEPARATOR(ch) ((ch) == DIR_SEPARATOR)
#else /* DIR_SEPARATOR_2 */
# define IS_DIR_SEPARATOR(ch) \
	(((ch) == DIR_SEPARATOR) || ((ch) == DIR_SEPARATOR_2))
#endif /* DIR_SEPARATOR_2 */

char *
basename (name)
     const char *name;
{
  const char *base;

#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
  /* Skip over the disk name in MSDOS pathnames. */
  if (ISALPHA (name[0]) && name[1] == ':') 
    name += 2;
#endif

  for (base = name; *name; name++)
    {
      if (IS_DIR_SEPARATOR (*name))
	{
	  base = name + 1;
	}
    }
  return (char *) base;
}

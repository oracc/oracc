/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: xsystem.c,v 0.6 1997/09/08 14:50:07 sjt Exp s $
*/

#include <psd_base.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <messages.h>

int
xaccess (const char *path, int amode, Boolean quit)
{
  int ret;
  if (NULL == path)
    {
      if (quit)
        error (NULL, "unable to access NULL path");
      else
        return -1;
    }
  ret = access (path, amode);
  if (ret && quit)
    xperror ("access to '%s' denied", path);

  return ret;
}

#if defined(__MPW__) && defined(MAC)
/* CodeWarrior doesn't implement access() on the Mac.  
   Too bad; I can do it if they can't. */
int
mac_access (path, amode)
{
  struct stat res;
  int test_passed;
  if (stat (fn, res) < 0)
    xperror ("stat failed on '%s'", fn);
  switch (amode)
    {
    case F_OK:
      test_passed = res.st_mode&S_IFREG;
      break;
    case R_OK:
      test_passed = res.st_mode&S_IREAD;
      break;
    case W_OK:
      test_passed = res.st_mode&S_IWRITE;
      break;
    case X_OK:
      test_passed = res.st_mode&S_IEXEC;
      break;
    default:
      fatal();
      break;
    }
  return !test_passed;
}
#endif

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
    fatal ();
  ret = fread ((char*)buf, size, count, fp);
  if ((size_t)EOF == ret || (ret != count && binflag))
    error (NULL, "read failed on '%s'", fn);
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
    error (NULL, "write failed on '%s'", fn);
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
  error (NULL, buf);
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
    VOIDRET strcpy (tmp, c);
  return tmp;
}

#if 0
#if defined(MSDOS) | defined(OS2)
#include <io.h>
void
xsetmode (int handle, int mode)
{
  if (-1 == setmode (handle, mode))
    xperror ("setmode failed");
}
#endif
#endif

#if !defined (HAVE_STRERROR)
extern char *sys_errlist[];
extern int sys_nerr;

char *
strerror (num)
     int num;
{
  if (num >= sys_nerr)
    return ("Unknown file system error");
  else
    return (sys_errlist[num]);
}
#endif /* !HAVE_STRERROR */

/* From emacs/src/sysdep.c */
#ifndef HAVE_MKDIR
/*
 * Written by Robert Rother, Mariah Corporation, August 1985.
 *
 * If you want it, it's yours.  All I ask in return is that if you
 * figure out how to do this in a Bourne Shell script you send me
 * a copy.
 *					sdcsvax!rmr or rmr@uscd
 *
 * Severely hacked over by John Gilmore to make a 4.2BSD compatible
 * subroutine.	11Mar86; hoptoad!gnu
 *
 * Modified by rmtodd@uokmax 6-28-87 -- when making an already existing dir,
 * subroutine didn't return EEXIST.  It does now.
 */

/*
 * Make a directory.
 */
#ifdef MKDIR_PROTOTYPE
MKDIR_PROTOTYPE
#else
int
mkdir (dpath, dmode)
     char *dpath;
     int dmode;
#endif
{
  int cpid, status, fd;
  struct stat statbuf;

  if (stat (dpath, &statbuf) == 0)
    {
      errno = EEXIST;		/* Stat worked, so it already exists */
      return -1;
    }

  /* If stat fails for a reason other than non-existence, return error */
  if (errno != ENOENT)
    return -1;

  synch_process_alive = 1;
  switch (cpid = fork ())
    {

    case -1:			/* Error in fork */
      return (-1);		/* Errno is set already */

    case 0:			/* Child process */
      /*
		 * Cheap hack to set mode of new directory.  Since this
		 * child process is going away anyway, we zap its umask.
		 * FIXME, this won't suffice to set SUID, SGID, etc. on this
		 * directory.  Does anybody care?
		 */
      status = umask (0);	/* Get current umask */
      status = umask (status | (0777 & ~dmode));	/* Set for mkdir */
      fd = sys_open("/dev/null", 2);
      if (fd >= 0)
        {
	  dup2 (fd, 0);
	  dup2 (fd, 1);
	  dup2 (fd, 2);
        }
      execl ("/bin/mkdir", "mkdir", dpath, (char *) 0);
      _exit (-1);		/* Can't exec /bin/mkdir */

    default:			/* Parent process */
      wait_for_termination (cpid);
    }

  if (synch_process_death != 0 || synch_process_retcode != 0)
    {
      errno = EIO;		/* We don't know why, but */
      return -1;		/* /bin/mkdir failed */
    }

  return 0;
}
#endif /* not HAVE_MKDIR */

#undef strlen
size_t
xstrlen (const char *c, const char *fn, size_t ln)
{
  static size_t n = 0;
  if (c == NULL)
    error (ewfile(fn,ln), "strlen #%d called with NULL argument", ++n);
  return strlen (c);
}


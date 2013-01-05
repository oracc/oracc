/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: psd_base.h,v 0.7 1998/12/19 06:11:55 s Exp s $
*/

#ifndef PSD_BASE_H_ 
#define PSD_BASE_H_ 1

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#ifdef __MWERKS__

#define STDC_HEADERS 1
#define HAVE_GETCWD 1
#define HAVE_LIMITS_H 1
/*NB: MWERKS CW 4 mkdir is broken; mkdir is defined as _mkdir, which is
  prototyped as having only a path argument.  Fix the stat.win32.h header
  to define mkdir(d,m) as _mkdir(d)
 */
#define HAVE_MKDIR 1
#define HAVE_STAT_H 1
#define HAVE_STRERROR 1
#define HAVE_STRING_H 1
#define HAVE_UNISTD_H 1
#define HAVE_VPRINTF 1

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#include <unix.h>

#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>


#define STDC_HEADERS 1


#ifdef STDC_HEADERS
#include <stdlib.h>
#else
void exit ();
# ifdef HAVE_MALLOC_H
#  include <malloc.h>
# else
   char *malloc ();
   char *realloc ();
   char *calloc ();
   void free ();
# endif
#endif

#if HAVE_STAT_H
#include <stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif
#endif /*HAVE_STAT_H*/

/* FIXME: the sources don't use this stuff yet; see textutils/lib/error.c
   for an example of how to do so.  Any file using va_start needs to be
   fixed. */
#if HAVE_VPRINTF || HAVE_DOPRNT || _LIBC
# if __STDC__
#  include <stdarg.h>
#  define VA_START(args, lastarg) va_start(args, lastarg)
# else
#  include <varargs.h>
#  define VA_START(args, lastarg) va_start(args)
# endif
#else
# define va_alist a1, a2, a3, a4, a5, a6, a7, a8
# define va_dcl char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
# ifndef strchr
#  define strchr index
# endif
#endif

#include <assert.h>

#if 0
#ifndef NDEBUG
#undef strlen
#define strlen(s) xstrlen(s,__FILE__,__LINE__)
#include <xsystem.h>
#endif
#endif

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#if HAVE_LIMITS_H
# include <limits.h>
#endif

#ifndef ULONG_MAX
# define ULONG_MAX ((unsigned long) ~(unsigned long) 0)
#endif

#ifndef LONG_MAX
# define LONG_MAX ((long int) (ULONG_MAX >> 1))
#endif

#if defined (TM_IN_SYS_TIME)
#include <sys/time.h>
#else
#include <time.h>
#endif /* !TM_IN_SYS_TIME */

#if defined (HAVE_SYS_FCNTL_H)
#include <sys/fcntl.h>
#else
#include <fcntl.h>
#endif /* !HAVE_SYS_FCNTL_H */

#ifndef HAVE_GETCWD
#define getcwd(dir,len) getwd(dir)
#endif /* !HAVE_GETCWD */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/**************************************************************/

/* Set an OS type; in each case, more compiler-specified defines
   could be tested for to ensure that the base OS type gets set
   up properly.  I think that for the purposes of the PSD sources
   MSDOS should be sufficient rather than special WINxxx identifiers,
   but I could be wrong.  Mac these days assumes MACOSX. */

#if defined(__TURBOC__) || defined(_QC)
#define MSDOS	1
#endif

#if defined(__MWERKS__)
#if defined(__INTEL__)
#define WIN 1
#define MSDOS 1
#else
#define MAC 1
#endif
#endif

#if defined(__linux__) || defined(unix)
#define UNIX 1
#endif

#ifdef __EMX__
#define OS2 1
#endif

#if defined(__APPLE__)
#define MACOSX 1
#endif

/**************************************************************/

/* FIXME: is this section necessary with more recent compilers?
   perhaps a single OS16BIT define should be used at configuration
   to include/exclude this stuff?  It all dates to some years back
   when I still cared about 16 bit systems; perhaps we shouldn't
   care any more? */

/* Microsoft QC 2.5 doesn't allow math with void pointers and maybe
 * others don't either. If yours is one of these, use the second def,
 * otherwise use the first. (Turbo C doesn't allow math with void's
 * either: I guess the 16 bit compilers can't guarantee the pointer
 * size with near and far etc.) */
#ifdef __GCC__
#define VOIDMATH
#else
#define VOIDMATH	(char *)
#endif

/* Dummy QC 2.5 also complains about use of (void) to explicitly ignore
 * a return value, so we use VOIDRET instead, and define it appropriately.
 */
#ifdef _QC
#define VOIDRET
#else
#define VOIDRET		(void)
#endif
#define VRET VOIDRET

/* const handling is also different in TC/QC from POSIX, so it's easiest to
 * dump const altogether in TC/QC. 
 */
#if defined (_QC) || defined (__TURBOC__)
#define const
#endif

/* size_t is 64K in a 16 bit system, and there's generally some overhead
 * in malloc also, so this macro is used to check possible overflow 
 * situations and error out of them (we assume a max of 512 bytes 
 * reserved by malloc's)
 */
#ifdef MSDOS
#define DOS16_CHECK(t)	if ((t)>(64*1016L)) error(NULL,"DOS 64K memory limit exceeded.");
#define DOS16(t)	if ((t)>(65535u))   error(NULL,"DOS 64K limit exceeded.");
#else
#define DOS16_CHECK(t)
#define DOS16(t)
#endif

/**************************************************************/

/**Establish the system dependent file name delimiters.
 * 
 * Some file systems use CR and LF to delimit lines, these should have
 * CRLF defined to 1 (CRLF support is probably inadequate in the sources
 * at present).
 *
 * How about Macs \r only? How about supporting remote devices
 * (ftp-addresses) within the framework of the fname routines?
 */
#if defined (MSDOS) || defined(WIN) || defined (OS2)
#define DIRSEP_CHAR     '/'     /* the pathname routines all do \ mapping */
#define DIRSEP_STRING   "/"
#define EXTSEP_STRING	"."
#define EXE_EXT		"exe"
#define CRLF	1
#define NEWLINE_LEN	2
#if defined(MSDOS) || defined(WIN)
#define CALL_OS_SPEC	"COMSPEC"
#define CALL_OS_SHELL	"command.com"
#define CALL_OS_MINUSC  "/c"
#else
#define CALL_OS_SPEC	"OS2_SHELL"
#define CALL_OS_SHELL	"cmd.exe"
#define CALL_OS_MINUSC  "/c"
#endif /* !MSDOS */
#ifdef __MWERKS__
#define CALL_WITH_FORK 		0
#define CALL_WITH_EXEC		1
#define CALL_WITH_SPAWN 	0
#define CALL_WITH_SYSTEM 	0
#else
#define CALL_WITH_FORK 		0
#define CALL_WITH_SPAWN 	1
#define CALL_WITH_SYSTEM 	0
#endif /*__MWERKS__*/
#else
#if (UNIX|MACOSX)
#define DIRSEP_CHAR     '/'
#define DIRSEP_STRING   "/"
#define EXTSEP_STRING	"."
#define EXE_EXT		""
#define CRLF	0
#define NEWLINE_LEN	1
#define CALL_OS_SPEC	"SHELL"
#define CALL_OS_SHELL	"/bin/sh"
#define CALL_OS_MINUSC  "-c"
#define CALL_WITH_FORK 		1
#define CALL_WITH_SPAWN 	0
#define CALL_WITH_SYSTEM 	0
#else
#if (MAC)
#define DIRSEP_CHAR     ':'
#define DIRSEP_STRING   "/"
#define EXTSEP_STRING	"."
#define NEWLINE_LEN		1
#define CALL_OS_SPEC	""
#define CALL_OS_SHELL	""
#define CALL_OS_MINUSC  ""
#define EXE_EXT			""
#define CRLF			0
#endif /*MAC*/
#endif /*UNIX*/
#endif /*MSDOS|WIN|OS2*/

/**this macro must return TRUE if a filename is a relative path
 */
#if defined (MSDOS) || defined(WIN) || defined (OS2)
#define RELPATH(p)\
        ((p)[0] != '.' && (p)[0] != '/' && (p)[0] != '\\' && (p)[1] != ':')
#else   /* works for UNIX, don't know about MAC */
#if UNIX
#define RELPATH(p)      ((p)[0] != '.' && (p)[0] != '/')
#else
#define RELPATH(p)		0 /* I have to get a Mac book ...*/
#endif /*!UNIX*/
#endif /*MSDOS|WIN|OS2*/

/*The maximum length of the basename part of a file, including 0*/
#define MAX_BASENAME_		((size_t)9)

#ifndef _MAX_PATH
#define _MAX_PATH       1024
#endif	/*!_MAX_PATH*/

#undef min
#undef max
#define min(a,b)	((a)>(b)?(b):(a))
#define max(a,b)	((a)>(b)?(a):(b))

#ifdef MSDOS
#define SIZET_MAX	USHRT_MAX
#else
#define SIZET_MAX	ULONG_MAX
#endif

/**************************************************************/
/* Include the most used library header files. */
#include <psdtypes.h>
#include <xsystem.h>
#include <messages.h>
#include <misc.h>

/**************************************************************/

typedef int sort_cmp_func(const void*,const void*);

extern const char *prog;
extern int major_version;
extern int minor_version;

#endif /*PSD_BASE_H*/

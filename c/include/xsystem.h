/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: xsystem.h,v 0.8 1998/12/19 06:13:36 s Exp $
*/

#ifndef XSYSTEM_H_
#define XSYSTEM_H_ 1

#include <psdtypes.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef SETVBUF_REVERSED
#define xsetvbuf(a,b,c) setvbuf(a,c,b)
#else
#define xsetvbuf setvbuf
#endif

#if defined(MSDOS) | defined(OS2)
#include <io.h>
#include <fcntl.h>
extern void xsetmode (int handle, int mode);
#else
#define xsetmode(h,m)
#endif

#if defined(__MWERKS__) & defined(MAC)
#define access mac_access
int mac_access(const char*path, int amode);
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif

#define  xfseek(n,s,o,w) { \
			   if (fseek((s),(o),(w))) \
			     { \
			       error (NULL, "seek failed on %s", n); \
			     } \
			 }

#define xfputc(n,c,s) { if (EOF==fputc(c,s)) \
                          error(NULL,"write failed on %s",s);}

extern int xaccess (const char *path, int amode, Boolean quit);
extern int xmkdir (const char *path, mode_t mode, Boolean quit);
extern char *xgetcwd (void);
extern char *xstrdup (const char *str);
extern size_t xstrlen (const char *str, const char *fn, size_t ln);
extern FILE *xfopen (const char *fn, const char *mode);
extern void xfclose (const char *fn, FILE*fp);
extern void xstat (const char *fn, struct stat *res);
extern void xfstat (const char *fn, int handle, struct stat *res);
extern size_t xfwrite (const char *fn, Boolean binflag, const void *buf, 
		       size_t size, size_t count, FILE*fp);
extern size_t xfread (const char *fn, Boolean binflag, const void *buf, 
		      size_t size, size_t count, FILE*fp);
extern void xremove (const char *fn);
extern void xrename (const char *oldfn, const char *newfn);
extern void xperror (const char *fmt, ...);

#endif /*XSYSTEM_H_*/

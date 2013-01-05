/*************************************************************************************************
 * System-dependent configurations of Hyper Estraier
 *                                                      Copyright (C) 2004-2007 Mikio Hirabayashi
 * This file is part of Hyper Estraier.
 * Hyper Estraier is free software; you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation; either
 * version 2.1 of the License or any later version.  Hyper Estraier is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with Hyper
 * Estraier; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA.
 *************************************************************************************************/


#ifndef _MYCONF_H                        /* duplication check */
#define _MYCONF_H



/*************************************************************************************************
 * common settings
 *************************************************************************************************/


#undef TRUE
#define TRUE           1
#undef FALSE
#define FALSE          0

#if defined(_MYNOZLIB)
#define ESTUSEZLIB     FALSE
#else
#define ESTUSEZLIB     TRUE
#endif

#if defined(_MYLZO)
#define ESTUSELZO      TRUE
#else
#define ESTUSELZO      FALSE
#endif

#if defined(_MYBZIP)
#define ESTUSEBZIP     TRUE
#else
#define ESTUSEBZIP     FALSE
#endif

#if defined(_MYMECAB)
#define ESTUSEMECAB    TRUE
#else
#define ESTUSEMECAB    FALSE
#endif



/*************************************************************************************************
 * system discrimination
 *************************************************************************************************/


#if defined(__linux__)

#define _SYS_LINUX_
#define ESTSYSNAME  "Linux"

#elif defined(__FreeBSD__)

#define _SYS_FREEBSD_
#define ESTSYSNAME  "FreeBSD"

#elif defined(__NetBSD__)

#define _SYS_NETBSD_
#define ESTSYSNAME  "NetBSD"

#elif defined(__OpenBSD__)

#define _SYS_OPENBSD_
#define ESTSYSNAME  "OpenBSD"

#elif defined(__sun__)

#define _SYS_SUNOS_
#define ESTSYSNAME  "SunOS"

#elif defined(__hpux)

#define _SYS_HPUX_
#define ESTSYSNAME  "HP-UX"

#elif defined(__osf)

#define _SYS_TRU64_
#define ESTSYSNAME  "Tru64"

#elif defined(_AIX)

#define _SYS_AIX_
#define ESTSYSNAME  "AIX"

#elif defined(__APPLE__) && defined(__MACH__)

#define _SYS_MACOSX_
#define ESTSYSNAME  "Mac OS X"

#elif defined(_MSC_VER)

#define _SYS_MSVC_
#define ESTSYSNAME  "Windows (VC++)"

#elif defined(_WIN32)

#define _SYS_MINGW_
#define ESTSYSNAME  "Windows (MinGW)"

#elif defined(__CYGWIN__)

#define _SYS_CYGWIN_
#define ESTSYSNAME  "Windows (Cygwin)"

#else

#define _SYS_GENERIC_
#define ESTSYSNAME  "Generic"

#endif



/*************************************************************************************************
 * general headers
 *************************************************************************************************/


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <depot.h>
#include <curia.h>
#include <cabin.h>
#include <villa.h>

#include <regex.h>
#include <iconv.h>

#if ESTUSEZLIB
#include <zlib.h>
#endif

#if ESTUSELZO
#include <lzo/lzo1x.h>
#endif

#if ESTUSEBZIP
#include <bzlib.h>
#endif

#include "md5.h"

#if defined(_SYS_MSVC_)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <direct.h>
#include <windows.h>
#include <io.h>
#include <winsock2.h>

#elif defined(_SYS_MINGW_)

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>
#include <dirent.h>
#include <windows.h>
#include <io.h>
#include <winsock2.h>

#elif defined(_SYS_CYGWIN_)

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <utime.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <windows.h>
#include <io.h>

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <utime.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif

#if ! defined(_QDBM_LIBVER)
#error QDBM is required but not found.
#elif _QDBM_LIBVER < 1411
#error This version of QDBM is too old.  Use 1.8.75 or later.
#endif

#if defined(__va_copy)
#define est_va_copy(a, b) \
  __va_copy(a, b)
#elif defined(va_copy)
#define est_va_copy(a, b) \
  va_copy(a, b)
#else
#define est_va_copy(a, b) \
  (a = b)
#endif



/*************************************************************************************************
 * notation of paths in the file system
 *************************************************************************************************/


#if defined(_SYS_MSVC_) || defined(_SYS_MINGW_)

#define ESTPATHCHR      '\\'
#define ESTPATHSTR      "\\"
#define ESTEXTCHR       '.'
#define ESTEXTSTR       "."
#define ESTCDIRSTR      "."
#define ESTPDIRSTR      ".."

#define \
  open(pathname, flags, mode) \
  open(pathname, flags | O_BINARY, mode)

#else

#define ESTPATHCHR      '/'
#define ESTPATHSTR      "/"
#define ESTEXTCHR       '.'
#define ESTEXTSTR       "."
#define ESTCDIRSTR      "."
#define ESTPDIRSTR      ".."

#endif



#endif                                   /* duplication check */


/* END OF FILE */

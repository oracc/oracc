/** @file jpegcom.c
 * Read or write comments in JFIF or EXIF JPEG files
 * @author Marko Mäkelä <msmakela@nic.funet.fi>
 */

/* Copyright © 2003,2004 Marko Mäkelä.

   This file is part of JPEGCOM, a program for reading and writing
   comments in digital photographs.

   JPEGCOM is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   JPEGCOM is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   The GNU General Public License is often shipped with GNU software, and
   is generally kept in a file called COPYING or LICENSE.  If you do not
   have a copy of the license, write to the Free Software Foundation,
   59 Temple Place, Suite 330, Boston, MA 02111 USA. */

#if defined WIN32 || defined __WIN32
# undef __STRICT_ANSI__
# include <sys/types.h>
# include <sys/utime.h>
#else
# ifndef _POSIX_SOURCE
#  define _POSIX_SOURCE
# endif
# include <sys/types.h>
# include <utime.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <locale.h>

#include "jpeginfo.h"

#if 0
/** Read 16 bits from file, most significant byte first
 * @param file	the file
 * @return	the data read
 */
static unsigned
read_word (FILE* file)
{
  unsigned word;
  word = (unsigned) getc (file);
  word <<= 8;
  word |= (unsigned) getc (file);
  return word;
}
#endif

/** Main program
 * @param argc	argument count
 * @param argv	argument vector
 * @return	zero on success
 */
int
main (int argc, char** argv)
{
  setlocale (LC_ALL, "");

  if (argc > 1) {
    int i;
    for (i = 1; i < argc; i++) {
      FILE* f = fopen (argv[i], "rb");
      struct imageinfo ii;
      const char* s;

      if (!f) 
	{
	  (void) fputs (argv[i], stderr), (void) fflush (stderr);
	  perror (": open (reading)");
	} 
      else 
	{
	  if ((s = readinfo (&ii, NULL, f))) {
	    (void) fputs (argv[i], stderr);
	    (void) fputs (": ", stderr);
	    (void) fputs (s, stderr);
	    (void) putc ((int) '\n', stderr);
	  } else {
	    struct stat st;
	    stat(argv[i],&st);
	    printf("%s\t%ld\t%d\t%d\n",argv[i],(long)st.st_mtime,ii.width,ii.height);
	  }
	  (void) fclose (f);
	}
    }
  }
  
  return 0;
}

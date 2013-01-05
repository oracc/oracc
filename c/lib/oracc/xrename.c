/* xrename.c: conceptually rename with error checking.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* Copy FROM to TO, then unlink FROM.  It would probably be faster to
   read and write buffers, but the files we use this for aren't big
   enough for it to make a substantial difference.  */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "xsystem.h"

void
xrename(const char * from, const char * to)
{
  int c;
  FILE *input = xfopen (from, "r");
  FILE *output = xfopen (to, "w");

  while ((c = getc (input)) != EOF || !feof (input))
    putc (c, output);

  xfclose (from, input);
  xfclose (to, output);

  if (unlink (from) != 0)
    {
      fprintf (stderr, "warning: Could not unlink ");
      perror (from);
    }
}

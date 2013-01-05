/** @file info.h
 * Gather information of JFIF and EXIF JPEG files
 * @author Marko Mäkelä <msmakela@nic.funet.fi>
 */

/* Copyright © 2003 Marko Mäkelä.

   This file is part of PHOTOMOLO, a program for generating
   thumbnail images and HTML files for browsing digital photographs.

   PHOTOMOLO is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   PHOTOMOLO is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   The GNU General Public License is often shipped with GNU software, and
   is generally kept in a file called COPYING or LICENSE.  If you do not
   have a copy of the license, write to the Free Software Foundation,
   59 Temple Place, Suite 330, Boston, MA 02111 USA. */

#include <stdio.h>

/** Image comments */
struct comment
{
  /** Pointer to next comment */
  struct comment* next;
  /** The comment string */
  char text[1];
};

/** Image file entry */
struct imageinfo
{
  /** width in pixels */
  unsigned width;
  /** height in pixels */
  unsigned height;
};

/** Read image info
 * @param info	the information structure
 * @param comm	(output) pointer to the comment structure (optional)
 * @param file	the file pointer to the image
 * @return	NULL on success, error message on failure
 */
const char*
readinfo (struct imageinfo* info,
	  struct comment** comm,
	  FILE* file);

/** Deallocate image comments
 * @param comment	the comments
 */
void
freeinfo (struct comment* comment);

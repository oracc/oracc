/** @file info.c
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

#include "jpeginfo.h"
#include <stdlib.h>
#include <string.h>

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

/** Find the next JPEG marker code
 * @param file	the file
 * @return	the marker code found, or EOF
 */
static int
next_marker (FILE* file)
{
  int c;
  /* find the next 0xff */
  do {
    c = getc (file);
    if (c == EOF)
      return c;
  }
  while (c != 0xff);
  /* find the marker, ignoring subsequent 0xff bytes */
  do {
    c = getc (file);
    if (c == EOF)
      return c;
  }
  while (c == 0xff);
  return c;
}

/** Read image info
 * @param info	the information structure
 * @param comm	(output) pointer to the comment structure (optional)
 * @param file	the file pointer to the image
 * @return	NULL on success, error message on failure
 */
const char*
readinfo (struct imageinfo* info,
	  struct comment** comm,
	  FILE* file)
{
  memset (info, 0, sizeof *info);
  if (read_word (file) != 0xffd8)
    return "not a JPEG image file";
  /* scan markers until SOS */
  for (;;) {
    unsigned length;
    int marker = next_marker (file);
    if (marker == EOF) {
    eof:
      return "premature end of file";
    }
    if ((marker & 0xf0) == 0xc0 &&
	marker != 0xc4 && marker != 0xcc) {
      /* start of frame marker (image dimensions) */
      /** number of colour components */
      int c;
      length = read_word (file);
      if (length < 2)
	goto invalid_length;
      (void) getc (file); /* skip data precision */
      info->height = read_word (file);
      info->width = read_word (file);
      c = getc (file);
      if (c == EOF)
	goto eof;
      if (length != (8 + ((unsigned) c) * 3))
	return "inconsistent SOF marker length";
      while (c--)
	if (getc (file) == EOF /* component ID code */ ||
	    getc (file) == EOF /* sampling factors */ ||
	    getc (file) == EOF /* quantization table number */)
	  goto eof;
      continue;
    }
    else switch (marker) {
    case 0xfe:
      if (!comm)
	goto skip_marker;
      /* comment */
      length = read_word (file);
      if (length < 2)
	goto invalid_length;
      else {
	/* read the image comment */
	struct comment* comment = malloc (length + sizeof *comment);
	if (!comment)
	  return "out of memory";
	length -= 2;
	comment->next = 0;
	if (length != fread (comment->text, 1, length, file)) {
	  free (comment);
	  goto eof;
	}
	comment->text[length] = 0;
	/* append the comment to the list */
	if (!*comm)
	  *comm = comment;
	else {
	  struct comment* c;
	  for (c = *comm; c->next; c = c->next);
	  c->next = comment;
	}
      }
      continue;
    case 0xda: /* start of scan (compressed data) */
    case 0xd9: /* end of image */
      return 0;
    }
    if (feof (file))
      goto eof;
  skip_marker:
    /* skip the rest of the marker */
    length = read_word (file);
    if (length < 2) {
    invalid_length:
      return "erroneous JPEG marker length";
    }
    length -= 2;
    while (length--)
      if (getc (file) == EOF)
	goto eof;
  }
}

/** Deallocate image info
 * @param comment	the comments
 */
void
freeinfo (struct comment* comment)
{
  register struct comment* c;
  for (c = comment; c; c = comment) {
    comment = c->next;
    free (c);
  }
}

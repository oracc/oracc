/*************************************************************************************************
 * Morphological analyzers for Hyper Estraier
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


#ifndef _MYMORPH_H                       /* duplication check */
#define _MYMORPH_H


#include <estraier.h>
#include <cabin.h>



/*************************************************************************************************
 * pseudo API
 *************************************************************************************************/


/* Extract keywords of a document object using morphological analyzers.
   `doc' specifies a document object.
   `max' specifies the maximum number of keywords to be extracted.
   The return value is a new map object of keywords and their scores in decimal string.  Because
   the object of the return value is opened with the function `cbmapopen', it should be closed
   with the function `cbmapclose' if it is no longer in use. */
CBMAP *est_morph_etch_doc(ESTDOC *doc, int max);



#endif                                   /* duplication check */

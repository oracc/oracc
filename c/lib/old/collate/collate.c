/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: collate.c,v 0.6 2001/11/30 20:50:07 s Exp s $
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <utf2uni.h>
#include "collate.h"

int collate_obey_delims = 1;
static struct Collate_info *curr_collate;
static const unsigned char **curr_tiles;
static const char collate_digit_values[12] = {0,1,2,3,4,5,6,7,8,9,100,200};

void
collate_init (const unsigned char *name)
{
  struct Collate_info_list_node *cp;
  for (cp = collate_infos; cp->name; ++cp)
    if (!strcmp (name, cp->name))
      break;
  if (NULL == cp->name)
    {
      fprintf (stderr, "collate: unknown collate table name `%s'\n", name);
      exit (1);
    }
  else
    curr_collate = cp->coll;
}

void
collate_term ()
{
  curr_collate = NULL;
}

static inline Uchar
keyval (unsigned short u)
{
  size_t tile = u/256;

  if (NULL == curr_tiles[tile])
    return '\0';
  return curr_tiles[tile][u%256];
}

Uchar
collate_keyval (unsigned short u)
{
  return keyval(u);
}

Boolean
collate_isdelim (Uchar keychar)
{
  return curr_collate->delims[keychar];
}

Uchar *
collate_makekey (Uchar *s)
{
  Uchar *src, *dst;
  src = dst = s;
  curr_tiles = curr_collate->tiles;
  while (*s)
    {
      int inc = *s < 0x80 ? 1 : ((!(*s & 0x20)) ? 2 : 3);
      if (*dst = keyval (UTF2Unicode(s)))
	++dst;
      s += inc;
    }
  *dst = '\0';
  return src;
}

Uchar *
collate_makekey_i (Uchar *s)
{
  Uchar *src, *dst, *chk;
  src = dst = s;
  curr_tiles = curr_collate->tiles_i;
  while (isspace(*s))
    ++s;
  while (*s)
    {
      int inc = *s < 0x80 ? 1 : ((!(*s & 0x20)) ? 2 : 3);
      if (*dst = keyval (UTF2Unicode(s)))
	++dst;
      s += inc;
    }
  *dst = '\0';
  /* why is this code only in collate_makekey_i ? */
  for (chk = src; chk && curr_collate->delims[*chk]; ++chk)
    ;
  if (!*chk)
    *src = '\0';
  return src;
}

Uchar *
collate_significant(Uchar *s, Boolean foldcase)
{
  Uchar *src, *dst;
  src = dst = s;
  if (foldcase)
    collate_set_tiles_i();
  else
    collate_set_tiles();
  while (*s)
    {
      int inc = *s < 0x80 ? 1 : ((!(*s & 0x20)) ? 2 : 3);
      if (keyval (UTF2Unicode(s)))
	{
	  while (inc--)
	    *dst++ = *s++;
	}
      else
	s += inc;
    }
  *dst = '\0';
  return src;
}


static inline int
numeric (const Uchar *s)
{
  int accumulator = 0;

  if (!curr_collate->digits[*s])
    return -1;

  do
    {
      int val = collate_digit_values[*s - curr_collate->digit_offset];

      accumulator *= 10;

      if (val > 9) {
	accumulator += 100;
      } else {
	accumulator += val;
      }

    }
  while (curr_collate->digits[*++s]);

  return accumulator;
}

void
collate_set_tiles ()
{
  curr_tiles = curr_collate->tiles;
}

void
collate_set_tiles_i ()
{
  curr_tiles = curr_collate->tiles_i;
}

int
collate_cmp_utf8 (const Uchar *k1, const Uchar *k2)
{
  const Uchar *kk1 = k1, *kk2 = k2;
  while (*kk1 && *kk2)
    {
      int ret = (int)keyval(UTF2Unicode(kk1)) 
	- (int)keyval(UTF2Unicode(kk2));
      if (ret)
	return ret;
      else
	{
	  int kk1inc = *kk1 < 0x80 ? 1 : ((!(*kk1 & 0x20)) ? 2 : 3);
	  int kk2inc = *kk2 < 0x80 ? 1 : ((!(*kk2 & 0x20)) ? 2 : 3);
	  kk1 += kk1inc;
	  kk2 += kk2inc;
	}
    }
  return (int)keyval(UTF2Unicode(kk1)) 
    - (int)keyval(UTF2Unicode(kk2));
}

/* compare strings grapheme by grapheme */
int
collate_cmp_graphemes (const Uchar *k1, const Uchar *k2)
{
  static Uchar k1buf[1024], k2buf[1024];
  const Uchar *kk1, *kk2, *b1, *b2, *d1, *d2, *e1, *e2, *max_e1, *max_e2;
  int first_pass = 1;

START:

  if (first_pass)
    {
      strcpy(k1buf,k1);
      strcpy(k2buf,k2);
      kk1 = collate_makekey_i(k1buf);
      kk2 = collate_makekey_i(k2buf);
      while (curr_collate->delims[*kk1])
	++kk1;
      while (curr_collate->delims[*kk2])
	++kk2;
      max_e1 = kk1 + strlen (kk1);
      max_e2 = kk2 + strlen (kk2);
      while (curr_collate->delims[max_e1[-1]])
	--max_e1;
      while (curr_collate->delims[max_e2[-1]])
	--max_e2;
    }
  else
    {
      strcpy(k1buf,k1);
      strcpy(k2buf,k2);
      kk1 = collate_makekey(k1buf); 
      kk2 = collate_makekey(k2buf);
      max_e1 = kk1 + strlen (kk1);
      max_e2 = kk2 + strlen (kk2);
    }

  for (b1 = kk1, b2 = kk2; 
       *b1 && *b2; 
       b1 = (e1 < max_e1) ? e1+1 : e1, b2 = (e2 < max_e2) ? e2+1 : e2)
    {
      while (curr_collate->delims[*b1])
	++b1;
      while (curr_collate->delims[*b2])
	++b2;
      for (d1 = NULL, e1 = b1; e1 < max_e1; ++e1)
	{
	  if (curr_collate->digits[*e1])
	    d1 = e1;
	  if (curr_collate->delims[*e1])
	    break;
	}
      if (!d1)
	d1 = e1;

      for (d2 = NULL, e2 = b2; e2 < max_e2; ++e2)
	{
	  if (curr_collate->digits[*e2])
	    d2 = e2;
	  if (curr_collate->delims[*e2])
	    break;
	}
      if (!d2)
	d2 = e2;

      /* compare up to digits first */
      while (*b1 == *b2 && b1 < d1 && b2 < d2)
	{
	  ++b1;
	  ++b2;
	}

      if (b1 < d1 || b2 < d2) /* keys differ before digits */
	{
	  if (b1 < d1 && b2 < d2)
	    return (int)*b1 - (int)*b2;
	  if (b1 < d1) /* b1's grapheme text is longer than b2's */
	    return 1;
	  if (b2 < d2) /* b2's grapheme text is longer than b1's */
	    return -1;
	}

      if (d1 != e1 || d2 != e2) /* either or both graphemes had digits */
	{
	  int i1 = numeric(d1);
	  int i2 = numeric(d2);
	  if (i1 < i2)
	    return -1;
	  else if (i1 > i2)
	    return 1;
	}

      /* maybe we are sorting by multiple delimiter types */
      if ((!first_pass || collate_obey_delims) && *e1 != *e2)
	return (int)*e1 - (int)*e2;

      /* graphemes (and delimiters) were identical */
    }

  /* We only get here if either or both of b1 and b2 were exhausted */
  if (*b1)
    return 1;
  else if (*b2)
    return -1;
  else if (first_pass)
    {
      first_pass = 0;
      goto START;
    }
  else
    return 0;
}

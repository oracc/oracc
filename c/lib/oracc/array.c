/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: array.c,v 0.3 1997/09/08 14:50:02 sjt Exp $
*/

#include <psd_base.h>
#include <array.h>

/**merge two sorted arrays.
 *
 * if we are inverting, all the elements which are not in both arrays are
 *    put into the common array
 * otherwise only elements in both arrays are put into the common array
 *
 */
void *
array_merge (Boolean invert, void *arg_ap1, void *arg_ap2, size_t size, 
		size_t arg_count1, size_t arg_count2, size_t *new_count,
		int (*cmp)(const void *, const void *))
{
  register size_t i1, i2, j;
  size_t base1, base2;
  size_t count1, count2;
  void *new_ap;
  
  int ret;
  void *ap1, *ap2;

  if (NULL == arg_ap1 || NULL == arg_ap2 || arg_count1 == 0 || arg_count2 == 0)
    {
      if (NULL != new_count)
	*new_count = 0;
      return NULL;
    }
  else
    new_ap = malloc ((size * arg_count1) + (size * arg_count2));

  /* First set things up so ap1 and count1 refer to the shortest array.
     This also ensures that a zero length list will terminate the merge
     immediately. */
  if (arg_count1 <= arg_count2)
    {
      ap1 = arg_ap1;
      ap2 = arg_ap2;
      count1 = arg_count1;
      count2 = arg_count2;
    }
  else
    {
      ap2 = arg_ap1;
      ap1 = arg_ap2;
      count2 = arg_count1;
      count1 = arg_count2;
    }

  i1 = i2 = j = 0;
  while (i1 < count1 && i2 < count2)
    {
      ret = cmp ((VOIDMATH ap1) + (i1 * size), (VOIDMATH ap2) + (i2 * size));
      if (0 == ret)
	{
	  if (!invert)
	    memcpy ((VOIDMATH new_ap) + (j++ * size),
		    (VOIDMATH ap1) + (i1 * size),
		    size);
	  ++i1;
	  ++i2;
	}
      else if (ret < 0)		/* *ap1 is less than *ap2, move it to >= */
	{
	  base1 = i1;
	  while (i1 < count1 && 
		 (ret = cmp ((VOIDMATH ap1) + (i1 * size), (VOIDMATH ap2) + (i2 * size))) < 0)
	    ++i1;
	  if (invert && i1 > base1)
	    memcpy ((VOIDMATH new_ap) + (j++ * size), (VOIDMATH ap1) + (base1 * size),
		    (i1 - base1) * size);
	}
      else			/* *ap2 is less than *ap1, move it to >= */
	{
	  base2 = i2;
	  while (i2 < count2
	      && (ret = cmp((VOIDMATH ap2) + (i2 * size), (VOIDMATH ap1) + (i1 * size))) < 0)
	    ++i2;
	  if (invert && i2 > base2)
	    memcpy ((VOIDMATH new_ap) + (j++ * size), 
		    (VOIDMATH ap2) + (base2 * size), 
		    (i2 - base2) * size);
	}
    }

  /* there may still be stuff in ap2 that we need to add */
  if (count2 > i2 && invert)
    {
      memcpy ((VOIDMATH new_ap) + (j * size), 
	      (VOIDMATH ap2) + (i2 * size), 
	      (count2 - i2) * size);
      j += count2 - i2;
    }

  if (j)
    new_ap = realloc (new_ap, j * size);
  else
    {
      if (NULL != new_ap)
	{
	  free (new_ap);
	  new_ap = NULL;
	}
    }

  *new_count = j;
  return new_ap;
}

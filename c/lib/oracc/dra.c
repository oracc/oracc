/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: dra.c,v 0.3 1997/09/08 14:50:03 sjt Exp $
*/

#include <psd_base.h>
#include <dra.h>

Array *
dra_create (size_t element_size, size_t initial_alloc, size_t increment)
{
  Array *tmp = malloc (sizeof (Array));
  if (initial_alloc)
    {
      tmp->allocated = initial_alloc * element_size;
      tmp->data = malloc (tmp->allocated);
    }
  else
    {
      tmp->allocated = 0;
      tmp->data = NULL;
    }
  tmp->in_use = 0;
  tmp->element_size = element_size;
  tmp->increment = increment * element_size;
  return tmp;
}

/**reallocate mem used by ap so it's exactly what is needed and no more.
 */
void
dra_resize (Array*ap)
{
  ap->allocated = ap->in_use;
  if (ap->in_use)
    ap->data = realloc (ap->data, ap->in_use);
  else
    {
      free (ap->data);
      ap->data = NULL;
    }
}

Array *
dra_attach (void *data, size_t element_size, size_t count)
{
  Array *tmp = dra_create (element_size, 0, 0);
  tmp->data = data;
  tmp->in_use = tmp->allocated = count * element_size;
  return tmp;
}

void *
dra_detach (Array*ap, size_t *count)
{
  void *tmp;
  if (NULL == ap)
    {
      if (NULL != count)
        *count = 0;
      return NULL;
    }
  dra_resize (ap);
  tmp = ap->data;
  if (NULL != count)
    *count = ap->in_use / ap->element_size;
  free (ap);
  return tmp;
}

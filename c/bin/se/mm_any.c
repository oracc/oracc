/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: mm_any.c,v 0.3 1997/09/08 14:50:19 sjt Exp $
*/
#include <string.h>
#include <psd_base.h>
#include "mm_any.h"

struct mem_block
{
  void *mem;
  struct mem_block *next;
};

struct any_type *
init_mm (int element_size, int elements_per_alloc)
{
  struct any_type *any = malloc (sizeof (struct any_type));
  any->element_size = element_size;
  any->elements_per_alloc = elements_per_alloc;
  any->last_element_used = 0;
  any->mem_rover = any->mem_base = malloc (sizeof (struct mem_block));
  any->mem_rover->mem = calloc (any->elements_per_alloc, any->element_size);
  any->mem_rover->next = NULL;
  return any;
}

void *
new (struct any_type *any)
{
  if (any->last_element_used == any->elements_per_alloc)
    {
      if (NULL == any->mem_rover->next)
	{
	  any->mem_rover->next = malloc (sizeof (struct mem_block));
	  any->mem_rover = any->mem_rover->next;
	  any->mem_rover->mem = calloc (any->elements_per_alloc ,
					any->element_size);
	  any->mem_rover->next = NULL;
	}
      else
	{
	  any->mem_rover = any->mem_rover->next;
	}
      any->last_element_used = 0;
    }
  return ((char*)any->mem_rover->mem) + any->element_size*any->last_element_used++;
}

void
reset (struct any_type *any)
{
  struct mem_block *mp;

  for (mp = any->mem_base; mp; mp = mp->next)
    memset (mp->mem, '\0', any->elements_per_alloc * any->element_size);
  
  any->mem_rover = any->mem_base;
  any->last_element_used = 0;
}

#include <string.h>
#include "memo.h"

struct memo_block
{
  void *mem;
  size_t bytes_in_block;
  struct memo_block *next;
};

static Memo *memostr = NULL;

Memo *
memo_init (int element_size, int elements_per_alloc)
{
  Memo *any = malloc(sizeof (Memo));
  any->element_size = element_size;
  any->elements_per_alloc = elements_per_alloc;
  any->last_element_used = 0;
  any->mem_rover = any->mem_base = malloc(sizeof (struct memo_block));
  any->mem_rover->mem = calloc(any->elements_per_alloc, any->element_size);
  any->mem_rover->next = NULL;
  return any;
}

void *
memo_new (Memo *any)
{
  /* > can occur when an array alloc uses more elements than
     elements_per_alloc */
  if (any->last_element_used >= any->elements_per_alloc)
    {
      if (any->mem_rover)
	any->mem_rover->bytes_in_block = (any->last_element_used * any->element_size);

      if (NULL == any->mem_rover->next)
	{
	  any->mem_rover->next = malloc (sizeof (struct memo_block));
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

void *
memo_new_array (Memo *any, int count)
{
  if (count > any->elements_per_alloc)
    {
      if (any->mem_rover)
	any->mem_rover->bytes_in_block = (any->last_element_used * any->element_size);

      if (NULL == any->mem_rover->next)
	{
	  any->mem_rover->next = malloc (sizeof (struct memo_block));
	  any->mem_rover = any->mem_rover->next;
	  any->mem_rover->mem = calloc (count,
					any->element_size);
	  any->mem_rover->next = NULL;
	}
      else
	{
	  any->mem_rover = any->mem_rover->next;
	  any->mem_rover->mem = realloc(any->mem_rover->mem,
					count * any->element_size);
	  memset(any->mem_rover->mem, '\0', count*any->element_size);
	}
      any->last_element_used = count;
      return any->mem_rover->mem;
    }
  else if ((count+any->last_element_used) > any->elements_per_alloc)
    {
      if (NULL == any->mem_rover->next)
	{
	  any->mem_rover->next = malloc (sizeof (struct memo_block));
	  any->mem_rover = any->mem_rover->next;
	  any->mem_rover->mem = calloc (any->elements_per_alloc ,
					any->element_size);
	  any->mem_rover->next = NULL;
	}
      else
	{
	  any->mem_rover = any->mem_rover->next;
	}
      any->last_element_used = count;
      return any->mem_rover->mem;
    }
  else
    {
      void *ret = ((char*)any->mem_rover->mem) + any->element_size*any->last_element_used;
      any->last_element_used += count;
      return ret;
    }
}

void
memo_reset (Memo *any)
{
  struct memo_block *mp;

  for (mp = any->mem_base; mp; mp = mp->next)
    {
      memset (mp->mem, '\0', any->elements_per_alloc * any->element_size);
      mp->bytes_in_block = 0;
    }
  
  any->mem_rover = any->mem_base;
  any->last_element_used = 0;
}

void
memo_term (Memo *any)
{
  struct memo_block *mp, *tmp;

  if (any)
    {
      for (mp = any->mem_base; mp; mp = tmp)
	{
	  tmp = mp->next;
	  if (mp->mem)
	    free(mp->mem);
	  free(mp);
	}
      free(any);
    }
}

void *
memo_merge(Memo *any, size_t *nbytes)
{
  struct memo_block *mp;
  void *m;
  size_t bytes = 0;

  for (mp = any->mem_base; mp; mp = mp->next)
    if (mp->next)
      bytes += mp->bytes_in_block;
    else
      bytes += any->last_element_used * any->element_size;
  
  m = malloc(bytes);

  for (bytes = 0, mp = any->mem_base; mp; mp = mp->next)
    {
      size_t ncopy;
      if (mp->next)
	ncopy = mp->bytes_in_block;
      else
	ncopy = any->last_element_used * any->element_size;
      memcpy(((char*)m)+bytes, mp->mem, ncopy);
      bytes += ncopy;
    }

  *nbytes = bytes;
  return m;
}

Memo_str *
memo_str(Mloc *m, const unsigned char *s)
{
  Memo_str *ms = NULL;
  if (m && s)
    {
      if (!memostr)
	memostr = memo_init(sizeof(Memo_str), 128);
      ms = memo_new(memostr);
      ms->m = *m;
      ms->s = s;
    }
  return ms;
}

  

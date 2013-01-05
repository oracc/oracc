#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pool.h"

#undef POOL_DEBUG

struct pool_block
{
  unsigned char *mem;
  unsigned char *top;
  unsigned char *last_begin;
  unsigned char *avail;
  struct pool_block *next;
};

struct pool_block *pool_rover = NULL;
struct pool_block *pool_base = NULL;

static struct pool_block *new_block(void);

void
pool_init()
{
  if (!pool_base)
    pool_base = pool_rover = new_block();
}

void
pool_term()
{
  struct pool_block *pbp,*tmp;
  for (pbp = pool_base; pbp; pbp = tmp)
    {
      tmp = pbp->next;
      free(pbp->mem);
      free(pbp);
    }
  pool_base = NULL;
}

static struct pool_block *
new_block()
{
  struct pool_block *new;
  new = malloc(sizeof(struct pool_block));
  if (!new)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
#ifdef POOL_DEBUG
  else
    {
      static int nblocks = 0;
      fprintf(stderr,"new block %d\n",++nblocks);
    }
#endif
  new->avail = new->mem = malloc(POOL_BLOCK_SIZE);
  if (!new->mem)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->top = new->mem + POOL_BLOCK_SIZE;
  new->next = NULL;
  if (pool_rover)
    pool_rover->next = new;
  return new;
}

/* FIXME: this routine will break if strlen(s)+1 > POOL_BLOCK_SIZE */
unsigned char *
pool_copy(register const unsigned char *s)
{
  size_t len;
  static unsigned char empty[1] = "";
  
  if (!s)
    return NULL;
  else if (!*s)
    return empty;
  len = strlen((char *)s) + 1;
  if ((pool_rover->avail+len) >= pool_rover->top)
    pool_rover = new_block();
  pool_rover->last_begin = memcpy(pool_rover->avail, s, len);
  pool_rover->avail += len;
#ifdef POOL_DEBUG
  fprintf(stderr,"(%s) %d used\n",s, pool_rover->avail - pool_rover->mem);
#endif
  return pool_rover->last_begin;
}

/* one day this may relocate blocks to the smallest hole they fit in;
   this will involve keeping a list of holes, probably sorted by size */
unsigned char *
pool_resize(unsigned char *s)
{
  if (pool_rover->last_begin == s)
    pool_rover->avail = pool_rover->last_begin 
      + strlen((char *)pool_rover->last_begin) + 1;
  return s;
}

void
pool_free (unsigned char *s)
{
  /* add the location to the list-of-holes */
}

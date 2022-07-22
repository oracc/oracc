#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "npool.h"

#undef POOL_DEBUG

static struct pool_block *new_block(struct npool *p);

struct npool *
npool_init()
{
  struct npool *p = calloc(1,sizeof(struct npool));
  if (!p)
    {
      fputs("out of core\n",stderr);
      exit(2);
    }
  p->base = p->rover = new_block(p);
  return p;
}

void
npool_term(struct npool *p)
{
  struct pool_block *pbp,*tmp;
  if (!p)
    return;
  for (pbp = p->base; pbp; pbp = tmp)
    {
      tmp = pbp->next;
      free(pbp->mem);
      free(pbp);
    }
  free(p);
}

static struct pool_block *
new_block(struct npool *p)
{
  struct pool_block *new;
  new = malloc(sizeof(struct pool_block));
  if (!new)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->used = new->mem = calloc(1,POOL_BLOCK_SIZE);
  if (!new->mem)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->top = new->mem + POOL_BLOCK_SIZE;
  new->next = NULL;
  if (p->rover)
    p->rover->next = new;
  return new;
}

static struct pool_block *
new_block2(struct npool *p, size_t len)
{
  struct pool_block *new;
  new = malloc(sizeof(struct pool_block));
  if (!new)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->used = new->mem = malloc(len);
  if (!new->mem)
    {
      (void)fputs("out of core",stderr);
      exit(2);
    }
  new->top = new->mem + POOL_BLOCK_SIZE;
  new->next = NULL;
  if (p->rover)
    p->rover->next = new;
  return new;
}

unsigned char *
npool_copy(register const unsigned char *s, struct npool *p)
{
  size_t len;
  
  if (!s)
    return NULL;
  len = strlen((char *)s) + 1;
  if (len >= POOL_BLOCK_SIZE)
    {
      fprintf(stderr, "len = %ld\n", len);
      p->rover = new_block2(p,len);
    }
  else {
    if ((p->rover->used+len) >= p->rover->top)
      p->rover = new_block(p);
  }
  p->rover->last_begin = memcpy(p->rover->used, s, len);
  p->rover->used += len;
  return p->rover->last_begin;
}

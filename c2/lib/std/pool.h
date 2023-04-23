#ifndef POOL_H_
#define POOL_H_ 1
#define POOL_BLOCK_SIZE        (8192*8)
#define POOL_OWN_BLOCK_CUTOFF 	128

#include <stdlib.h>

struct pool
{
  struct pool_block *rover;
  struct pool_block *base;
};

typedef struct pool Pool;

struct pool_block
{
  unsigned char *mem;
  unsigned char *top;
  unsigned char *last_begin;
  unsigned char *used;
  struct pool_block *next;
};

extern Pool* pool_init(void);
extern void pool_reset(Pool*);
extern void pool_term(Pool*);
extern unsigned char *pool_alloc(size_t len, Pool *p);
extern unsigned char *pool_copy(const unsigned char *s, Pool*p);

#endif

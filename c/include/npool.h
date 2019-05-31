#ifndef NPOOL_H_
#define NPOOL_H_ 1
#define POOL_BLOCK_SIZE        (8192*8)
#define POOL_OWN_BLOCK_CUTOFF 	128

struct npool
{
  struct pool_block *rover;
  struct pool_block *base;
};

struct pool_block
{
  unsigned char *mem;
  unsigned char *top;
  unsigned char *last_begin;
  unsigned char *used;
  struct pool_block *next;
};

extern struct npool* npool_init(void);
extern void npool_term(struct npool*);
extern unsigned char *npool_copy(const unsigned char *s, struct npool*p);
#endif

#ifndef POOL_H_
#define POOL_H_ 1
#define POOL_BLOCK_SIZE        (8192*8)
#define POOL_OWN_BLOCK_CUTOFF 	128

#define pool_copy	c1_pool_copy
#define pool_free	c1_pool_free
#define pool_init	c1_pool_init
#define pool_resize	c1_pool_resize
#define pool_term	c1_pool_term

extern void pool_init(void);
extern void pool_term(void);
extern unsigned char *pool_copy(const unsigned char *s);
extern unsigned char *pool_resize(unsigned char *s);
extern void pool_free(unsigned char *s);
#endif

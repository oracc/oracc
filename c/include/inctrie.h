#ifndef _INCTRIE_H
#define _INCTRIE_H

struct inctrie { struct inctrie **cells; const void *val; short level; };

extern struct inctrie *inctrie_create(void);
extern void inctrie_free(struct inctrie *ip);
extern struct inctrie *inctrie_insert(struct inctrie *ip, const char *key, 
				      const void *val, short level);
extern const void *inctrie_lookup(struct inctrie *ip, const char *key, int *length);
extern const unsigned char *inctrie_map(struct inctrie *ip, const char *str,
					const char *end, int append,
					void(*er)(const char*,const char*,
						  void *,
						  const char *,size_t),
					void *u,
					const char *f, size_t l);
#endif /*_INCTRIE_H*/

#ifndef MEMO_H_
#define MEMO_H_ 1

#include <stdlib.h>
#include <mesg.h>

struct memo
{
  struct memo_block *mem_rover;
  struct memo_block *mem_base;
  int last_element_used;
  int elements_per_alloc;
  int element_size;
};

typedef struct memo Memo;

struct memo_str
{
  const unsigned char *s;
  void *user;
  Mloc m;
};
typedef struct memo_str Memo_str;

extern Memo * memo_init (int element_size, int elements_per_alloc);
extern void memo_term(Memo *any);
extern void *memo_merge(Memo *any, size_t *bytes);
extern void *memo_new (Memo *any);
extern void *memo_new_array (Memo *any, int count);
extern void memo_reset (Memo *any);
extern Memo_str* memo_str(Mloc *m, const unsigned char *s);
#endif

#ifndef RUN_H_
#define RUN_H_

#include "hash.h"
#include "npool.h"

struct run_context
{
  Hash_table *known_projects;
  struct npool *pool;
  struct proj_context *proj;
  List *langs;
};

struct run_context *run_init(void);
void run_term(struct run_context *);

#endif/*RUN_H_*/

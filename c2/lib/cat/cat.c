#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "cat.h"
#include "cat.tab.h"

Pool *catpool;
Memo *catchunk_mem;

int cat_initted = 0;
int cattrace = 1;

void
cat_init(void)
{
  if (cat_initted++)
    return;
  catchunk_mem = memo_init(sizeof(struct catchunk),1024);
  catpool = pool_init();
  cat_reset_head();
}

void
cat_term(void)
{
  if (cat_initted)
    {
      memo_term(catchunk_mem);
      pool_term(catpool);
      catchunk_mem = NULL;
      catpool = NULL;
      cat_initted = 0;
    }
}

void
cat_dump(struct catchunk *ccp)
{
  struct catchunk *cp;
  for (cp = ccp; cp; cp = cp->next)
    fprintf(stderr, "[%d]\t%s\n", cp->line, cp->text);
}

struct catchunk *
cat_read(const char *file)
{
  cat_init();
  return catyacc();
}


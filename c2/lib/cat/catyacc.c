#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include "cat.h"

extern Memo *catchunk_mem;
extern Pool *catpool;
struct catchunk *cat_head = NULL;
static struct catchunk *tail = NULL;
extern char *catfile, *currcatfile;

struct catchunk *
catyacc(void)
{
  currcatfile = catfile;
  catparse();
  return cat_head;
}

void
cat_chunk(const char *f, int l, char *t)
{
  struct catchunk *c = memo_new(catchunk_mem);
  if (!cat_head)
    cat_head = tail = c;
  else
    {
      tail->next = c;
      tail = c;
    }
  tail->file = f;
  tail->line = l;
  tail->text = t;
}

void
cat_cont(int l, char *t)
{
  fprintf(stderr, "cat_cont: tail=%s; t=%s\n", tail->text, t);
  while (isspace(t[1]))
    ++t;
  *t = ' ';
  if (tail)
    {
      unsigned char *newtext = pool_alloc(strlen(tail->text) + strlen(t) + 1, catpool);
      sprintf((char*)newtext, "%s%s", tail->text, t);
      fprintf(stderr, "cat_cont: newtext=%s\n", newtext);
      tail->text = (char*)newtext;
      tail->last = l;
    }
}

void
cat_reset_head(void)
{
  cat_head = tail = NULL;
}

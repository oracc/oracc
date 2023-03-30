#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include "atf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <memo.h>
#include <pool.h>
#include "cat.h"

extern char *atffile, *curratffile;

struct catchunk *
atfyacc(void)
{
  curratffile = atffile;
  atfparse();
  /*fprintf(stderr, "catchunk *=%lu\n", sizeof(struct catchunk *));*/
  return cat_head;
}

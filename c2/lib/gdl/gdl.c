#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <prop.h>
#include <memo.h>
#include <tree.h>

#include "gdl.h"
#include "gdl.tab.h"

Pool *gdlpool;

int gdl_lexfld[128];

#if 0
Memo *gdlchunk_mem;
Memo *gdlnode_mem;
#endif

/* set to 1 when a vq has !, e.g., a!(BA) */
int gdl_corrq = 0;

int gdl_initted = 0;
int gdltrace = 0;

void
gdl_init(void)
{
  if (gdl_initted++)
    return;

  gdl_lexfld['@'] = LF_AT;
  gdl_lexfld['='] = LF_EQUALS;
  gdl_lexfld['#'] = LF_HASH;
  gdl_lexfld['"'] = LF_QUOTE;
  gdl_lexfld['~'] = LF_TILDE;
  gdl_lexfld['|'] = LF_VBAR;

  gdlpool = pool_init();
#if 0
  gdlchunk_mem = memo_init(sizeof(struct gdlchunk),1024);
  gdlnode_mem = memo_init(sizeof(struct gdlnode),1024);
#endif
}

void
gdl_term(void)
{
  if (gdl_initted)
    {
#if 0
      memo_term(gdlchunk_mem);
      gdlchunk_mem = NULL;
#endif
      pool_term(gdlpool);
      gdlpool = NULL;
      gdl_initted = 0;
    }
}

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

/* set to 1 when a vq has !, e.g., a!(BA) */
int gdl_corrq = 0;

int gdl_initted = 0;
int gdltrace = 0;

void
gdl_init(void)
{
  if (gdl_initted++)
    return;

  gdl_lexfld_init();
  
  gdlpool = pool_init();
}

void
gdl_term(void)
{
  if (gdl_initted)
    {
      pool_term(gdlpool);
      gdlpool = NULL;
      gdl_initted = 0;
    }
}

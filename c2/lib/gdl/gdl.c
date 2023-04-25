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

/* These only get set when reading ATF but they are used in GDL
   messages so they are declared here which means that programs using
   GDL but not ATF don't have to pull in the whole ATF library */
char *curr_pqx = NULL;
int curr_pqx_line = 0;

/* set to 1 when a vq has !, e.g., a!(BA) */
int gdl_corrq = 0;

int gdl_initted = 0;
int gdltrace = 0;

/* If this is non-zero we use orig graphemes in reconstituting
   compounds rather than c10e */
int gdl_orig_mode = 0;

void
gdl_init(void)
{
  if (gdl_initted++)
    return;

  gdl_balance_init();

  gdl_lexfld_init();

  gdl_modq_init();
  
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
      gdl_balance_term();
      gdl_modq_term();
    }
}

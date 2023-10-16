#include <pool.h>
#include "asl.h"

Pool *aslpool;

void
asl_init(void)
{
  aslpool = pool_init();
}

void
asl_term(void)
{
  pool_term(aslpool);
}

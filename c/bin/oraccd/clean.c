#include <stdio.h>
#include <unistd.h>
#include "oraccd.h"

void
clean(struct oraccd_config *cfg)
{
  if (ORACCD_STALE_LOCK == status_build(0, cfg))
    {
      printf("oraccd: removing lockfile\n");
      unlink("/tmp/oraccd-build.lock");
    }
  if (ORACCD_STALE_LOCK == status_serve(0, cfg))
    {
      printf("oraccd: removing lockfile\n");
      unlink("/tmp/oraccd-serve.lock");
    }
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ox.h"
#include "run.h"
#include "proj_context.h"

void
proj_default(struct run_context *r)
{
  proj_init(r, "oracc");
}

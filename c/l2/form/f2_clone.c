#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include <psdtypes.h>
#include "warning.h"
#include "memblock.h"
#include "npool.h"
#include "lang.h"
#include "f2.h"

struct f2 *
f2_clone(struct f2 *from, struct f2 *clone)
{
  *clone = *f2;
  if (f2->nfinds)
    {
      clone->finds = malloc(f2->nfinds * sizeof(struct sig*));
      memcpy(f2->finds, clone->finds);
    }
  return clone;
}

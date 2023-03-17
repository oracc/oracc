#include <stdlib.h>
#include <stdio.h>
#include "xml.h"

Xmlhelper *
xmlh_init(FILE *fp)
{
  Xmlhelper *xhp = calloc(1, sizeof(Xmlhelper));
  xhp->fp = fp;
  return xhp;
}

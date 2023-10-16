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

void
xml_attr(const char **atts, FILE *fp)
{
  const char **ap;
  for (ap = atts; ap[0]; ap += 2)
    fprintf(fp, " %s=\"%s\"", ap[0], xmlify((uccp)ap[1]));
}

#include <stdlib.h>
#include <tree.h>
#include "asl.h"
#include "asl.tab.h"

Tree *
aslyacc(void)
{
  Tree *tp = NULL;
  curraslfile = aslfile;
  aslparse();
  return tp;
}

int
asl_at_check(const char *atp)
{
  return SIGN;
}

int
asl_grapheme(const char *gp)
{
  return GNAME;
}

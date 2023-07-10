#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tree.h>
#include "asl.h"
#include "signlist.h"
#include "asl.tab.h"

struct sl_signlist *curr_sl = NULL;

Tree *
aslyacc(void)
{
  Tree *tp = NULL;
  curraslfile = aslfile = "<stdin>";
  curr_sl = asl_bld_init();
  aslparse();
  /*asl_bld_term(sl);*/ /* This needs to be called after the signlist is done with */
  return tp;
}

int
asl_grapheme(const char *gp)
{
  const char *gp_orig = gp;
  while (*gp)
    if (*gp < 1288 && islower(*gp))
      return GVALUE;
    else if (*gp < 1288 && isupper(*gp))
      return GNAME;
    else
      ++gp;
  if (!strcmp(gp_orig, "15"))
    return GNAME;
  else
    return GVALUE;
}

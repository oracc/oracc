#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tree.h>
#include "asl.h"
#include "signlist.h"
#include "asl.tab.h"

struct sl_signlist *
aslyacc(void)
{
  curraslfile = aslfile = "<stdin>";
  /*curr_asl = asl_bld_init();*/ /* This is done via new @signlist command */
  aslparse();
  /*asl_bld_term(sl);*/ /* This needs to be called after the signlist is done with */
  return curr_asl;
}

int
asl_grapheme(const char *gp)
{
  const char *gp_orig = gp;
  while (*gp)
    if (*gp <= 127 && islower(*gp))
      return GVALUE;
    else if (*gp <= 127 && isupper(*gp))
      return GNAME;
    else
      ++gp;
  if (!strcmp(gp_orig, "15"))
    return GNAME;
  else
    return GVALUE;
}

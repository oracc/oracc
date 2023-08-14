#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tree.h>
#include "asl.h"
#include "signlist.h"
#include "asl.tab.h"

struct sl_signlist *
aslyacc(const char *file)
{
  if (file)
    curraslfile = aslfile = file;
  else
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

/**If t and a are both NULL return n; else if a is NULL initialize to t; else append a to t.
 */
char *
longtext(struct sl_signlist *sl, char *t, char *a)
{
  static char *n;
  if (a)
    {
      char *n2;
      while ('\t' == *a || ' ' == *a)
	++a;
      n2 = (char*)pool_alloc(strlen(t)+strlen(a)+2, sl->p);
      strcpy(n2,n);
      strcat(n2," ");
      strcat(n2, a);
      n = n2;
    }
  else if (t)
    n = (char*)pool_copy((uccp)t, sl->p);
  return n;
}

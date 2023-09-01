#include <stdlib.h>
#include <string.h>
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

/**If t and a are both NULL return n; else if a is NULL initialize to t; else append a to t.
 */
char *
longtext(struct sl_signlist *sl, char *t, char *a)
{
  static char *n;
  char *ret = NULL;
  if (a)
    {
      char *n2;
      while ('\t' == *a || ' ' == *a)
	++a;
      n2 = (char*)pool_alloc(strlen(t)+strlen(a)+3, sl->p);
      strcpy(n2,n);
      strcat(n2,"\n\t");
      strcat(n2, a);
      ret = n = n2;
    }
  else if (t)
    ret = n = (char*)pool_copy((uccp)t, sl->p);
  else
    {
      ret = n;
      n = NULL;
    }
  return ret;
}

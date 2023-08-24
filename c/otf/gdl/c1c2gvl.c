/**c1c2gvl
 *
 * Bridge function for c1 code to use c2 grapheme validation.
 *
 * Instead of calling gvl_validate or the like, all programs should call
 *
 *   mess = c1c2gvl(file,line,grapheme,type)
 *
 * Returned mess is NULL if the grapheme passed validation.
 *
 * t is 0 for default behaviour; 1 for force gdlparse_string; other
 * uses may be added.
 */

#include <stdlib.h>
#include <string.h>
#include "c1c2gvl.h"

const char *
c1c2gvl(const char *f, size_t l, unsigned const char *g, int t)
{
  if (g)
    {
      const char *mess = NULL;
      int gspecial = (NULL == strpbrk((const char *)g, "()|@~"));
      if (!t)
	{
	  mess = gvl_legacy(f, l, g, 0);
	  if (mess && !gspecial)
	    return mess;
	}
      return gvl_legacy(f, l, g, 1);
    }
  else
    return "(no grapheme to check)";
}

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
#include <warning.h>
#include "c1c2gvl.h"

int c1c2_verbose = 0;

const char *
c1c2gvl(const char *f, size_t l, unsigned const char *g, const char *s)
{
  if (g)
    {
      const char *mess = NULL;
      mess = gvl_bridge(f, l, g, s);
      if (mess)
	{
	  return mess;
	}
      else
	{
	  if (c1c2_verbose)
	    vnotice("%s validated OK", g);
	      return NULL;
	}
    }
  else
    return "(no grapheme to check)";
}

#include <ctype128.h>
#include "gx.h"

int
parse_dcf(struct entry *e, unsigned char *s)
{
  if (*s)
    {
      /* parse DCF and add it as key/value */
      /* hash_add(e->dcfs, dcf, val) */
      if (*s == '=')
	{
	  e->lang = (ucp)"sux"; /* HACK: look lang code up, e.g., @EG => sux */
	}
    }
  return 0;
}

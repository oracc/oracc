#include <ctype128.h>
#include "gx.h"

void
stem_init(struct entry *e, unsigned char *stem)
{

#if 0
  if (!e->b_allow)
    e->b_allow = hash_create(1024);
#endif

  if (!e->stems)
    e->stems = list_create(LIST_SINGLE);
  list_add(e->stems, stem);

#if 0
  hash_add(e->b_allow, lhs, rhs);
#endif

}

void
parse_stems(struct entry *e, unsigned char *s, locator *lp)
{
}

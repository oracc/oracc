#include <ctype128.h>
#include "gx.h"

void
parse_allow(struct entry *e, unsigned char *s, locator *lp)
{
  unsigned char *equals = NULL;
  static int one = 1;
  
  if (!e->b_allow)
    e->b_allow = hash_create(1);
  equals = (ucp)strstr((ccp)s, " = ");
  if (equals)
    {
      unsigned char *tmp = equals;
      Hash_table *h_a, *h_b;
      while (isspace(tmp[-1]))
	--tmp;
      *tmp = '\0';
      equals += 3;
      while (isspace(*equals))
	++equals;
      while (isspace(*s))
	++s;
      h_a = hash_find(e->b_allow, s);
      h_b = hash_find(e->b_allow, equals);
      if (!h_a)
	{
	  h_a = hash_create(1);
	  hash_add(h_a, equals, &one);
	  hash_add(e->b_allow, s, h_a);
	}
      if (!h_b)
	{
	  h_b = hash_create(1);
	  hash_add(h_b, s, &one);
	  hash_add(e->b_allow, equals, h_b);
	}
    }
  else
    warning("syntax error in @allow: must be VALUE₁ = VALUE₂");
}

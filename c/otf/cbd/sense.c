#include <ctype128.h>
#include "gx.h"

struct sense *
init_sense(void)
{
  return calloc(1, sizeof(struct sense));
}

/* On entry s is pointing after the @sense and any spaces; for @sense! this will need adjusting */
void
parse_sense(struct entry *e, unsigned char *s)
{
  struct sense *sp = init_sense();
  if (!e->senses)
    e->senses = list_create(LIST_SINGLE);
  list_add(e->senses, sp);
  if ('!' == *s)
    {
      sp->bang = 1;
      ++s;
    }
  while (isspace(*s))
    ++s;
  if (isdigit(*s) && s[1] && '.' == s[1])
    {
      sp->num = s;
      while (!isspace(*s))
	++s;
      if (*s)
	*s++ = '\0';
      while (isspace(*s))
	++s;
    }
  if ('[' == *s)
    {
      ++s;
      if (*s)
	{
	  sp->sgw = s;
	  while (*s && ']' != *s)
	    ++s;
	  if (*s)
	    {
	      *s++ = '\0';
	      while (isspace(*s))
		++s;
	    }
	  else
	    {
	      warning("malformed @sense: SENSE GW missing ']'");
	      return;
	    }
	}
      else
	{
	  warning("malformed @sense: SENSE GW ends with '['");
	  return;
	}
    }
  sp->pos = s;
  while (*s && !isspace(*s))
    ++s;
  if (*s)
    {
      *s++ = '\0';
      while (isspace(*s))
	++s;
    }
  else
    {
      warning("malformed @sense: ends with POS and has no meaning");
      return;
    }
  sp->mng = s;
  s += strlen((ccp)s);
  while (isspace(s[-1]))
    --s;
  if (*s)
    *s = '\0';
}

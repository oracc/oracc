#include <ctype128.h>
#include "gx.h"

static void parse_one_base(struct entry *e, unsigned char *s);

void
parse_bases(struct entry *e, unsigned char *s)
{
  unsigned char *t = NULL;
  e->b_pri = hash_create(1);
  e->b_alt = hash_create(1);
  e->b_sig = hash_create(1);

  untab(s);

  t = s+strlen((ccp)s);
  while (isspace(t[-1]))
    --t;
  if (';' == *t)
    warning("redundant semi-colon at end of @bases line--please remove it.");

  /* split on '; ' */
  while ((t = (ucp)strstr((ccp)s, "; ")))
    {
      *t = '\0';
      parse_one_base(e,s);
      s = t+1;
      while (isspace(*s))
	++s;
    }
  /* s should always be pointing to the only or last base */
  if (*s)
    parse_one_base(e,s);  
}

static void
parse_one_base(struct entry *e, unsigned char *s)
{
  unsigned char *pri, *alts, *t;
  pri = s;
  while (*s && !isspace(*s))
    ++s;

  if (*s)
    *s++ = '\0';
  if (verbose)
    fprintf(stderr, "registering pri %s\n", pri);
  hash_add(e->b_pri, pri, "1");

  if (*s)
    {
      while (isspace(*s))
	++s;
      if ('(' == *s)
	{
	  ++s;
	  while (isspace(*s))
	    ++s;
	  alts = s;
	  while ((t = (ucp)strstr((ccp)s, ", ")))
	    {
	      *t = '\0';
	      hash_add(e->b_alt, s, pri);
	      s = t+1;
	      while (isspace(*s))
		++s;
	    }
	  if (*s)
	    {
	      for (t = s; *t; ++t)
		if (')' == *t)
		  break;
	      if (*t)
		*t = '\0';
	      else
		vwarning("missing ')' from end of alts belonging to pri %s", pri);
	      if (verbose)
		fprintf(stderr, "adding alt %s to pri %s\n", s, pri);
	      hash_add(e->b_alt, s, pri);
	    }
	}
      else
	{
	  vwarning("syntax error in @bases: junk after primary %s", pri);
	} 
    }
}

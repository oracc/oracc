#include <ctype128.h>
#include "gx.h"

static void parse_one_base(struct entry *e, unsigned char *s);
static void process_alt(struct entry *e, unsigned char *pri, unsigned char *pri_sig, unsigned char *alt);

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
  unsigned char *pri, *alts, *t, *sig, *pri_sig;
  pri = s;
  while (*s && !isspace(*s))
    ++s;

  if (*s)
    *s++ = '\0';
  if (verbose)
    fprintf(stderr, "registering pri %s\n", pri);
  if ((sig = gdl_sig(pri,1,1)))
    {
      unsigned char *known_sig = NULL;
      if ((known_sig = hash_find(e->b_sig, sig)))
	{
	  vwarning("duplicate or equivalent primary base %s ~~ %s", pri, known_sig);
	}
      else
	{
	  pri_sig = npool_copy(sig, e->owner->pool);
	  hash_add(e->b_pri, pri, pri_sig);
	  hash_add(e->b_sig, pri_sig, pri);
	}
      free(sig);
    }
  else
    vwarning("gdl_sig failed on %s", pri);
  

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
	      process_alt(e, pri, pri_sig, s);
	      s = t+1;
	      while (isspace(*s))
		++s;
	    }
	  if (*s)
	    {
	      for (t = s+strlen((ccp)s); t > s; --t)
		if (!isspace(t[-1]))
		  break;
	      if (')' == t[-1])
		{
		  --t;
		  *t = '\0';
		}
	      else
		vwarning("missing ')' from end of alts belonging to pri %s", pri);
	      process_alt(e, pri, pri_sig, s);
	    }
	}
      else
	{
	  vwarning("syntax error in @bases: junk after primary %s", pri);
	} 
    }
}

static void
process_alt(struct entry *e, unsigned char *pri, unsigned char *pri_sig, unsigned char *alt)
{
  unsigned char *alt_sig = NULL;
  if (verbose)
    fprintf(stderr, "adding alt %s to pri %s\n", alt, pri);
  alt_sig = gdl_sig(alt,1,1);
  if (strcmp((ccp)pri_sig,(ccp)alt_sig))
    vwarning("alt %s is not equivalent to primary %s (%s != %s)", alt, pri, alt_sig, pri_sig);
  else
    hash_add(e->b_alt, alt, pri);
}

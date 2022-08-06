#include <ctype128.h>
#include "gx.h"

static List *curr_base_list = NULL;

/* At parse time we just save the bases in a list of lists */
void
bases_pri_save(YYLTYPE l, struct entry *e, unsigned char *p)
{
  if (!e->bases)
    e->bases = list_create(LIST_SINGLE);
  list_add(e->bases, (curr_base_list = list_create(LIST_SINGLE)));
  list_add(curr_base_list, loctok(&l,e,p));
}

void
bases_alt_save(YYLTYPE l, struct entry *e, unsigned char *a)
{
  if (curr_base_list)
    list_add(curr_base_list, loctok(&l,e,a));
}

static void parse_one_base(struct entry *e, unsigned char *s);
static void process_alt(struct entry *e, unsigned char *pri, unsigned char *pri_sig, unsigned char *alt);

void
parse_bases(struct entry *e, unsigned char *s, locator *lp)
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

static int
allowed(struct entry *e, unsigned char *a, unsigned char *b)
{
  if (e && e->b_allow)
    {
      Hash_table *allowed = hash_find(e->b_allow, a);
      if (allowed && hash_find(allowed, b))
	return 1;
    }
  return 0;
}

static void
parse_one_base(struct entry *e, unsigned char *s)
{
  unsigned char *pri, *t, *sig, *pri_sig = NULL;
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
      if ((known_sig = hash_find(e->b_sig, sig)) && !allowed(e, pri, known_sig))
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

#include <ctype128.h>
#include "gx.h"

extern int sigs;

struct sense *
sense_init(YYLTYPE l, struct entry *e)
{
  struct sense *sp = mb_new(e->owner->sensesmem);
  sp->l = l;
  list_add(e->senses, sp);
  return sp;
}

#if 0

struct sense *
init_sense(void)
{
  return calloc(1, sizeof(struct sense));
}

/* On entry s is pointing after the @sense and any spaces; for @sense! this will need adjusting */
void
parse_sense(struct entry *e, unsigned char *s, locator *lp)
{
  struct sense *sp = parse_sense_sub(s,lp);

  if (sp && e)
    {
      if (!e->senses)
	e->senses = list_create(LIST_SINGLE);
      list_add(e->senses, sp);
      if (sigs)
	{
	  struct f2 *f2p = NULL;
	  for (f2p = list_first(e->forms); f2p; f2p = list_next(e->forms))
	    {
	      unsigned char *sig = NULL;
	      f2p->epos = sp->pos;
	      f2p->sense = sp->mng;
	      sig = form_sig(e,f2p);  
	      puts((ccp)sig);
	    }
	}
    }

}

struct sense *
parse_sense_sub(unsigned char *s, locator*lp)
{
  struct sense *sp = init_sense();
  sp->l.file = lp->file;
  sp->l.line = lp->line;
  
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
	      vwarning2(lp->file,lp->line,"malformed @sense: SENSE GW missing ']'");
	      return NULL;
	    }
	}
      else
	{
	  vwarning2(lp->file,lp->line,"malformed @sense: SENSE GW ends with '['");
	  return NULL;
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
      vwarning2(lp->file,lp->line,"malformed @sense: ends with POS and has no meaning");
      return NULL;
    }
  sp->mng = s;
  s += strlen((ccp)s);
  while (isspace(s[-1]))
    --s;
  if (*s)
    *s = '\0';
  return sp;
}
#endif

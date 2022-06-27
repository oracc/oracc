#include <ctype128.h>
#include "gx.h"
#include "f2.h"

void
parse_form(struct entry *e, unsigned char *lp)
{
  struct f2 *f2p = calloc(1, sizeof(struct f2));
  unsigned char *sig = NULL;

  list_add(e->forms, f2p);

  /* on entry lp is pointing to the orthographic form */
  f2p->form = lp;
  while (*lp && !isspace(*lp))
    ++lp;
  if (*lp)
    *lp++ = '\0';
  while (isspace(*lp))
    ++lp;

  while (*lp)
    {
      switch (*lp)
	{
	case '@':
	  f2p->rws = ++lp;
	  break;
	case '%':
	  f2p->lang = ++lp;
	  break;
	case ':':
	  f2p->form = ++lp;
	  break;
	case '$':	  
	  f2p->norm = ++lp;
	  break;
	case '/':
	  f2p->base = ++lp;
	  break;
	case '+':
	  if (lp[1] == '-')
	    f2p->cont = ++lp;
	  else if (lp[1] == '.')
	    f2p->augment = ++lp;
	  else
	    warning("'+' in signature should be followed by '-' or '.'");
	  ++lp;
	  break;
	case '#':
	  if (lp[1] == '#')
	    {
	      ++lp;
	      f2p->morph2 = ++lp;
	    }
	  else
	    f2p->morph = ++lp;
	  break;
	case '*':
	  f2p->stem = ++lp;
	  break;
#if 0
	  /* can't occur in glo atm, but what about a-a\a moving \a to FORM in future?? */
	case '\\':
	  f2p->disambig = ++lp;
	  while (isalnum(*lp) || '\\' == *lp)
	    ++lp;
	  break;
#endif
	default:
	  vwarning("parse error at '%c'", *lp);
	  return;
	}
      while (*lp && !isspace(*lp))
	++lp;
      if (*lp)
	{
	  *lp = '\0';
	  while (isspace(*lp))
	    ++lp;
	}	  
    }

  f2p->project = e->owner->project;
  if (!f2p->lang)
    f2p->lang = e->lang;
  f2p->core = langcore_of((ccp)f2p->lang);
  f2p->cf = e->cf;
  f2p->gw = e->cf;
  f2p->pos = e->cf;
  sig = form_sig(e,f2p);  
  if (verbose)
    fprintf(stderr, "%s\n", sig);

}

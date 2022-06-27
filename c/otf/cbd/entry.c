#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "gx.h"

static struct entry *
init_entry(void)
{
  return calloc(1,sizeof(struct entry));
}
static void
term_entry(struct entry *e)
{
}

unsigned char **
parse_entry(struct cbd *c, unsigned char **ll)
{
  unsigned char *s = NULL, end;
  struct entry *e = NULL;
  int plus = 0;
  if (ll[0][0] == '+')
    {
      plus = 1;
      s = &ll[0][1];
    }
  else
    s = ll[0];
  if (!strncmp((ccp)s, "@entry", strlen("@entry")))
    {
      e = init_entry();
      list_add(c->entries, e);
      parse_cgp(e, s);
      /* fprintf(stderr, "@entry %s[%s]%s\n", cf,gw,pos); */
      ++ll;
      ++lnum;
    }
  else
    {
      vwarning("expected @entry but got %s",ll[0]);
      return ll;
    }
  while (*ll)
    {
      if (ll[0][0])
	{
	  if (ll[0][0] == '@')
	    {
	      if (!strncmp((ccp)ll[0], "@end", strlen("@end")))
		{
		  s = ll[0] + strlen("@end");
		  s = tok(s, &end);
		  if (s && !strcmp((ccp)s,"entry"))
		    {
		      /* finish entry code goes here */
		      term_entry(e);
		      ++ll;
		      ++lnum;
		      return ll;
		    }
		  else
		    {
		      warning("bad @end entry line");
		      ++ll;
		      ++lnum;
		      break;
		    }
		}
	      else
		{
		  unsigned char *tag = &ll[0][1], *es, save = '\0';
		  struct cbdtag *p = NULL;
		  for (es = tag+1; *es && !isspace(*es); ++es)
		    ;
		  if (*es)
		    {
		      save = *es;
		      *es++ = '\0';
		      while (isspace(*es))
			++e;
		    }
		  if ((p = cbdtags((ccp)tag, strlen((ccp)tag))))
		    {
		      /* fprintf(stderr, "found %s with parser %p\n", tag, (void*)p->parser); */
		      (p->parser)(e,es);
		    }
		  else
		    {
		      vwarning("unknown tag %s", tag);
		      ++ll;
		      ++lnum;
		      break;
		    }
		  ++ll;
		  ++lnum;
 		}
	    }
	  else if (ll[0][0] == '>')
	    {
	      /* edit tag processing goes here */
	      ++ll;
	      ++lnum;
	    }
	  else
	    {
	      warning("bad character at start of line in entry");
	      ++ll;
	      ++lnum;
	      break;
	    }
	}
      else
	{
	  warning("glossary ended in mid-entry");
	  break;
	}
    }
  return ll;
}

void
parse_cgp(struct entry *e, unsigned char *s)
{
  unsigned char *t = NULL;
  while (!isspace(*s))
    ++s;
  while (isspace(*s))
    ++s;
  e->cf = s;
  t = s + strlen((ccp)s);
  while (t > s && isspace(t[-1]))
    --t;
  *t = '\0';
  while (t > s && !isspace(t[-1]))
    --t;
  e->pos = t;
  while (t > s && isspace(t[-1]))
    --t;
  if (']' == t[-1])
    {
      --t;
      *t = '\0';
      while (t > s && '[' != *t)
	--t;
      if (t == s)
	warning("syntax error in @entry: missing [ at start of GW");
      else
	{
	  *t = '\0';
	  e->gw = t+1;
	  if (strchr((ccp)s,'[') || strchr((ccp)s,']'))
	    warning("syntax error in @entry: too many [ or ]");
	  while (isspace(t[-1]))
	    --t;
	  *t = '\0';
	}
    }
  else
    warning("syntax error in @entry: expected ']' to end GW");
}

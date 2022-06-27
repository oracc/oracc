#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "gx.h"

extern struct header *hdr;

const unsigned char *cf, *gw, *pos;

void parse_entry(unsigned char *s);

unsigned char **
entry(unsigned char **ll)
{
  unsigned char *s = NULL, end;
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
      parse_entry(s);
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
		      cf = gw = pos = NULL;
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
		  unsigned char *tag = &ll[0][1], *e, save = '\0';
		  struct cbdtag *p = NULL;
		  for (e = tag+1; *e && !isspace(*e); ++e)
		    ;
		  if (*e)
		    {
		      save = *e;
		      *e++ = '\0';
		      while (isspace(*e))
			++e;
		    }
		  if ((p = cbdtags((ccp)tag, strlen((ccp)tag))))
		    {
		      /* fprintf(stderr, "found %s with parser %p\n", tag, (void*)p->parser); */
		      (p->parser)(e);
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
parse_entry(unsigned char *s)
{
  unsigned char *t = NULL;
  while (!isspace(*s))
    ++s;
  while (isspace(*s))
    ++s;
  cf = s;
  t = s + strlen((ccp)s);
  while (t > s && isspace(t[-1]))
    --t;
  *t = '\0';
  while (t > s && !isspace(t[-1]))
    --t;
  pos = t;
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
	  gw = t+1;
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

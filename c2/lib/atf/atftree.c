#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "atf.h"

char *
atf_name(struct catchunk *cp, char **data)
{
  static int intrans = 0;
  char *n = NULL, *s = NULL;
  
  if (cp)
    {
      switch (*cp->text)
	{
	case '&':
	  *data = cp->text + 1;
	  intrans = 0;
	  return "amp";
	  break;
	case '@':
	  if (!intrans)
	    intrans = !strncmp(cp->text+1, "translation", strlen("translation"));
	  return cat_name(cp, data);
	  break;
	case '#':
	  n = cp->text;
	  s = ++n;
	  while (*s && !isspace(*s) && ':' != *s)
	    ++s;
	  if (':' == *s && isspace(s[1])) /* ^XXX:<WHITE> is a #-protocol */
	    {
	      *s++ = '\0';
	      while (*s && (' ' == *s || '\t' == *s))
		++s;
	      *data = s;
	      return n;
	    }
	  else
	    {
	      *data = cp->text;
	      return "comment";
	    }
	  break;
	case '$':
	  s = cp->text + 1;
	  while (*s && (' ' == *s || '\t' == *s))
	    ++s;
	  *data = s;
	  return "dollar";
	  break;
	case '>':
	case '<':
	case '+':
	case '|':
	  *data = cp->text; /* leave the link chars in the text for parsing later */
	  return "link";
	  break;
	case '=':
	  n = cp->text + 2;
	  while (isspace(*n))
	    ++n;
	  *data = n;
	  switch (cp->text[1])
	    {
	    case '=':
	      return "equiv";
	    default:
	      fprintf(stderr, "unhandled = start\n");
	      return "mystery";
	    }
	  break;
	default:
	  *data = cp->text;
	  s = cp->text;
	  while (*s && ' ' != *s  && '\t' != *s)
	    ++s;
	  switch (s[-1])
	    {
	    case '.':
	      return "line";
	      break;
	    case ':':
	      return "siglum";
	      break;
	    default:
	      if (intrans)
		{
		  *data = cp->text;
		  return "#trans";
		}
	      else
		{
		  fprintf(stderr, "%s:%d: atf_name: internal error, no '.' or ':' in passed line type\n",
			  curratffile, atflineno);
		  return "mystery";
		}
	      break;
	    }
	  break;
	}
    }
  else
    return NULL;
}

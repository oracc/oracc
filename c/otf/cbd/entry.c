#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "gx.h"

extern struct header *hdr;

unsigned char **
entry(unsigned char **ll)
{
  unsigned char *s = NULL, end;
  if (!strncmp((ccp)ll[0], "@entry", strlen("@entry")))
    {
      fprintf(stderr, "%s\n", (ccp)ll[0]);
      ++ll;
      ++lnum;
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
		  for (e = tag+1; *e && !isspace(*e); ++e)
		    ;
		  if (*e)
		    {
		      save = *e;
		      *e = '\0';
		    }
		  if (cbdtags((ccp)tag, strlen((ccp)tag)))
		    {
		      fprintf(stderr, "found %s\n", tag);
		    }
		  else
		    {
		      vwarning("unknown tag %s", tag);
		    }
		  ++ll;
		  ++lnum;
		  break;
		}
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

#include <string.h>
#include <stdlib.h>
#include <ctype128.h>
#include "gx.h"

extern struct header hdr;

unsigned char **
header(unsigned char **ll)
{
  unsigned char *s = NULL, end = '\0';
  while (1)
    {
      if (!ll[0])
	break;
      if (!strncmp((ccp)ll[0], "@project", strlen("@project")))
	{
	  s = ll[0] + strlen("@project");
	  hdr.project = tok(s,&end);
	  ++ll;
	  ++lnum;
	}
      else if (!strncmp((ccp)ll[0], "@lang", strlen("@lang")))
	{
	  s = ll[0] + strlen("@lang");
	  hdr.lang = tok(s,&end);
	  ++ll;
	  ++lnum;
	}
      else if (!strncmp((ccp)ll[0], "@name", strlen("@name")))
	{
	  s = ll[0] + strlen("@name");
	  hdr.name = tok(s,&end);
	  ++ll;
	  ++lnum;
	}
      else if (!strncmp((ccp)ll[0], "@trans", strlen("@trans")))
	{
	  s = ll[0] + strlen("@trans");
	  hdr.trans = tok(s,&end);
	  ++ll;
	  ++lnum;
	}
      else
	{
	  break;
	}
    }

  if (!hdr.project)
    warning("no @project found in header");
  else if (!hdr.lang)
    warning("no @lang found in header");
  else
    {
      if (!hdr.trans)
	hdr.trans = (ucc)"en";
      if (!hdr.name)
	hdr.name = hdr.name; /* lazy but avoids allocating memory */
    }
  
  return ll;
}

#include <string.h>
#include <stdlib.h>
#include <ctype128.h>
#include "gx.h"

#if 0

extern const char *project;

unsigned char **
parse_header(struct cbd *c, unsigned char **ll)
{
  unsigned char *s = NULL, end = '\0';
  while (1)
    {
      if (!ll[0])
	break;
      if (!strncmp((ccp)ll[0], "@project", strlen("@project")))
	{
	  s = ll[0] + strlen("@project");
	  c->project = tok(s,&end);
	  if (!project)
	    project = (ccp)c->project;
	  ++ll;
	  ++c->l.line;
	}
      else if (!strncmp((ccp)ll[0], "@lang", strlen("@lang")))
	{
	  s = ll[0] + strlen("@lang");
	  c->lang = tok(s,&end);
	  ++ll;
	  ++c->l.line;
	}
      else if (!strncmp((ccp)ll[0], "@name", strlen("@name")))
	{
	  s = ll[0] + strlen("@name");
	  c->name = tok(s,&end);
	  ++ll;
	  ++c->l.line;
	}
      else if (!strncmp((ccp)ll[0], "@trans", strlen("@trans")))
	{
	  s = ll[0] + strlen("@trans");
	  c->trans = tok(s,&end);
	  ++ll;
	  ++c->l.line;
	}
      else
	{
	  break;
	}
    }

  if (!c->project)
    warning("no @project found in header");
  else if (!c->lang)
    warning("no @lang found in header");
  else
    {
      if (!c->trans)
	c->trans = (ucc)"en";
      if (!c->name)
	c->name = c->name; /* lazy but avoids allocating memory */
    }
  
  return ll;
}

#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ccgi-1.1/ccgi.h"

static const char *
cgi_arg(const char *arg, const char *val)
{
  char *c = malloc(strlen(arg)+strlen(val)+2);
  sprintf(c, "%s=%s", arg, val);
  return c;
}

static char const**
mkvec(CGI_varlist *vl)
{
  CGI_value *value;
  const char *name;
  char const** vec;
  int i;

  for (i = 0, name = CGI_first_name(vl); name != 0; name = CGI_next_name(vl))
    ++i;
  
  if (!i)
    i = 1;
    
  vec = malloc((i+3) * sizeof(const char*));
  
  ((char **)vec)[0] = "/usr/bin/perl";
  ((char **)vec)[1] = "/usr/local/oracc/bin/p3-pager.plx";
  if (i)
    {
      for (i = 2, name = CGI_first_name(vl); name != 0; name = CGI_next_name(vl), ++i)
	((char const**)vec)[i] = cgi_arg(name,CGI_lookup(vl,name));
    }
  else
    {
      vec[2] = cgi_arg("project", "epsd2");
      i = 3;
    }

  ((char **)vec)[i] = NULL;
    
  return vec;
}

int
main(int argc, char **argv)
{
  CGI_varlist *vl = CGI_get_post(0, NULL);
  char const**vec = mkvec(vl);
  execv("/usr/bin/perl", vec);
  return 1;
}

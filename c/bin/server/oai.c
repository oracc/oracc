#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "ctype128.h"
#include "resolver.h"
#include "ccgi/ccgi.h"

extern void dump_vl(CGI_varlist *vl);

static int
countv(CGI_varlist *v)
{
  int i = 0;
  const char *name;
  for (name = CGI_first_name(v);
       name != 0;
       name = CGI_next_name(v))
    ++i;
  return i;
}

void
oai(char *ctext)
{
  CGI_varlist *v = NULL;
  int vcount = -1;
  char **va = NULL;
  const char *name;
  int i;

  setenv("QUERY_STRING", query_string, 1);
  v = CGI_get_query(v);
  v = CGI_get_post(v, NULL);
  dump_vl(v);
  vcount = countv(v);
  va = malloc((7+(2*vcount))*sizeof(char*));
  va[0] = "/usr/bin/perl";
  va[1] = "/home/oracc/bin/oai.plx";
  va[2] = "project";
  va[3] = strdup(project);
  va[4] = "referer";
  if (getenv("HTTP_REFERER"))
    va[5] = strdup(getenv("HTTP_REFERER"));
  else
    va[5] = "unknown";
  for (name = CGI_first_name(v), i=6;
       name != 0;
       name = CGI_next_name(v), ++i)
    {
      va[i++] = strdup(name);
      va[i] = strdup(CGI_lookup(v,name));
    }
  va[i] = NULL;

  execv("/usr/bin/perl", va);
  fprintf(stderr, "execv /usr/bin/perl failed\n");  
  exit(1);
}

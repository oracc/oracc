#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype128.h>
#include "ccgi/ccgi.h"

static char *srchindex = NULL;
static int p3trace = 0;

static void
p3srch(CGI_varlist *vl)
{
  const char *d = CGI_lookup(vl, "tmpdir");
  const char *s = CGI_lookup(vl, "srch");
  char *sfn = malloc(strlen(d) + 10);
  FILE *fp = NULL;

  fprintf(stderr, "p3: saving search '%s'\n", s);
  sprintf(sfn, "%s/search.txt", d);
  if ((fp = fopen(sfn, "w")))
    {
      while (isspace(*s))
	++s;
      if (*s == '!')
	{
	  const char *tmp = ++s;
	  while (*s && !isspace(*s))
	    ++s;
	  if (*s)
	    {
	      int slen = (s - tmp);
	      srchindex = malloc(slen + 1);
	      strncpy(srchindex, s, slen);
	      srchindex[slen] = '\0';
	      (void)CGI_add_var(vl, "srchindex", srchindex);
	      while (isspace(*s))
		++s;
	    }
	}
      fputs(s, fp);
      fclose(fp);
    }
  else
    {
      fprintf(stderr, "p3: failed to open %s to write srch command\n", sfn);
    }
}

static char *
p3tempdir(CGI_varlist *vl)
{
  char *d = NULL, *tmpdir;
  tmpdir=getenv("TMPDIR");

  if (!tmpdir)
    {
      fprintf(stderr, "p3: no TMPDIR in environment; using /tmp\n");
      tmpdir = "/tmp";
    }
  d = malloc(strlen(tmpdir)+11);
  if (tmpdir[strlen(tmpdir)-1] == '/') 
    sprintf(d,"%s%s", tmpdir, "p3.XXXXXX");
  else
    sprintf(d,"%s/p3.XXXXXX", tmpdir);
  d = mkdtemp(d);
  (void)CGI_add_var(vl,"tmpdir",d);
  
  return d;
}

static const char *
cgi_arg(const char *arg, const char *val)
{
  char *c = NULL;
  c = malloc(strlen(arg)+strlen(val)+2);
  sprintf(c, "%s=%s", arg, val);
  return c;
}

static char const**
mkvec(CGI_varlist *vl)
{
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
	{
	  if (strcmp(name, "srch"))
	    ((char const**)vec)[i] = cgi_arg(name,CGI_lookup(vl,name));
	  else
	    --i;
	}
    }
  else
    {
      vec[2] = cgi_arg("project", "epsd2");
      i = 3;
    }

  ((char **)vec)[i] = NULL;
    
  return vec;
}

static void
dump_vl(CGI_varlist *vl)
{
  const char *name;
  CGI_value  *value;

  for (name = CGI_first_name(vl); name != 0;
       name = CGI_next_name(vl))
    {
      int i;
      value = CGI_lookup_all(vl, 0);
      for (i = 0; value[i] != 0; i++) 
	fprintf(stderr, "%s [%d] >>%s<<\n", name, i, value[i]);
    }
}

void
p3(const char *project)
{
  CGI_varlist *vl = NULL;
  char const**vec = NULL, *srch;
  extern int ccgi_verbose;

  if (p3trace)
    ccgi_verbose = 1;

  vl = CGI_get_post(0, NULL);
  if (!CGI_lookup(vl, "project"))
    vl = CGI_add_var(vl, "project", project);

  if (p3trace)
    fprintf(stderr, "p3: 1\n");

  if (p3trace)
    dump_vl(vl);

  p3tempdir(vl);

  if (p3trace)
    fprintf(stderr, "p3: 2\n");

  if ((srch = CGI_lookup(vl, "srch")) && *srch && *srch != '0')
    {
      if (p3trace)
	fprintf(stderr, "p3: 2a\n");

      p3srch(vl);

      if (p3trace)
	fprintf(stderr, "p3: 2b\n");
    }

  if (p3trace)
    fprintf(stderr, "p3: 3\n");

  vec = mkvec(vl);

  if (p3trace)
    fprintf(stderr, "p3: 4\n");

  execv("/usr/bin/perl", (char *const *)vec);

  fprintf(stderr, "p3: failed to exec p3-pager.plx\n");
}

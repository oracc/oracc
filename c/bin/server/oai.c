#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "ctype128.h"
#include "resolver.h"
#include "ccgi/ccgi.h"

extern void dump_vl(CGI_varlist *vl);


static void oai_exec(char **va, int len)
{
  char **ea = NULL;
  int i = 0;
  ea = malloc((7+len)*sizeof(char*));
  ea[0] = "/usr/bin/perl";
  ea[1] = "/home/oracc/bin/oai.plx";
  ea[2] = "project";
  ea[3] = project;
  ea[4] = "request";
  ea[5] = "http://oracc.org/oai";
  for (i = 6, j=0; va[j]; ++i, ++j)
    ea[i] = va[j];
  ea[i] = NULL;
  execv("/usr/bin/perl", ea);
  fprintf(stderr, "execv /usr/bin/perl failed\n");  
  exit(1);  
}

static char **
oai_err(const char *err, const char *str)
{
  static char *va[3];
  va[0] = err;
  va[1] = str;
  va[2] = NULL;
  oai_exec(va, 2);
}

static int
is_oai_arg(const char *name)
{
  return name && (!strcmp(name,"from")
		  || !strcmp(name,"identifier")
		  || !strcmp(name,"metadataPrefix")
		  || !strcmp(name,"resumptionToken")
		  || !strcmp(name,"set")
		  || !strcmp(name,"until")
		  || !strcmp(name,"verb"));
}

static int
countv(CGI_varlist *v)
{
  int i = 0;
  const char *name;
  for (name = CGI_first_name(v);
       name != 0;
       name = CGI_next_name(v))
    {
      if (is_oai_arg)
	++i;
      else
	oai_err("badArgument", "illegal argument");
    }
  return i;
}

/* The oai() function validates for illegal or duplicate arguments
   then passes everything on to oai.plx which does further validation
   and handles the OAI verbs */
void
oai(void)
{
  CGI_varlist *v = NULL;
  int vcount = -1;
  char **va = NULL;
  const char *name;
  int i;

  print_hdr_xml();
  
  setenv("QUERY_STRING", query_string, 1);
  v = CGI_get_query(v);
  v = CGI_get_post(v, NULL);
  dump_vl(v);
  vcount = countv(v);
  for (name = CGI_first_name(v), i=6;
       name != 0;
       name = CGI_next_name(v), ++i)
    {
      CGI_value *all = CGI_lookup_all(name,v);
      const char *v = NULL;
      if (all)
	{
	  if (all[0])
	    {
	      if (all[1])
		oai_err("badArgument","duplicate argument");
	      else
		v = all[0];
	    }
	  else
	    v = "";
	}
      else
	continue; /* can't happen */
      va[i++] = strdup(name);
      va[i] = strdup(v);
    }
  va[i] = NULL;
  oai_exec(va, vcount);
}

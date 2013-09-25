#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static char sesh_template[L_tmpnam];

static const char *sesh_path = NULL;

char *
sesh_file(const char *basename)
{
  char *tmp = malloc(strlen(sesh_path) + strlen(basename) + 2);
  (void)sprintf(tmp, "%s/%s", sesh_path, basename);
  return tmp;
}

/* calling this routine is optional for methods--a method which
   returns immediately and does not require tmp files can do without */
void
sesh_init(xmlrpc_env * const envP, xmlrpc_value * const s, int with_tmpdir)
{
  fprintf(stderr, "in sesh_init\n");
  trace();
  if (with_tmpdir)
    {
      char *tmpdir;
      trace();
      tmpdir = (char*)mkdtemp(sesh_template);
      if (tmpdir)
	{
	  char *basename = NULL;
	  fprintf(stderr, "sesh: tmpdir=%s\n", tmpdir);
	  basename = strrchr(tmpdir, '/');
	  sesh_path = tmpdir;
	  if (basename)
	    {
	      trace();
	      fprintf(stderr, "sesh: basename=%s\n", tmpdir);
	      ++basename;
	      xmlrpc_struct_set_value(envP, s, "session", xmlrpc_string_new(envP, basename));
	      xmlrpc_struct_set_value(envP, s, "#tmpdir", xmlrpc_string_new(envP, tmpdir));
	    }
	  else
	    {
	      fprintf(stderr, "oracc-xmlrpc: no / in session name %s\n", tmpdir);
	      /* not good enough */
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr, "oracc-xmlrpc: failed to create tmpdir from template %s\n", sesh_template);
	  perror("oracc-xmlrpc");
	  /* not good enough */
	  exit(1);
	}
    }
}

void
sesh_set_template(const char *template)
{
  strcpy(sesh_template, template);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static char sesh_template[L_tmpnam];

/* calling this routine is optional for methods--a method which
   returns immediately and does not require tmp files can do without */
void
sesh_init(xmlrpc_env * const envP, xmlrpc_value * const s, int with_tmpdir)
{
  if (with_tmpdir)
    {
      char *tmpdir;
      tmpdir = mkdtemp(sesh_template);
      if (tmpdir)
	{
	  char *basename = strrchr(tmpdir, '/');
	  if (basename)
	    {
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

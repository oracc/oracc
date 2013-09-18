#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static char sesh_template[L_tmpnam];

/* calling this routine is optional for methods--a method which
   returns immediately and does not require tmp files can do with out
   it and just retrieve the arg struct from the params for itself. */
char *
sesh_init(xmlrpc_env * const envP, xmlrpc_value * const params, int with_tmpdir)
{
  char *tmpdir;
  xmlrpc_value * const s;
  xmlrpc_array_read_item(envP, paramArrayP, 0, &s);
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
      return basename;
    }
  else
    {
      perror("oracc-server");
      /* not good enough */
      exit(1);
    }
}

void
sesh_set_template(const char *template)
{
  strcpy(session_template, template);
}

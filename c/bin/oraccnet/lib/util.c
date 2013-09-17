#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

char session_template[L_tmpnam];

char *
create_session(xmlrpc_env * const envP, xmlrpc_value * const s)
{
  char *tmpdir = mkdtemp(session_template);
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
      /* This is not good enough: return a useful error message to caller */
      exit(1);
    }
}

void 
dieIfFaultOccurred (xmlrpc_env * const envP)
{
  if (envP->fault_occurred)
    {
      fprintf(stderr, "ERROR: %s (%d)\n",
	      envP->fault_string, envP->fault_code);
    exit(1);
  }
}

void
set_session_template(const char *template)
{
  strcpy(session_template, template);
}

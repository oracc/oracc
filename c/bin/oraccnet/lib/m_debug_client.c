#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int debug_wait = 0;

void
debug_return_handler(xmlrpc_env *const envP, struct client_method_info *cmi, xmlrpc_value *resultP)
{
  struct call_info *cip = callinfo_unpack(envP, resultP);
  dieIfFaultOccurred(envP);
  fprintf(stdout,
	  "clientIP=%s\n"
	  "serverURL=%s\n"
	  "session=%s\n"
	  "user=%s\n"
	  "password=%s\n"
	  "method=%s\n"
	  "project=%s\n"
	  "version=%s\n",
	  cip->clientIP, cip->serverURL, cip->session,
	  cip->user, cip->password, 
	  cip->method, cip->project, cip->version
	  );
  if (cip->methodargs[0])
    {
      int i;
      fprintf(stdout, "Methodargs:\n");
      for (i = 0; cip->methodargs[i]; ++i)
	{
	  fprintf(stdout, "  %s\n", cip->methodargs[i]);
	  if (!strncmp(cip->methodargs[i], "file:", 5))
	    {
	      char *filename = malloc(strlen(cip->methodargs[i]) + 5);
	      sprintf(filename, "%s.new", &cip->methodargs[i][5]);	      
	    }
	}
    }
}

xmlrpc_value *
debug_call(xmlrpc_env *const envP, struct client_method_info *cmi)
{
  xmlrpc_value *s = callinfo_pack(envP, cmi->instance);
  xmlrpc_value *a = xmlrpc_array_new(envP);
  xmlrpc_array_append_item(envP, a, s);
  return xmlrpc_client_call_params(envP, cmi->instance->serverURL, "debug", a);
}

struct client_method_info debug_client_info =
{
  "debug",
  &debug_call,
  debug_return_handler,
  &debug_wait,
  NULL
};

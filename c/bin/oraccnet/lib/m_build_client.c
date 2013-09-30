#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int build_wait = 1;

void
build_return_handler(xmlrpc_env *const envP, struct client_method_info *cmi, xmlrpc_value *s)
{
  if (s)
    {
      char *str = NULL;
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, s, "status", &status);
      if (status)
	{
	  trace();
	  xmlrpc_read_string(envP, status, (const char **)&str);
	  fprintf(stderr, "oracc-client: build initial status: %s\n", str);
	}
      else
	fprintf(stderr, "oracc-client: build: no status set on RPC return\n");
    }
}

xmlrpc_value *
build_call(xmlrpc_env *const envP, struct client_method_info *cmi)
{
  return xmlrpc_client_call(envP, cmi->instance->serverURL, "build", "()", NULL);
}

struct client_method_info build_client_info =
{
  "build",
  &generic_request,
  build_return_handler,
  &build_wait,
  NULL
};

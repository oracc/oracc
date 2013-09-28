#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int status_wait = 0;

void
status_return_handler(xmlrpc_env *const envP, struct client_method_info *cmi, xmlrpc_value *s)
{
  if (s)
    {
      char *str = NULL;
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, s, "status", &status);
      if (status)
	{
	  xmlrpc_read_string(envP, status, (const char **)str);
	  fprintf(stderr, "%s\n", str);
	}
      else
	fprintf(stderr, "none\n");
    }
}

xmlrpc_value *
status_call(xmlrpc_env *const envP, struct client_method_info *cmi)
{
  return xmlrpc_client_call(envP, cmi->instance->serverURL, "status", "()", NULL);
}

struct client_method_info status_client_info =
{
  "status",
  &generic_request,
  status_return_handler,
  &status_wait,
  NULL
};

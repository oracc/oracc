#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int environment_wait = 0;

void
environment_action(xmlrpc_env *const envP, xmlrpc_value *resultP)
{
  int i, max;
  for (i = 0, max = xmlrpc_array_size(envP, (xmlrpc_value * const)resultP); i < max; ++i)
    {
      const char *addr;
      xmlrpc_value * itemP;
      xmlrpc_array_read_item(envP, resultP, i, &itemP);
      xmlrpc_read_string(envP, itemP, (const char ** const)&addr);
      dieIfFaultOccurred(envP);
      printf("oracc-client: environment: %s\n", addr);
    }
}

xmlrpc_value *
environment_call(xmlrpc_env *const envP, const char *serverURL)
{
  return xmlrpc_client_call(envP, serverURL, "environment", "()", NULL);
}

struct client_method_info environment_client_info =
{
  "environment",
  &environment_call,
  environment_action,
  &environment_wait,
  NULL
};

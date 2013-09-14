#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int debug_wait = 0;

void
debug_action(xmlrpc_env *const envP, xmlrpc_value *resultP)
{
  const char *addr;
  xmlrpc_read_string(envP, (xmlrpc_value * const)resultP, (const char ** const)&addr);
  dieIfFaultOccurred(envP);
  printf("oracc-client: debug: REMOTE_ADDRESS=%s.\n", addr);
}

xmlrpc_value *
debug_call(xmlrpc_env *const envP, const char *serverURL)
{
  return xmlrpc_client_call(envP, serverURL, "debug", "()", NULL);
}

struct client_method_info debug_client_info =
{
  "debug",
  &debug_call,
  debug_action,
  &debug_wait,
  NULL
};

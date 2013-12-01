#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int deploy_wait = 10;

void
deploy_return_handler(xmlrpc_env *const envP, struct client_method_info *cmi, xmlrpc_value *s)
{
  if (s)
    {
      char *str = NULL;
      if ((str = result_request_status(envP, s)))
	fprintf(stderr, "oracc-client: deploy: request status: %s\n", str);
      else
	fprintf(stderr, "oracc-client: deploy: no request status set by RPC\n");
      if ((str = result_method_status(envP, s)))
	fprintf(stderr, "oracc-client: deploy: method status: %s\n", str);
      else
	fprintf(stderr, "oracc-client: deploy: no method status set by RPC\n");
      (void)result_method_file(envP, s, "request-log", "server.log");
      (void)result_method_file(envP, s, "method-log", "build.log");
    }
}

xmlrpc_value *
deploy_call(xmlrpc_env *const envP, struct client_method_info *cmi)
{
  return xmlrpc_client_call(envP, cmi->instance->serverURL, "deploy", "()", NULL);
}

struct client_method_info deploy_client_info =
{
  "deploy",
  &generic_request,
  deploy_return_handler,
  &deploy_wait,
  NULL
};

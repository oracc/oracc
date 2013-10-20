#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int build_wait = 30;

void
build_return_handler(xmlrpc_env *const envP, struct client_method_info *cmi, xmlrpc_value *s)
{
  if (s)
    {
      char *str = NULL;
      if ((str = result_request_status(envP, s)))
	fprintf(stderr, "oracc-client: build: request status: %s\n", str);
      else
	fprintf(stderr, "oracc-client: build: no request status set by RPC\n");
      if ((str = result_method_status(envP, s)))
	fprintf(stderr, "oracc-client: build: method status: %s\n", str);
      else
	fprintf(stderr, "oracc-client: build: no method status set by RPC\n");
      (void)result_method_file(envP, s, "method-log", "build.log");
      (void)result_method_file(envP, s, "method-zip", "build.zip");
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

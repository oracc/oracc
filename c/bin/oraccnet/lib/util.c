#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

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

xmlrpc_value *
generic_request(xmlrpc_env *const envP, struct client_method_info *cmi)
{
  xmlrpc_value *s = callinfo_pack(envP, cmi->instance);
  xmlrpc_value *a = xmlrpc_array_new(envP);
  xmlrpc_array_append_item(envP, a, s);
  return xmlrpc_client_call_params(envP, cmi->instance->serverURL, 
				   cmi->instance->method, a);
}

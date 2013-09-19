#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int ox_wait = 0;

void
ox_response_handler(xmlrpc_env *const envP, 
		    struct client_method_info *cmi, 
		    xmlrpc_value *resultP)
{
  struct call_info *cip = callinfo_unpack(envP, resultP);
  dieIfFaultOccurred(envP);
  
  /* This should exit if the server returned an error */
  proc_error_check(envP, cmi, resultP);
  
  /* check for opts which modify the input and save the file, 
     backing up original first */
  
  /* if there were ox-errors, send the error log to stdout */
}

struct client_method_info ox_client_info =
{
  "ox",
  &generic_request,
  ox_response_handler,
  &ox_wait,
  NULL
};

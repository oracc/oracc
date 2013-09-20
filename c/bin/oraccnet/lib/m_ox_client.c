#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int ox_wait = 0;

void
ox_response_handler(xmlrpc_env *const envP, 
		    struct client_method_info *cmi, 
		    xmlrpc_value *s)
{
  struct call_info *cip = NULL;
  xmlrpc_value *status = NULL;
  xmlrpc_value *log = NULL;

  if (s)
    {
      xmlrpc_struct_find_value(envP, s, "status", &status);
      if (status)
	{
	  fprintf(stderr, "found status\n");
	}
      xmlrpc_struct_find_value(envP, s, "log", &log);
      if (log)
	{
	  file_dump(envP, log, "-");
	}
    }

  /*  callinfo_unpack(envP, resultP); */
  /* This should exit if the server returned an error */
  
  
  /* check for opts which modify the input and save the file, 
     backing up original first */
  
  /* if there were ox-errors, send the error log to stdout */
 
  return;
}

struct client_method_info ox_client_info =
{
  "ox",
  &generic_request,
  ox_response_handler,
  &ox_wait,
  NULL
};

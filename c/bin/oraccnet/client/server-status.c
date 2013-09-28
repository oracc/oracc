#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

extern struct client_method_info status_client_info;

xmlrpc_value *
server_status(xmlrpc_env * envP, struct call_info *cip)
{
  xmlrpc_value *resultP = NULL;
  struct client_method_info *cmi = &status_client_info;

  cmi->instance = cip;
  resultP = cmi->call(envP, cmi);
  dieIfFaultOccurred(envP);

  if (resultP)
    {
      char *str = NULL;
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, resultP, "status", &status);
      if (status)
	{
	  trace();
	  xmlrpc_read_string(envP, status, (const char **)&str);
	  fprintf(stderr, "%s\n", str);
	  if (!strcmp(str, "completed"))
	    return resultP;
	}
      else
	{
	  /* should return an error condition to caller here
	     so that when server bombs/loses session client can 
	     terminate gracefully */
	}
    }
  
  /* Dispose of our result value. */
  xmlrpc_DECREF(resultP);
  
  return NULL;
}

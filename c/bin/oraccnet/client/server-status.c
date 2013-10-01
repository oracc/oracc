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
  struct client_method_info *cmi = NULL;
  struct meths_tab *meth = NULL;
  struct call_info *cip_status = callinfo_clone(cip);
  cip_status->method = "status";
  if (!(meth = meths(cip_status->method, strlen(cip_status->method))))
    {
      fprintf(stderr, "oracc-client: unknown method name `%s'\n", cip_status->method);
      exit(1);
    }

  cmi = meth->info;
  meth->info->instance = cip_status;
  resultP = cmi->call(envP, cmi);
  dieIfFaultOccurred(envP);

  if (resultP)
    {
      char *str = NULL;
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, resultP, "method-status", &status);
      if (status)
	{
	  trace();
	  xmlrpc_read_string(envP, status, (const char **)&str);
	  /* This diagnostic needs to be suppressable */
	  fprintf(stderr, "server-status: status=%s\n", str);
	  if (!strcmp(str, "completed"))
	    return resultP;
	}
      else
	{
	  fprintf(stderr, "server-status: no method-status\n");
	  /* should return an error condition to caller here
	     so that when server bombs/loses session client can 
	     terminate gracefully */
	}
    }

  /* Dispose of our result value. */
  xmlrpc_DECREF(resultP);
  
  return NULL;
}

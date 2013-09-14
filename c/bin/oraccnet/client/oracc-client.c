/* A simple synchronous XML-RPC client written in C, as an example of an
   Xmlrpc-c client.  This invokes the sample.add procedure that the Xmlrpc-c
   example xmlrpc_sample_add_server.c server provides.  I.e. it adds two
   numbers together, the hard way.

   This sends the RPC to the server running on the local system ("localhost"),
   HTTP Port 8080.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"
#include "config.h"  /* information about this build environment */

#define NAME "Xmlrpc-c Test Client"
#define VERSION "1.0"

int 
main(int argc, char *argv[0])
{
  xmlrpc_env env;
  const char * const serverURL = "http://oracc.bfos:80/xmlrpc";
  xmlrpc_value *resultP = NULL;
  struct client_method_info *cmi = NULL;
  struct meths_tab *meth = NULL;
  
  if (argc-1 != 1)
    {
      fprintf(stderr, "oracc-client: this program takes only one argument, a method name\n");
      exit(1);
    }
  else
    {
      if (!(meth = meths(argv[1], strlen(argv[1]))))
	{
	  fprintf(stderr, "oracc-client: unknown method name `%s'\n", argv[1]);
	  exit(1);
	}
      else
	cmi = meth->info;
    }
  
  /* Initialize our error-handling environment. */
  xmlrpc_env_init(&env);
  
  /* Start up our XML-RPC client library. */
  xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
  dieIfFaultOccurred(&env);
  
  resultP = cmi->call(&env, serverURL);
  dieIfFaultOccurred(&env);

#if 0
  if (cmi->wait_seconds)
    {
      while (status(&env, cmi))
	{
	  sleep(cmi->wait_seconds);
	}
      resultP = cmi->term(&env, cmi);
    }
#endif

  cmi->action(&env, resultP);

  /* Dispose of our result value. */
  xmlrpc_DECREF(resultP);
  
  /* Clean up our error-handling environment. */
  xmlrpc_env_clean(&env);
  
  /* Shutdown our XML-RPC client library. */
  xmlrpc_client_cleanup();
  
  return 0;
}

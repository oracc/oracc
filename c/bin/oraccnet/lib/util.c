#include <stdio.h>
#include <stdlib.h>
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

#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

static xmlrpc_value *
debug_method(xmlrpc_env *const envP,
	     xmlrpc_value *const paramArrayP, 
	     void *serverInfo,
	     void *callInfo
	     )
{
  const char *addr = getenv("REMOTE_ADDR");
  xmlrpc_value *s;

  fprintf(stderr, "oracc-xmlrpc: debug: REMOTE_ADDR=%s\n", addr);
  s = sesh_init(envP, paramArrayP);
  xmlrpc_struct_set_value(envP, s, "clientIP", xmlrpc_string_new(envP, addr));
  
  return s;
}

struct xmlrpc_method_info3 debug_server_info =
{
  "debug",
  debug_method,
  NULL,
  0,
  "s:",
  "return the value of call_info structure with clientIP added to it from the environment",
};

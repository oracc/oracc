#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

static void
debug_action(xmlrpc_env *const envP, xmlrpc_value *resultP)
{    
  xmlrpc_read_string(&env, (xmlrpc_value * const)resultP, &addr);
  dieIfFaultOccurred(&env);
  printf("oracc-client: debug: REMOTE_ADDRESS=%s.\n", addr);
}

static void
debug_call(xmlrpc_env *const envP, const char *serverURL)
{
  return xmlrpc_client_call(&env, serverUrl, "debug", "()", NULL);
}

static xmlrpc_value *
debug_method(xmlrpc_env *const envP,
	     xmlrpc_value *const paramArrayP, 
	     void *serverInfo,
	     void *callInfo
	     )
{
  const char *addr = getenv("REMOTE_ADDR");
  fprintf(stderr, "oracc-xmlrpc: debug: REMOTE_ADDR=%s\n", addr);
  return xmlrpc_build_value(envP, "s", addr);
}

struct client_method_info m_debug_client =
{
  "debug",
  debug_call,
  debug_action,
};

struct xmlrpc_method_info3 m_debug_serve =
{
  "debug",
  debug_method,
  NULL,
  0,
  "s:",
  "return the value of the REMOTE_ADDR environment variable",
};

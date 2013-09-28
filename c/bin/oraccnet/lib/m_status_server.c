#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

static xmlrpc_value *status_method(xmlrpc_env *const envP,
			       xmlrpc_value *const paramArrayP, 
			       void *serverInfo,
			       void *callInfo
			       );

struct xmlrpc_method_info3 status_server_info =
{
  "status",
  status_method,
  NULL,
  0,
  "s:",
  "return the status for the session given in the request"
};

static xmlrpc_value *
status_method(xmlrpc_env *const envP,
	  xmlrpc_value *const paramArrayP, 
	  void *serverInfo,
	  void *callInfo
	  )
{
  const char *addr = getenv("REMOTE_ADDR");
  xmlrpc_value *s = NULL, *s_ret = NULL, *exec_ret = NULL;
  struct call_info *cip, *cip_clone;

  trace();
  fprintf(stderr, "oracc-xmlrpc: status: REMOTE_ADDR=%s\n", addr);
  xmlrpc_array_read_item(envP, paramArrayP, 0, &s);
  dieIfFaultOccurred(envP);

  /* No session initialization for status method */

  cip = callinfo_unpack(envP, s);
  cip->clientIP = addr;
  cip_clone = callinfo_clone(cip);
  cip_clone->files = NULL;
  cip_clone->methodargs = NULL;
  cip_clone = callinfo_clone(cip);
  s_ret = callinfo_pack(envP, cip_clone);
  exec_ret = status_get(envP, cip);
  xmlrpc_struct_set_value(envP, exec_ret, "callinfo", s_ret);
  trace();

  return exec_ret;
}

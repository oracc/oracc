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
  xmlrpc_value *s = NULL, *s_ret = NULL, *exec_ret = NULL;
  struct call_info *cip, *cip_clone;

  trace();
  xmlrpc_array_read_item(envP, paramArrayP, 0, &s);
  dieIfFaultOccurred(envP);
  trace();

  cip = callinfo_unpack(envP, s);
  fprintf(stderr, "oracc-xmlrpc: status: %s\n", cip->session);
  sesh_set_path(cip);
  cip_clone = callinfo_clone(cip);
  cip_clone->files = NULL;
  cip_clone->methodargs = NULL;
  s_ret = callinfo_pack(envP, cip_clone);
  trace();
  exec_ret = status_get(envP, cip);
  trace();
  xmlrpc_struct_set_value(envP, exec_ret, "callinfo", s_ret);
  trace();

  return exec_ret;
}

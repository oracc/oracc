#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

static xmlrpc_value *ox_method(xmlrpc_env *const envP,
			       xmlrpc_value *const paramArrayP, 
			       void *serverInfo,
			       void *callInfo
			       );

struct xmlrpc_method_info3 ox_server_info =
{
  "ox",
  ox_method,
  NULL,
  0,
  "s:",
  "return the value of call_info structure with clientIP added to it from the environment",
};

static xmlrpc_value *
ox_method(xmlrpc_env *const envP,
	  xmlrpc_value *const paramArrayP, 
	  void *serverInfo,
	  void *callInfo
	  )
{
  const char *addr = getenv("REMOTE_ADDR");
  xmlrpc_value *s, *s_ret, *exec_ret;
  struct call_info *cip;

  trace();

  fprintf(stderr, "oracc-xmlrpc: ox: REMOTE_ADDR=%s\n", addr);
  xmlrpc_array_read_item(envP, paramArrayP, 0, &s);
  sesh_init(envP, s, 1);

  cip = callinfo_unpack(envP, s);
  file_save(cip, "/Users/stinney/varoracc");
  
  cip->files = NULL;
  cip->methodargs = NULL;
  cip->clientIP = addr;

  trace();

  s_ret = callinfo_pack(envP, cip);
  
  trace();

  exec_ret = request_exec(envP, "/usr/local/oracc/bin/ox", "ox", cip, "/private/tmp/ox.log");
  xmlrpc_struct_set_value(envP, exec_ret, "callinfo", s_ret);

  trace();

  return exec_ret;
}

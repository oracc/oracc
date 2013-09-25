#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  xmlrpc_value *s = NULL, *s_ret = NULL, *exec_ret = NULL;
  struct call_info *cip, *cip_clone;
  struct file_data *infile = NULL;

  trace();

  fprintf(stderr, "oracc-xmlrpc: ox: REMOTE_ADDR=%s\n", addr);
  trace();

  xmlrpc_array_read_item(envP, paramArrayP, 0, &s);
  dieIfFaultOccurred(envP);
  trace();

  fprintf(stderr, "before sesh_init\n");

  sesh_init(envP, s, 1);

  fprintf(stderr, "after sesh_init\n");

  trace();

  cip = callinfo_unpack(envP, s);
  trace();

  cip->clientIP = addr;
  trace();

  file_save(cip, "/Users/stinney/varoracc");

  trace();

  cip_clone = callinfo_clone(cip);
  trace();

  cip_clone->files = NULL;
  trace();

  cip_clone->methodargs = NULL;
  trace();

  cip_clone = callinfo_clone(cip);
  trace();

  s_ret = callinfo_pack(envP, cip_clone);
  trace();
  
  infile = file_find(cip, "in");
  if (infile)
    {
      xmlrpc_value *b64;
      char *logfile = sesh_file("ox.log");
      trace();
      fprintf(stderr, "(1) argv[0] = %s\n", cip->methodargs[0]);
      callinfo_append_arg(cip, "l", NULL, logfile);
      callinfo_append_arg(cip, NULL, NULL, (const char *)infile->path);
      fprintf(stderr, "(2) argv[0] = %s\n", cip->methodargs[0]);
      exec_ret = request_exec(envP, "@ORACC@/bin/ox", "ox", cip);
      b64 = file_b64(envP, logfile, "ox_log", "out");
      if (b64)
	{
	  fprintf(stderr, "adding %s to exec_ret\n", logfile);
	  xmlrpc_struct_set_value(envP, exec_ret, "ox_log", b64);
	}
    }
  else
    {
      trace();
      exec_ret = request_error(envP, "oracc-xmlrpc: ox: can't find input file (what=in). Stop", NULL);
    }
  
  xmlrpc_struct_set_value(envP, exec_ret, "callinfo", s_ret);
  trace();

  return exec_ret;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

void
method_files(xmlrpc_env *envP, xmlrpc_value *s)
{
  char *rlog = sesh_file("request.log");
  char *mlog = sesh_file("method.log");
  char *zip = sesh_file("method.zip");
  if (!access(mlog, R_OK))
    {
      xmlrpc_value *x_mlog = file_pack(envP, "mlog", mlog);
      xmlrpc_struct_set_value(envP, s, "method-log", x_mlog);
    }
  if (!access(rlog, R_OK))
    {
      xmlrpc_value *x_rlog = file_pack(envP, "rlog", rlog);
      xmlrpc_struct_set_value(envP, s, "request-log", x_rlog);
    }
  if (!access(zip, R_OK))
    {
      xmlrpc_value *xzip = file_pack(envP, "zip", zip);
      xmlrpc_struct_set_value(envP, s, "method-zip", xzip);
    }
}

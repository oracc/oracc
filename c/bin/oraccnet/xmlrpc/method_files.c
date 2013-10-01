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
  char *log = sesh_file("method.log");
  char *zip = sesh_file("method.zip");
  if (!access(log, R_OK))
    {
      xmlrpc_value *xlog = file_pack(envP, "log", log);
      xmlrpc_struct_set_value(envP, s, "method-log", xlog);
    }
  if (!access(zip, R_OK))
    {
      xmlrpc_value *xzip = file_pack(envP, "zip", zip);
      xmlrpc_struct_set_value(envP, s, "method-zip", xzip);
    }
}

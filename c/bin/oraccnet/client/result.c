#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"

int
result_method_file(xmlrpc_env *envP, xmlrpc_value *s, const char *member, const char *fname)
{
  if (s)
    {
      xmlrpc_value *file = NULL;
      xmlrpc_struct_find_value(envP, s, member, &file);
      if (file)
	return file_dump(envP, file, fname);
    }
  return 0;
}

char *
result_method_status(xmlrpc_env *envP, xmlrpc_value *s)
{
  char *str = NULL;
  if (s)
    {
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, s, "method-status", &status);
      if (status)
	xmlrpc_read_string(envP, status, (const char **)&str);
    }
  return str;
}

char *
result_request_status(xmlrpc_env *envP, xmlrpc_value *s)
{
  char *str = NULL;
  if (s)
    {
      xmlrpc_value *status = NULL;
      xmlrpc_struct_find_value(envP, s, "status", &status);
      dieIfFaultOccurred(envP);
      if (status)
	xmlrpc_read_string(envP, status, (const char **)&str);
    }
  return str;
}

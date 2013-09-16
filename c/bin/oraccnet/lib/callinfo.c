#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static void unpack(xmlrpc_env *envP, xmlrpc_value *s, const char *mem, char **valp);

struct call_info *
callinfo_new(void)
{
  return calloc(1, sizeof(struct call_info));
}

#define nonull(ptr)  (ptr?ptr:"")

xmlrpc_value *
callinfo_pack(xmlrpc_env *envP, struct call_info *cip)
{
  xmlrpc_value *s = xmlrpc_struct_new(envP);
  xmlrpc_value *methargs = xmlrpc_array_new(envP);
  int i;

  xmlrpc_struct_set_value(envP, s, "clientIP", xmlrpc_string_new(envP, nonull(cip->clientIP)));
  xmlrpc_struct_set_value(envP, s, "serverURL", xmlrpc_string_new(envP, nonull(cip->serverURL)));
  xmlrpc_struct_set_value(envP, s, "session", xmlrpc_string_new(envP, nonull(cip->session)));
  xmlrpc_struct_set_value(envP, s, "method", xmlrpc_string_new(envP, nonull(cip->method)));
  xmlrpc_struct_set_value(envP, s, "user", xmlrpc_string_new(envP, nonull(cip->user)));
  xmlrpc_struct_set_value(envP, s, "password", xmlrpc_string_new(envP, nonull(cip->password)));
  xmlrpc_struct_set_value(envP, s, "project", xmlrpc_string_new(envP, nonull(cip->project)));
  xmlrpc_struct_set_value(envP, s, "version", xmlrpc_string_new(envP, nonull(cip->version)));

  for (i = 0; cip->methodargs[i]; ++i)
    xmlrpc_array_append_item(envP, methargs, xmlrpc_string_new(envP, cip->methodargs[i]));
  xmlrpc_struct_set_value(envP, s, "methodargs", methargs);

  return s;
}

struct call_info *
callinfo_unpack(xmlrpc_env *envP, xmlrpc_value *s)
{
  struct call_info *cip = callinfo_new();
  xmlrpc_value *methargs;
  int i;

  unpack(envP, s, "clientIP", &cip->clientIP);
  unpack(envP, s, "serverURL", &cip->serverURL);
  unpack(envP, s, "session", &cip->session);
  unpack(envP, s, "method", &cip->method);
  unpack(envP, s, "user", &cip->user);
  unpack(envP, s, "password", &cip->password);
  unpack(envP, s, "project", &cip->project);
  unpack(envP, s, "version", &cip->version);
  cip->methodargs = malloc(METHOD_ARGS_MAX * sizeof(char *));
  xmlrpc_struct_find_value(envP, s, "methodargs", &methargs);
  for (i = 0; i < xmlrpc_array_size(envP, methargs); ++i)
    {
      xmlrpc_value *v;
      xmlrpc_array_read_item(envP, methargs, i, &v);
      xmlrpc_read_string(envP, v, (const char **const)(&cip->methodargs[i]));
    }
  cip->methodargs[i] = NULL;
  return cip;
}

static void
unpack(xmlrpc_env *envP, xmlrpc_value *s, const char *mem, char **valp)
{
  xmlrpc_value *val;

  xmlrpc_struct_find_value(envP, s, mem, &val);
  if (val)
    xmlrpc_read_string(envP, val,  (const char **const)valp);
}

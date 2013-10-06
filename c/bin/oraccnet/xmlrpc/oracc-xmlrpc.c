#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

#include "registry.c"

#define EXES "XXXXXX"

static xmlrpc_registry *
register_methods(xmlrpc_env *env)
{
  int i;
  xmlrpc_registry *registryP = xmlrpc_registry_new(env);
  for (i = 0; methodlist[i]; ++i)
    xmlrpc_registry_add_method3(env, registryP, methodlist[i]);
  return registryP;
}

int
main (int const argc, const char **const argv)
{
  xmlrpc_registry *registryP;
  xmlrpc_env env;
  char *sesh_buf;

  trace = 1;
  umask(0);

  sesh_buf = malloc(strlen(varoracc) + strlen(EXES) + 2);
  sprintf(sesh_buf, "%s/%s", varoracc, EXES);
  sesh_set_template(sesh_buf);
  if (argc - 1 > 0 && argv == argv)
    {
      fprintf (stderr, "There are no arguments to a CGI script\n");
      exit (1);
    }
  xmlrpc_env_init(&env);
  registryP = register_methods(&env);
  fprintf(stderr, "Calling xmlrpc_server_cgi_process_call\n");
  xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 4 * XMLRPC_XML_SIZE_LIMIT_DEFAULT);
  xmlrpc_server_cgi_process_call(registryP);
  xmlrpc_registry_free(registryP);
  return 0;
}

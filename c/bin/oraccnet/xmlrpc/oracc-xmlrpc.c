#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_cgi.h>
#include "oraccnet.h"

#include "registry.c"

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

  trace = 1;
  umask(0);

  sesh_set_template("/Users/stinney/varoracc/XXXXXX");
  if (argc - 1 > 0 && argv == argv)
    {
      fprintf (stderr, "There are no arguments to a CGI script\n");
      exit (1);
    }
  xmlrpc_env_init(&env);
  registryP = register_methods(&env);
  fprintf(stderr, "Calling xmlrpc_server_cgi_process_call\n");
  xmlrpc_server_cgi_process_call(registryP);
  xmlrpc_registry_free(registryP);
  return 0;
}

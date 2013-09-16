#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"
#include "config.h"  /* information about this build environment */

#define NAME "oracc-client"
#define VERSION "1.0"

static int options(int argc, char *argv[], struct call_info *cip);
static void usage(void);

int 
main(int argc, char *argv[0])
{
  xmlrpc_env env;
  char * const serverURL = "http://oracc.bfos/xmlrpc";
  xmlrpc_value *resultP = NULL;
  struct client_method_info *cmi = NULL;
  struct meths_tab *meth = NULL;
  struct call_info *cip = NULL;
  
  cip = callinfo_new();
  cip->methodargs = malloc(METHOD_ARGS_MAX * sizeof(char*));
  cip->methodargs[0] = NULL;
  if (options(argc, argv, cip))
    {
      usage();
      exit(1);
    }

  if (!cip->serverURL)
    cip->serverURL = serverURL;

  if (!(meth = meths(cip->method, strlen(cip->method))))
    {
      fprintf(stderr, "oracc-client: unknown method name `%s'\n", argv[1]);
      exit(1);
    }

  cmi = meth->info;
  meth->info->instance = cip;

  /* Initialize our error-handling environment. */
  xmlrpc_env_init(&env);
  
  /* Start up our XML-RPC client library. */
  xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
  dieIfFaultOccurred(&env);
  
  resultP = cmi->call(&env, cmi);
  dieIfFaultOccurred(&env);

#if 0
  if (cmi->wait_seconds)
    {
      while (status(&env, cmi))
	{
	  sleep(cmi->wait_seconds);
	}
      resultP = cmi->term(&env, cmi);
    }
#endif

  cmi->action(&env, cmi, resultP);

  /* Dispose of our result value. */
  xmlrpc_DECREF(resultP);
  
  /* Clean up our error-handling environment. */
  xmlrpc_env_clean(&env);
  
  /* Shutdown our XML-RPC client library. */
  xmlrpc_client_cleanup();
  
  return 0;
}

static void
add_method_arg(struct call_info *cmi, char *arg)
{
  static int Margs = 0;
  
  if (Margs < METHOD_ARGS_MAX)
    {
      cmi->methodargs[Margs++] = arg;
    }
  else
    {
      fprintf(stderr, "oracc-client: too many -M args (max = %d)\n", METHOD_ARGS_MAX);
      exit(1);
    }
}

static int
badopt(char opt)
{
  fprintf(stderr, "oracc-client: option '%c' must be followed by an argument\n", opt);
  return 1;
}

static int
options(int argc, char *argv[0], struct call_info *ci)
{
  int ch;
  while ((ch = getopt(argc, argv, "m:M:p:s:u:v:w:")) != -1) 
    {
      switch (ch) 
	{
	case 'm':
	  if (optarg)
	    ci->method = optarg;
	  else
	    return badopt('m');
	  break;
	case 'M':
	  if (optarg)
	    add_method_arg(ci, optarg);
	  else
	    return badopt('M');
	  break;
	case 'p':
	  if (optarg)
	    ci->project = optarg;
	  else
	    return badopt('p');
	  break;
	case 's':
	  if (optarg)
	    ci->serverURL = optarg;
	  else
	    return badopt('s');
	  break;
	case 'u':
	  if (optarg)
	    ci->user = optarg;
	  else
	    return badopt('u');
	  break;
	case 'v':
	  if (optarg)
	    ci->version = optarg;
	  else
	    return badopt('v');
	  break;
	case 'w':
	  if (optarg)
	    ci->password = optarg;
	  else
	    return badopt('w');
	  break;
	case '?':
	default:
	  usage();
	  exit(0);
	  break;
	}
    }
  argc -= optind;
  argv += optind;
  add_method_arg(ci, NULL);

  if (argv[0])
    {
      fprintf(stderr, "oracc-client: junk on command line after options\n");
      return 1;
    }
  return 0;
}

static void
usage(void)
{
  fprintf(stderr, "oracc-client usage:\n");
  fprintf(stderr, 
	  "oracc-client [OPTIONS]\n"
	  "   -m method\n"
	  "  [-p PROJECT ]\n"
	  "  [-s SERVER (include /xmlrpc) ]\n"
	  "  [-u USER ]\n"
	  "  [-v VERSION (of project) ]\n"
	  "  [-w PASSWORD ]\n"
	  );
}

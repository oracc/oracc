#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include "oraccnet.h"
#include "config.h"  /* information about this build environment */

#define NAME "oracc-client"
#define VERSION "1.0"

static int wait_arg = 0;

static int options(int argc, char *argv[], struct call_info *cip);
static void usage(void);

int 
main(int argc, char *argv[])
{
  xmlrpc_env env;
  char * const serverURL = "http://oracc.bfos/xmlrpc";
  xmlrpc_value *resultP = NULL;
  struct client_method_info *cmi = NULL;
  struct meths_tab *meth = NULL;
  struct call_info *cip = NULL;
  
  cip = callinfo_new();
  cip->methodargs = malloc(argc * sizeof(char*));
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

  if (wait_arg > 0)
    cip->wait_seconds = wait_arg;
  else
    cip->wait_seconds = *cmi->wait_seconds;

  /* Initialize our error-handling environment. */
  xmlrpc_env_init(&env);
  
  /* Start up our XML-RPC client library. */
  xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
  dieIfFaultOccurred(&env);
  
  resultP = cmi->call(&env, cmi);
  dieIfFaultOccurred(&env);

  if (cip->wait_seconds)
    {
      xmlrpc_value *callinfo;
      struct call_info *server_cip;

      xmlrpc_struct_find_value(&env, resultP, "callinfo", &callinfo);
      dieIfFaultOccurred(&env);
      server_cip = callinfo_unpack(&env, callinfo);
      cip->session = server_cip->session;
      do
	{
	  fprintf(stderr, "oracc-client: %s: sleeping %d seconds in session %s ...\n", 
		  cip->method, cip->wait_seconds, cip->session);
	  sleep(cip->wait_seconds);
	}
      while ((resultP = server_status(&env, cip)) == NULL);
    }

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
  cmi->methodargs[Margs++] = arg;
}

static int
badopt(char opt)
{
  fprintf(stderr, "oracc-client: option '%c' must be followed by an argument\n", opt);
  return 1;
}

static int
options(int argc, char *argv[], struct call_info *ci)
{
  int ch;
  while ((ch = getopt(argc, argv, "m:M:p:P:s:S:u:v:w:")) != -1) 
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
	case 'P':
	  if (optarg)
	    ci->password = optarg;
	  else
	    return badopt('w');
	  break;
	case 's':
	  if (optarg)
	    ci->serverURL = optarg;
	  else
	    return badopt('s');
	  break;
	case 'S':
	  if (optarg)
	    ci->session = optarg;
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
	  wait_arg = atoi(optarg);
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
	  "  [-S SESSION (only with -m status)]\n"
	  "  [-u USER ]\n"
	  "  [-P PASSWORD ]\n"
	  "  [-v VERSION (of project) ]\n"
	  "  [-w SECONDS (set client wait to SECONDS) ]\n"
	  "  [-MARG[=VAL] (set ARG for method VAL optional) ]\n"
	  "  [-Mfile:WHAT=NAME (set file arg with name NAME and purpose WHAT) ]\n"
	  );
}

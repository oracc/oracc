#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static void unpack(xmlrpc_env *envP, xmlrpc_value *s, const char *mem, char **valp);

struct call_info *
callinfo_new(void)
{
  return calloc(1, sizeof(struct call_info));
}

#define nonull(ptr)  (ptr?ptr:"")


/* Pack the information from any client method into a generic xmlrpc
   struct to send to the server */
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
    {
      xmlrpc_array_append_item(envP, methargs, xmlrpc_string_new(envP, cip->methodargs[i]));
      if (!strncmp(cip->methodargs[i], "file:", 5))
	{
	  const char *what, *name, *copy, *struct_name, *equals, *struct_name;
	  xmlrpc_value *file_struct, *file_what, *file_name;
	  
	  copy = strdup(cip->methodargs[i]);
	  equals = strchr(copy, '=');
	  if (equals)
	    {
	      /* REWRITE TO PACK ALL FILES INTO A SINGLE ARRAY WHICH IS THE VALUE OF THE STRUCT
		 MEMBER #content */
	      *equals = '\0';
	      struct_name = strdup(copy);
	      xmlrpc_value *content = file_pack(envP, file_what, file_name);
	      xmlrpc_struct_set_value(envP, s, struct_name, content);
	    }
	  else
	    {
	      fprintf(stderr, "oracc-client: bad -Mfile arg, expected -Mfile:WHAT=NAME\n");
	      exit(1);
	    }
	}
    }
  xmlrpc_struct_set_value(envP, s, "methodargs", methargs);

  return s;
}

/* Unpack the information returned by the server for use by the client
   after the method has been called */
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
      xmlrpc_value *files;
      struct file *tmp = NULL;
      xmlrpc_array_read_item(envP, methargs, i, &v);
      xmlrpc_read_string(envP, v, (const char **const)(&cip->methodargs[i]));
      if (!strncmp(cip->methodargs[i], "file:", 5))
	{
	  const char *what, *name;
	  what = &cip->methodargs[i][5];
	  name = strchr(what, '=');
	  if (name)
	    ++name;
	  else
	    {
	      fprintf(stderr, "oracc-client: bad -Mfile arg, expected -Mfile:WHAT=NAME\n");
	      exit(1);
	    }
	  if (cip->files == NULL)
	    {
	      cip->files = file_unpack(envP, fstruct);
	      cip->files_last = cip->files;
	    }
	  else
	    {
	      cip->files_last->next = file_unpack(envP, fstruct);
	      cip->files_last = cip->files_last->next;
	    }
	}
    }
  cip->methodargs[i] = NULL;

  if (!cip->session || !*cip->session)
    cip->session = create_session(envP, s);

  return cip;
}

static void
unpack(xmlrpc_env *envP, xmlrpc_value *s, const char *mem, char **valp)
{
  xmlrpc_value *val;

  xmlrpc_struct_find_value(envP, s, mem, &val);
  if (val)
    xmlrpc_read_string(envP, val, (const char **const)valp);
}

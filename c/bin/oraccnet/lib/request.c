#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include "oraccnet.h"

static void request_debug(const char *path, const char **argv, const char **envp);

#define ERR_MAX 512

const char **
request_argv(const char *name, struct call_info *cip)
{
  const char **ret = malloc(sizeof(char *) * (cip->nargs + 2));
  int i;
  request_debug("request_argv", (const char **)cip->methodargs, NULL);
  ret[0] = name;
  for (i = 0; i < cip->nargs; ++i)
    ret[i+1] = cip->methodargs[i];
  ret[++i] = NULL;
  return ret;
}

xmlrpc_value *
request_common(xmlrpc_env *const envP, const char *type, const char *fmt, va_list ap)
{
  static char buf[ERR_MAX];
  xmlrpc_value *s, *str;

  trace();
  /*  fprintf(stderr, "request_common: fmt=%s\n", fmt); */
  vsnprintf(buf, ERR_MAX, fmt, ap);
  trace();

  str = xmlrpc_string_new(envP, buf);
  dieIfFaultOccurred(envP);

  s = xmlrpc_struct_new(envP);
  xmlrpc_struct_set_value(envP, s, type, str);
  dieIfFaultOccurred(envP);

  trace();

  return s;
}

static void
request_debug(const char *path, const char **argv, const char **envp)
{
  int i;
  fprintf(stderr, 
	  "oracc-xmlrpc: request\n\t"
	  "path=%s", path);
  if (argv)
    {
      fprintf(stderr, "\n\targv=");
      for (i = 0; argv[i]; ++i)
	fprintf(stderr, "%s ", argv[i]);
    }
  if (envp)
    {
      fprintf(stderr, "\n\tenvp=");
      for (i = 0; envp[i]; ++i)
	fprintf(stderr, "%s ", envp[i]);
    }
  fprintf(stderr, "\n");
}

const char **
request_envp(struct call_info *cip)
{
  return NULL;
}

xmlrpc_value *
request_exec(xmlrpc_env * const envP, const char *path, const char *name, struct call_info *cip)
{
  pid_t pid;
  const char **argv = NULL;
  const char **envp = NULL;
  char *request_log = sesh_file("request.log");
  
  trace();

  if (cip->nargs && cip->methodargs)
    argv = request_argv(name, cip);
  envp = request_envp(cip);

  fprintf(stderr, "oracc-xmlrpc: entering request_exec\n");

  if ((pid = fork()) < 0)
    {
      /* set error status to return to client */
      trace();

      return request_error(envP,
			   "oracc-xmlrpc: %s: fork failed: %s", 
			   cip->method,
			   strerror(errno),
			   NULL);
    }
  else if (pid != 0) /* parent */
    {
      /* either wait for child if the method has wait_seconds == 0, or return OK 
	 status to send back to client, which will then ping the server until it
	 gets back a 'completed' status
       */
      trace();

      if (0 == cip->wait_seconds)
	{
	  xmlrpc_value *s;
	  int sloc;

	  waitpid(pid, &sloc, 0);
	  if (WIFSIGNALED(sloc))
	    {
	      trace();
	      return request_error(envP, "oracc-xmlrpc: %s: unexpected failure or crash\n", cip->method, NULL);
	    }
	  else
	    {
	      trace();
	      s = request_status(envP, "OK", NULL);
	    }

	  xmlrpc_struct_set_value(envP, s, "request_log",
				  file_b64(envP, request_log, "request_log", "request_log"));
	  dieIfFaultOccurred(envP);
	  return s;
	}
      else
	return request_status(envP, "started", NULL);
    }
  else /* child */
    {
      int fd;

      trace();

      fprintf(stderr, "oracc-xmlrpc: switching child output to %s\n", request_log);

      if ((fd = open(request_log, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0)
	{
	  perror("request-open");
	  return request_error(envP, "oracc-xmlrpc: %s: child failed to open %s\n%s", cip->method, request_log, strerror(errno), NULL);
	}
      if (dup2(fd, fileno(stderr)) < 0)
	{
	  perror("request-dup2-stderr");
	  return request_error(envP, "oracc-xmlrpc: %s: child failed to attach stdout to %s\n%s", cip->method, request_log, strerror(errno), NULL);
	}
      if (dup2(fd, fileno(stdout)) < 0)
	{
	  perror("request-dup2-stdout");
	  return request_error(envP, "oracc-xmlrpc: %s: child failed to attach stderr to %s\n%s", cip->method, request_log, strerror(errno), NULL);
	}
      else
	setbuf(stdout, NULL);
      request_debug(path, argv, envp);
      if (execve(path, (char *const *)argv, (char *const *)envp))
	{
	  perror("request-execve");
	  return request_error(envP,
			       "oracc-xmlrpc: %s: exec failed\n%s",
			       cip->method,
			       strerror(errno),
			       NULL);
	}
      else
	return NULL; /* NEVER HAPPENS */
    }

  trace();
}

xmlrpc_value *
request_error(xmlrpc_env *const envP, const char *fmt, ...)
{
  va_list ap;
  xmlrpc_value *s;

  trace();

  va_start(ap, fmt);
  s = request_common(envP, "error", fmt, ap);
  va_end(ap);

  trace();

  return s;
}

xmlrpc_value *
request_status(xmlrpc_env *const envP, const char *fmt, ...)
{
  va_list ap;
  xmlrpc_value *s;

  trace();

  va_start(ap, fmt);
  s = request_common(envP, "status", fmt, ap);
  va_end(ap);

  trace();

  return s;
}

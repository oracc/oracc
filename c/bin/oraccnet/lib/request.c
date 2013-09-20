#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include "oraccnet.h"

#define ERR_MAX 512

const char **
request_argv(const char *name, struct call_info *cip)
{
  const char **ret = malloc(sizeof(char *) * (cip->nargs + 2));
  ret[0] = name;
  memcpy(&ret[1], cip->methodargs, (cip->nargs + 1) * sizeof(char*));
  return ret;
}

xmlrpc_value *
request_common(xmlrpc_env *const envP, const char *type, const char *fmt, va_list ap)
{
  char buf[ERR_MAX];
  xmlrpc_value *s, *str;

  trace();
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

const char **
request_envp(struct call_info *cip)
{
  return NULL;
}

xmlrpc_value *
request_exec(xmlrpc_env * const envP, const char *path, const char *name, struct call_info *cip, const char *logfile)
{
  pid_t pid;
  const char **argv = NULL;
  const char **envp = NULL;
  
  trace();

  if (cip->nargs && cip->methodargs)
    argv = request_argv(name, cip);
  envp = request_envp(cip);

  fprintf(stderr, "oracc-xmlrpc: entering request_exec\n");

  if ((pid = fork()) < 0)
    {
      /* set error status to return to client */
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
      if (0 == cip->wait_seconds)
	{
	  int sloc, logfd;
	  struct stat logstat;
	  unsigned char *logbuf = NULL;
	  xmlrpc_value *b64, *s;
	  unsigned int loglen;

	  waitpid(pid, &sloc, 0);
	  if (WIFSIGNALED(sloc))
	    return request_error(envP, "oracc-xmlrpc: %s: unexpected failure or crash\n", NULL);
	  else
	    s = request_status(envP, "OK", NULL);

	  trace();

	  if ((stat(logfile, &logstat)) < 0)
	    return request_error(envP, "oracc-xmlrpc: %s: parent failed to stat ox.log\n%s", strerror(errno));

	  loglen = (unsigned int)logstat.st_size;

	  fprintf(stderr, "logfile %s is %d bytes in length\n", logfile, (int)loglen);

	  trace();

	  if ((logfd = open(logfile, O_RDONLY)) < 0)
	    return request_error(envP, "oracc-xmlrpc: %s: parent failed to open ox.log\n%s", strerror(errno));

	  fprintf(stderr, "logfile = %s; logfd = %d; loglen = %u\n", logfile, logfd, loglen);

	  trace();

	  logbuf = malloc(loglen + 1);

	  trace();

	  if (read(logfd, logbuf, loglen) < 0)
	    return request_error(envP, "oracc-xmlrpc: %s: parent failed to read %u bytes from ox.log\n%s", 
				 loglen, 
				 strerror(errno));
	  close(logfd);

	  trace();

	  logbuf[loglen] = '\0';

	  trace();

	  if (NULL == (b64 = file_b64(envP, "ox.log", "log", loglen, logbuf)))
	    dieIfFaultOccurred(envP);

	  trace();

	  xmlrpc_struct_set_value(envP, s, "log", b64);
	  dieIfFaultOccurred(envP);
	  return s;
	}
      else
	return request_status(envP, "started", NULL);
    }
  else /* child */
    {
      int fd;
      if ((fd = open(logfile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) < 0)
	return request_error(envP, "oracc-xmlrpc: %s: child failed to open ox.log\n%s", strerror(errno));
      if (dup2(fd, fileno(stderr)) < 0)
	return request_error(envP, "oracc-xmlrpc: %s: child failed to attach stdout to ox.log\n%s", strerror(errno));
      if (dup2(fd, fileno(stdout)) < 0)
	return request_error(envP, "oracc-xmlrpc: %s: child failed to attach stderr to ox.log\n%s", strerror(errno));
      if (execve(path, (char *const *)argv, (char *const *)envp))
	return request_error(envP,
			     "oracc-xmlrpc: %s: exec failed\n%s",
			     cip->method,
			     strerror(errno),
			     NULL);
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

/* this routine is only used by the rpc server */
xmlrpc_value *
file_b64(xmlrpc_env * const envP, const char *path, const char *name, const char *what)
{
  int pfd;
  struct stat pstat;
  unsigned char *pbuf = NULL;
  unsigned int plen;
  xmlrpc_value *b64_data, *b64_name, *b64_size, *b64_what, *b64;

  fprintf(stderr, "file_b64: processing request for path=%s\n", path);

  if ((stat(path, &pstat)) < 0)
    return request_error(envP, "oracc-xmlrpc: parent failed to stat %s\n%s", path, strerror(errno));
  
  plen = (unsigned int)pstat.st_size;

  if ((pfd = open(path, O_RDONLY)) < 0)
    return request_error(envP, "oracc-xmlrpc: parent failed to open file %s\n%s", path, strerror(errno));
  
  pbuf = malloc(plen + 1);
  if (read(pfd, pbuf, plen) < 0)
    return request_error(envP, "oracc-xmlrpc: parent failed to read %u bytes from %s\n%s", 
			 plen, path,
			 strerror(errno));
  close(pfd);
  pbuf[plen] = '\0';
  
  b64_data = xmlrpc_base64_new(envP, plen, pbuf);
  if (envP->fault_occurred) return NULL;
  
  b64_name = xmlrpc_string_new(envP, name);
  if (envP->fault_occurred) return NULL;
  
  b64_size = xmlrpc_int_new(envP, plen);
  if (envP->fault_occurred) return NULL;
  
  b64_what = xmlrpc_string_new(envP, what);
  if (envP->fault_occurred) return NULL;
  
  b64 = xmlrpc_struct_new(envP);
  if (envP->fault_occurred) return NULL;

  xmlrpc_struct_set_value(envP, b64, "data", b64_data);
  if (envP->fault_occurred) return NULL;

  xmlrpc_struct_set_value(envP, b64, "name", b64_name);
  if (envP->fault_occurred) return NULL;

  xmlrpc_struct_set_value(envP, b64, "size", b64_size);
  if (envP->fault_occurred) return NULL;

  xmlrpc_struct_set_value(envP, b64, "what", b64_what);
  if (envP->fault_occurred) return NULL;

  return b64;
}

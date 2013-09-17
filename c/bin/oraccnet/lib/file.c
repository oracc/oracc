#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

xmlrpc_value *
file_pack(xmlrpc_env * const envP, const char *filename)
{
  xmlrpc_value *b64 = NULL;
  int fd, statres;
  struct stat statbuf;
  unsigned char *content;

  if ((statres = stat(filename, &statbuf)))
    {
      perror("oracc-client");
      exit(1);
    }
  if ((fd = open(filename, O_RDONLY)) < 0)
    {
      perror("oracc-client");
      exit(1);
    }
  /* should trap oversized files here */
  if ((content = malloc(statbuf.st_size)) == NULL)
    {
      perror("oracc-client");
      exit(1);
    }
  if ((read(fd, content, statbuf.st_size)) < 0)
    {
      perror("oracc-client");
      exit(1);
    }

  b64 = xmlrpc_base64_new(envP, statbuf.st_size, content);

  return b64;
}

void
file_unpack(xmlrpc_env * const envP, xmlrpc_value * const filename, xmlrpc_value * const content)
{
  const unsigned char * cvalue;
  size_t len;
  xmlrpc_read_base64(envP, content, &len, &cvalue);
  
}

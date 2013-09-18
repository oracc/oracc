#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

xmlrpc_value *
file_pack(xmlrpc_env * const envP, const char *file_what, const char *file_name)
{
  xmlrpc_value *b64 = NULL;
  xmlrpc_value *b64_data = NULL;
  xmlrpc_value *b64_name = NULL;
  xmlrpc_value *b64_size = NULL;
  xmlrpc_value *b64_what = NULL;
  int fd, statres;
  struct stat statbuf;
  unsigned char *content;

  if ((statres = stat(file_name, &statbuf)))
    {
      perror("oracc-client");
      exit(1);
    }
  if ((fd = open(file_name, O_RDONLY)) < 0)
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

  b64_data = xmlrpc_base64_new(envP, statbuf.st_size, content);
  b64_name = xmlrpc_string_new(file_name);
  b64_size = xmlrpc_int_new(envP, (int)statbuf.st_size);
  b64_what = xmlrpc_string_new(file_what);

  b64 = xmlrpc_struct_new(envP);
  xmlrpc_struct_set_value(envP, b64, "data", b64_data);
  xmlrpc_struct_set_value(envP, b64, "name", b64_name);
  xmlrpc_struct_set_value(envP, b64, "size", b64_size);
  xmlrpc_struct_set_value(envP, b64, "what", b64_what);

  return b64;
}

struct file_data *
file_unpack(xmlrpc_env * const envP, xmlrpc_value * const fstruct)
{
  struct file_data *file;
  xmlrpc_value *const b64_data, * const b64_name, * const b64_size, * const b64_what;
  
  xmlrpc_struct_find_value(envP, file, "data", &b64_data);
  xmlrpc_struct_find_value(envP, file, "name", &b64_name);
  xmlrpc_struct_find_value(envP, file, "size", &b64_size);
  xmlrpc_struct_find_value(envP, file, "what", &b64_what);

  if (b64_data)
    xmlrpc_read_base64(envP, b64_data, &len, &file->data);
  if (b64_name)
    xmlrpc_read_string(envP, b64_name, (const char **const)(&file->name));
  if (b64_size)
    xmlrpc_read_int(envP, b64_size, &file->size);
  if (b64_what)
    xmlrpc_read_string(envP, b64_what, (const char **const)(&file->what));

  return file;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static void file_save_sub(struct file_data *fdatap, const char *dir, const char *session);

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
  b64_name = xmlrpc_string_new(envP, file_name);
  b64_size = xmlrpc_int_new(envP, (int)statbuf.st_size);
  b64_what = xmlrpc_string_new(envP, file_what);

  b64 = xmlrpc_struct_new(envP);
  xmlrpc_struct_set_value(envP, b64, "data", b64_data);
  xmlrpc_struct_set_value(envP, b64, "name", b64_name);
  xmlrpc_struct_set_value(envP, b64, "size", b64_size);
  xmlrpc_struct_set_value(envP, b64, "what", b64_what);

  return b64;
}

void
file_save(struct call_info *cip, const char *dir)
{
  struct file_data *fdatap;
  for (fdatap = cip->files; fdatap; fdatap = fdatap->next)
    file_save_sub(fdatap, dir, cip->session);
}

static void
file_save_sub(struct file_data *fdatap, const char *dir, const char *session)
{
  char *path = malloc(strlen(dir) + strlen((char *)fdatap->name) + 2);
  int fd, res;
  sprintf(path, "%s/%s/%s", dir, session, fdatap->name);
  fprintf(stderr, "oraccnet:file_save_sub: saving %s\n", path);
  /* try to open; on fail check for existence of file's dir--if it's not there
     use system mkdir -p to make it */
  if ((fd = open(path, O_WRONLY|O_CREAT)) < 0)
    {
      /* in xmlrpc server mode we need to send this error back to client */
      fprintf(stderr, "oraccnet:file_save_sub: failed to open %s\n", path);
      perror("oraccnet:file_save_sub");
      exit(1);
    }
  if ((res = write(fd, fdatap->data, fdatap->size)) < 0)
    {
      fprintf(stderr, "oraccnet:file_save_sub: write failed trying to save %d bytes to %s\n", fdatap->size, path);
      perror("oraccnet:file_save_sub");
      exit(1);
    }
  close(fd);
}

struct file_data *
file_unpack(xmlrpc_env * const envP, xmlrpc_value * const fstruct)
{
  struct file_data *fdata = NULL;
  xmlrpc_value *b64_data, *b64_name, *b64_size, *b64_what;
  size_t len;
  
  fdata = malloc(sizeof(struct file_data));
  xmlrpc_struct_find_value(envP, fstruct, "data", &b64_data);
  xmlrpc_struct_find_value(envP, fstruct, "name", &b64_name);
  xmlrpc_struct_find_value(envP, fstruct, "size", &b64_size);
  xmlrpc_struct_find_value(envP, fstruct, "what", &b64_what);

  if (b64_data)
    {
      xmlrpc_read_base64(envP, b64_data, &len, &fdata->data);
      fdata->size = (int)len;
    }
  if (b64_name)
    xmlrpc_read_string(envP, b64_name, (const char **const)(&fdata->name));
#if 0
  if (b64_size)
    xmlrpc_read_int(envP, b64_size, &fdata->size);
#endif
  if (b64_what)
    xmlrpc_read_string(envP, b64_what, (const char **const)(&fdata->what));

  fdata->next = NULL;

  return fdata;
}

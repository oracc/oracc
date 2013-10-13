#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <xmlrpc-c/base.h>
#include "oraccnet.h"

static void file_save_sub(struct file_data *fdatap, const char *dir, const char *session);

int
file_dump(xmlrpc_env * const envP, xmlrpc_value *const log, const char *fname)
{
  int fd = -1;
  struct file_data *fdata = file_unpack(envP, log);

  if (fname)
    {
      if ('-' == *fname)
	{
	  if ((fd = fileno(stdout)) < 0)
	    {
	      fprintf(stderr, "oracc-client: unable to write to stdout\n");
	      return -1;
	    }
	}
      else
	{
	  if ((fd = open(fname, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0)
	    {
	      fprintf(stderr, "oracc-client: failed to open %s for file dump\n", fname);
	      perror("oracc-client:file_dump");
	      return -1;
	    }
	}
      if (fdata->size != write(fd, fdata->data, fdata->size))
	{
	  fprintf(stderr, "oracc-client: failed to write %ld bytes to %s\n", (unsigned long)fdata->size, fname);
	  perror("oracc-client:file_dump");
	  return -1;      
	}
    }

  return 0;
}

struct file_data *
file_find(struct call_info *cip, const char *what)
{
  struct file_data *fdp;
  for (fdp = cip->files; fdp; fdp = fdp->next)
    if (!strcmp((char*)fdp->what, what))
      return fdp;
  return NULL;
}

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

  trace();

  fprintf(stderr, "file_pack: what=%s; name=%s\n", file_what, file_name);

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

  trace();

  return b64;
}

void
file_save(struct call_info *cip, const char *dir)
{
  struct file_data *fdatap;

  trace();

  for (fdatap = cip->files; fdatap; fdatap = fdatap->next)
    file_save_sub(fdatap, dir, cip->session);

  trace();
}

static void
file_save_sub(struct file_data *fdatap, const char *dir, const char *session)
{
  int fd, res;

  trace();

  fdatap->path = malloc(strlen(dir) + strlen(session) + strlen((char *)fdatap->name) + 3);
  sprintf((char*)fdatap->path, "%s/%s/%s", dir, session, fdatap->name);
  fprintf(stderr, "oraccnet:file_save_sub: saving %s\n", fdatap->path);
  /* try to open; on fail check for existence of file's dir--if it's not there
     use system mkdir -p to make it */
  if ((fd = open((char*)fdatap->path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) < 0)
    {
      /* in xmlrpc server mode we need to send this error back to client */
      fprintf(stderr, "oraccnet:file_save_sub: failed to open %s\n", fdatap->path);
      perror("oraccnet:file_save_sub");
      exit(1);
    }
  if ((res = write(fd, fdatap->data, fdatap->size)) < 0)
    {
      fprintf(stderr, "oraccnet:file_save_sub: write failed trying to save %d bytes to %s\n", fdatap->size, fdatap->path);
      perror("oraccnet:file_save_sub");
      exit(1);
    }
  close(fd);

  trace();

}

struct file_data *
file_unpack(xmlrpc_env * const envP, xmlrpc_value * const fstruct)
{
  struct file_data *fdata = NULL;
  xmlrpc_value *b64_data, *b64_name, *b64_size, *b64_what;
  size_t len;
  
  trace();

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

  trace();

  return fdata;
}

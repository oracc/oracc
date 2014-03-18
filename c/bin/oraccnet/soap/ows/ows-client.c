#include "soapH.h"
#include "xop.nsmap"

int run_client(int, char**);

int open_data(struct soap *soap, const char *file, struct x__Data *data);

int client_putData(struct soap *soap, int argc, char **argv);
int client_getData(struct soap *soap, int argc, char **argv);

int
main(int argc, char **argv)
{
  return run_client(argc, argv);
}

/* EVERYTHING AFTER THIS LINE IS ADAPTED FROM gsoap/samples/mtom-stream */

int
run_client(int argc, char **argv)
{
  struct soap soap;
  int ret = 0;
#ifdef WITH_GZIP
  soap_init1(&soap, SOAP_ENC_MTOM | SOAP_ENC_ZLIB); /* Enable MTOM */
#else
  soap_init1(&soap, SOAP_ENC_MTOM); /* Enable MTOM */
#endif
  /* Set the MIME callbacks */
  soap.fmimereadopen = mime_read_open;
  soap.fmimereadclose = mime_read_close;
  soap.fmimeread = mime_read;
  soap.fmimewriteopen = mime_client_write_open;
  soap.fmimewriteclose = mime_client_write_close;
  soap.fmimewrite = mime_client_write;
  /* Connect timeout value (sec) (not supported by Linux) */
  soap.connect_timeout = 10;
  /* IO timeouts (sec) */
  soap.send_timeout = 30;
  soap.recv_timeout = 30;
  /* Unix/Linux SIGPIPE, this is OS dependent:
     soap.accept_flags = SO_NOSIGPIPE;	// some systems like this
     soap.socket_flags = MSG_NOSIGNAL;	// others need this
     signal(SIGPIPE, sigpipe_handle);	// or a sigpipe handler (more portable)
  */

  switch (argv[1][1])
    {
    case 'p':
      ret = client_putData(&soap, argc, argv);
      break;
    case 'g':
      ret = client_getData(&soap, argc, argv);
      break;
    default:
      fprintf(stderr, "Usage: mtom-stream-test -p file1 file2 file3 ...\n");
      fprintf(stderr, "       mtom-stream-test -g key1 key2 key3 ...\n");
    }

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  return ret;
}

int
open_data(struct soap *soap, const char *file, struct x__Data *data)
{ 
  struct stat sb;
  FILE *fd = NULL;
  int size;
  soap_default_x__Data(soap, data);
  fd = fopen(file, "rb");
  if (!fd)
    { 
      fprintf(stderr, "Cannot open file %s\n", file);
      return soap->error = SOAP_EOF;
    }
  /* The handle for the streaming MIME callback is the open file fd */      
  data->xop__Include.__ptr = (unsigned char*)fd;
  if (!fstat(fileno(fd), &sb) && sb.st_size > 0)
    size = sb.st_size;
  else
    {
      /* File size is unknown, so must use HTTP chunking and set size = 0 */
      soap_set_omode(soap, SOAP_IO_CHUNK);
      size = 0;
    }
  data->xop__Include.__size = size;
  data->xmime5__contentType = file_type(file);
  data->xop__Include.id = NULL;
  data->xop__Include.type = data->xmime5__contentType;
  data->xop__Include.options = NULL;
  return SOAP_OK;
}

int 
client_putData(struct soap *soap, int argc, char **argv)
{ 
  int i;
  struct x__DataSet data;
  struct m__PutDataResponse response;
  data.__size = argc - 2;
  data.item = soap_malloc(soap, (argc - 2)*sizeof(struct x__Data));
  for (i = 2; i < argc; i++)
    open_data(soap, argv[i], &data.item[i - 2]);
  if (soap_call_m__PutData(soap, endpoint, NULL, &data, &response))
    soap_print_fault(soap, stderr);
  else
    { 
      printf("Data stored with keys:\n");
      for (i = 0; i < response.x__keys.__size; i++)
	printf("\t%s\n", response.x__keys.key[i]);
      printf("Use these keys to retrieve the data with -g key1 key2 ...\n");
    }
  return soap->error;
}

int 
client_getData(struct soap *soap, int argc, char **argv)
{ 
  int i;
  struct x__Keys keys;
  struct m__GetDataResponse response;
  keys.__size = argc - 2;
  keys.key = soap_malloc(soap, (argc - 2)*sizeof(char*));
  for (i = 2; i < argc; i++)
    keys.key[i - 2] = argv[i];
  /* Pass this information to the callbacks using the soap->user variable */
  soap->user = (void*)keys.key;
  if (soap_call_m__GetData(soap, endpoint, NULL, &keys, &response))
    soap_print_fault(soap, stderr);
  else
    printf("Data retrieved\n");
  return soap->error;
}

/******************************************************************************\
 *
 *	Client-Side Streaming MIME Callbacks
 *
\******************************************************************************/
void *
mime_client_write_open(struct soap *soap, void *unused_handle, 
		       const char *id, const char *type, const char *description, 
		       enum soap_mime_encoding encoding)
{ 
  /* Note: the 'unused_handle' is always NULL */
  FILE *fd;
  const char *file;
  fprintf(stderr, "Opening streaming inbound MIME channel for id=%s type=%s\n", id, type);
  /* soap->user points to array of keys (strings) that are file names */
  file = *(char**)soap->user;
  soap->user = (void*)(((char**)soap->user)+1);
  fd = fopen(file, "wb");
  if (!fd)
    soap->error = soap_receiver_fault(soap, "Cannot save data to file", file);
  return (void*)fd;
}

void
mime_client_write_close(struct soap *soap, void *handle)
{
  fprintf(stderr, "Closing streaming inbound MIME channel\n");
  fclose((FILE*)handle);
}

int
mime_client_write(struct soap *soap, void *handle, const char *buf, size_t len)
{
  FILE *fd = (FILE*)handle;
  while (len)
  { 
    size_t nwritten = fwrite(buf, 1, len, fd);
    if (!nwritten)
    {
      soap->errnum = errno;
      return SOAP_EOF;
    }
    len -= nwritten;
    buf += nwritten;
  }
  return SOAP_OK;
}

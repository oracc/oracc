int main()
{
   struct soap soap;
   struct xsd__base64Binary image;
   FILE *fd;
   struct stat sb;
   soap_init1(&soap, SOAP_ENC_MTOM); // mandatory to enable MTOM
   if (!fstat(fileno(fd), &sb) && sb.st_size > 0)
   { // because we can get the length of the file, we can stream it without chunking
      soap.fmimereadopen = mime_read_open;
      soap.fmimereadclose = mime_read_close;
      soap.fmimeread = mime_read;
      image.__ptr = (unsigned char*)fd; // must set to non-NULL (this is our fd handle which we need in the callbacks)
      image.__size = sb.st_size; // must set size
   }
   else
   { // don't know the size, so buffer it
      size_t i;
      int c;
      image.__ptr = (unsigned char*)soap_malloc(&soap, MAX_FILE_SIZE);
      for (i = 0; i < MAX_FILE_SIZE; i++)
      {
         if ((c = fgetc(fd)) == EOF)
            break;
         image.__ptr[i] = c;
      }
      fclose(fd);
      image.__size = i;
   }
   image.type = "image/jpeg"; // MIME type
   image.options = "This is my picture"; // description of object
   soap_call_ns__method(&soap, ...);
   ...
}
void *mime_read_open(struct soap *soap, void *handle, const char *id, const char *type, const char *description)
{ return handle;
}
void mime_read_close(struct soap *soap, void *handle)
{ fclose((FILE*)handle);
}
size_t mime_read(struct soap *soap, void *handle, char *buf, size_t len)
{ return fread(buf, 1, len, (FILE*)handle);
} 

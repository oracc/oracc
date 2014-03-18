#include "soapH.h" /* get the gSOAP-generated definitions */
#include "xop.nsmap" /* get the gSOAP-generated namespace bindings */

int
main(int argc, const char **argv)
{
  struct soap *soap = soap_new1(SOAP_ENC_MTOM);
  int soap_ret = 0;

  soap_post_check_mime_attachments(soap);

  soap_ret = soap_serve(soap);
  
  return soap_ret;
}

int
ows__ox(char *args[], char *session, struct ows__Data *infile, struct ows__Data *logfile)
{
  /* set up session with tmpdir */

  /* method logic, which is probs subr with generic ox handling here */

  *logfile = *infile; /* return input for now */

#if 0
    return soap_sender_fault(soap, 
			     "Square root of negative value", 
			     "I can only take the square root of non-negative values");
#endif

  return SOAP_OK;
}

int
ows_mtom(struct soap *soap)
{
  if (soap_check_mime_attachments(soap))
    {
      /* attachments are present, channel is still open */
      {
	do
	  {
	    ... // get data 'handle' from SOAP request and pass to callbacks
	    ... // set the fmime callbacks, if needed
	    struct soap_multipart *content = soap_get_mime_attachment(soap, (void*)handle);
	    printf("Received attachment with id=%s and type=%s\n", 
		   content->id ? content->id : "", 
		   content->type ? content->type : "");
	  }
	while (content);
	if (soap->error)
	  return soap->error;
      }
    }
}

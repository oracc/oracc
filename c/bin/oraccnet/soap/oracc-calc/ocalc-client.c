#include "soapH.h"
#include "ocalc.nsmap"
int main()
{ struct soap *soap = soap_new(); /* create environment */
  double n; /* result value */
  if (soap_call_ocalc__add(soap, NULL, NULL, 2, 3, &n) == SOAP_OK)
    printf("2 plus 3 is %f\n", n);
  else
    soap_print_fault(soap, stderr); /* print error */
  soap_end(soap); /* clean up deserialized data */
  soap_done(soap); /* detach environment */
  free(soap);
  return 0;
}

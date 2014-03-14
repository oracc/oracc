#include "soapH.h" /* get the gSOAP-generated definitions */
#include "ocalc.nsmap" /* get the gSOAP-generated namespace bindings */
#include <math.h>  
int main()  
{ return soap_serve(soap_new()); /* call the request dispatcher */  
}  
int ocalc__add(struct soap *soap, double a, double b, double *result)  
{ *result = a + b;  
  return SOAP_OK;  
}  
int ocalc__sub(struct soap *soap, double a, double b, double *result)  
{ *result = a - b;  
  return SOAP_OK;  
}  
int ocalc__sqrt(struct soap *soap, double a, double *result)
{ if (a >= 0)  
  { *result = sqrt(a);
    return SOAP_OK;  
  }  
  else 
    return soap_sender_fault(soap, "Square root of negative value", "I can only take the square root of non-negative values");    
}

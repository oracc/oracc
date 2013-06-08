#include <string.h>
#include <stdio.h>
#include "ccgi-1.1/ccgi.h"

int
main(int argc, char **argv)
{
  CGI_varlist *vl;
  char *p;
  CGI_value *value;
  int i;
  vl = CGI_get_post(0, "./cgi-upload-XXXXXX");
  
  return 0;
}

#include <locale.h>
#include <stdio.h>
#include <string.h>
#include "atf.h"
int lnum,verbose;
const char *file="<stdin>";
int
main(int argc, char **argv)
{
  char buf[128];
  setlocale(LC_ALL,LOCALE/*"en_GB.utf8"*/);
  while (fgets(buf,127,stdin))
    {
      size_t len, i;
      wchar_t *wbufp;
      if (*buf && buf[strlen(buf)-1] == '\n')
	buf[strlen(buf)-1] = '\0';
      wbufp = utf2wcs((unsigned char*)buf, &len);
      for (i = 0; i < len; ++i,++wbufp)
	if (*wbufp > 127)
	  fprintf(stderr,"%04lx\n",(long int)*wbufp);
    }
  return 0;
}

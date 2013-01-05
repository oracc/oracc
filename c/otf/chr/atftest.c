#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atf.h"

extern const char *file;
extern int lnum;
int
main(int argc, char **argv)
{
  const unsigned char *atf = (const unsigned char *)"sza", 
    *atfback = NULL, *ATF=(const unsigned char *)"ŠA";
  const unsigned char *utf = NULL, *utfback = NULL;
  wchar_t *wdest = NULL;
  size_t len = 0;

  file = "f";
  lnum = 0;

  atf_init();
  fprintf(stderr,"locale: %s\n",setlocale(LC_ALL,NULL));
  fprintf(stderr,"atf: %s\n",atf);
  utf = atf2utf(atf,0);
  fprintf(stderr,"utf: %s\n",utf);
  wdest = utf2wcs(utf,&len);
  fprintf(stderr,"U+%04x\n",(int)*wdest);
  atfback = wcs2atf(wdest,len);
  fprintf(stderr,"atf': %s\n", atfback);
  utfback = wcs2utf(wdest,len);
  fprintf(stderr,"utf': %s\n", utfback);
  utfback = utf_lcase(ATF);
  fprintf(stderr,"UTF': %s=>%s\n", ATF, utfback);
  utfback = utf_ucase(utf);
  fprintf(stderr,"utf': %s=>%s\n", utf, utfback);
  fprintf(stderr,"atf': %s=>%s\n", utf, utf2atf(utf));
  atf_term();
  return 0;
}
const char *prog = "atftest";
const char *usage_string = "";
int major_version=1,minor_version = 0;
int opts() { return 0; }
void help() { }

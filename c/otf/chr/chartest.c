#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atf.h"
#include "inctrie.h"
#include "charsets.h"

extern FILE *f_log;

int
main(int argc, char **argv)
{
  const unsigned char *atf = (const unsigned char *)"szaf", 
    *atfback = NULL, *ATF=(const unsigned char *)"ŠA";
  const unsigned char *utf = NULL, *utfback = NULL;
  wchar_t *wdest = NULL;
  size_t len = 0;
  struct charset *cp;

  f_log = stderr;
  file = "#internal#";
  lnum = 1;

  charsets_init();
  cp = get_charset(c_sux,m_graphemic);
  chartrie_init(cp);

  setlocale(LC_ALL,LOCALE);
  fprintf(stderr,"locale: %s\n",setlocale(LC_ALL,NULL));
  fprintf(stderr,"atf: %s\n",atf);

  utf = inctrie_map(cp->to_uni, (const char *)atf, (const char *)atf+strlen((const char*)atf),0, CHARSET_ERR_FUNC, cp,file,lnum);
  fprintf(stderr,"utf: %s\n",utf);

  wdest = utf2wcs(utf,&len);
  fprintf(stderr,"U+%04x\n",(int)*wdest);

#if 1
  atfback = (unsigned char *)cp->asc(wdest,len);
#else
  atfback = wcs2atf(wdest,len);
#endif
  fprintf(stderr,"atf': %s\n", atfback);

  utfback = wcs2utf(wdest,len);
  fprintf(stderr,"utf': %s\n", utfback);

  utfback = utf_lcase(ATF);
  fprintf(stderr,"UTF': %s=>%s\n", ATF, utfback);

  utfback = utf_ucase(utf);
  fprintf(stderr,"utf': %s=>%s\n", utf, utfback);

  fprintf(stderr,"atf': %s=>%s\n", utf, utf2atf(utf));

  charsets_term();
  utf2wcs(NULL,0);
  wcs2atf(NULL,0);
  wcs2utf(NULL,0);
  return 0;
}
const char *prog = "chartest";
const char *usage_string = "";
int major_version=1,minor_version = 0;
int opts() { return 0; }
void help() { }

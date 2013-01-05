#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "atflocale.h"
#include "hash.h"
#include "runexpat.h"

Hash_table *signiary;
int verbose;

static void
sH(void *userData, const char *name, const char **atts)
{
  char *p = NULL;
  if ((p = strchr(name,'|')))
    {
      *p = '\0';
      printf("%s",name);
      exit(0);
    }
}

static void
eH(void *userData, const char *name)
{
}

int
main(int argc, char **argv)
{
  const char *f[2];
  setlocale(LC_ALL,LOCALE);
  f[0] = argv[1];
  f[1] = NULL;
  runexpatNS(i_list,f,sH,eH,"|");
  return 1;
}

const char *prog = "xmlns";
int major_version = 1, minor_version = 0;
const char *usage_string = "xmlns XMLFILE";
void help () { }
int opts(int arg,char*str){ return 1; }

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "atflocale.h"
#include "oracclocale.h"
#include "hash.h"
#include "runexpat.h"

int in_r;
int n_r = 0;
int n_xis = 0;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (strlen(name) == 1)
    in_r = 1;
  else if (!strcmp(name,"xis"))
    {
      const char *id = get_xml_id(atts);
      n_r = 0;
      if (n_xis++)
	printf(" , ");
      printf("\"%s\": [ ",id);
    }
  else if (!strcmp(name,"xisses"))
    {
      printf("\"instances\": {");
    }
  else if (!strcmp(name,"rp-wrap"))
    ;
  else
    {
      fprintf(stderr,"%s: bad .xis file: name=%s\n",prog, name);
      exit(1);
    }
}

static void
eH(void *userData, const char *name)
{
  if (in_r)
    {
      if (n_r > 0)
	printf(",");
      printf("\"%s\"", charData_retrieve());
      ++n_r;
      in_r = 0;
    }
  else
    {
      if (!strcmp(name,"xis"))
	printf(" ] \n");
      else if (!strcmp(name,"xisses"))
	printf("}");
      else if (!strcmp(name,"rp-wrap"))
	;
      else
	{
	  fprintf(stderr,"%s: bad .xis file\n",prog);
	  exit(1);
	}
    }
}

int
main(int argc, char **argv)
{
  const char *f[2];
  setlocale(LC_ALL,ORACC_LOCALE);
  f[0] = argv[1];
  f[1] = NULL;
  printf("{");
  runexpat(i_stdin,NULL,sH,eH);
  printf("}");
  return 1;
}

const char *prog = "xmlns";
int major_version = 1, minor_version = 0;
const char *usage_string = "xmlns XMLFILE";
void help () { }
int opts(int arg,char*str){ return 1; }
int verbose = 0;

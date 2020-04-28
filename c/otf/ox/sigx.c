#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#include "tokenizer.h"
#include "initterm.h"
#include "f2.h"

int
main(int argc, char **argv)
{
  struct f2 *f2p = malloc(sizeof(struct f2));
  char buf[4096];
  int line = 0;
  char *file = NULL;
  if (argc > 0)
    {
      file = argv[1];
      while (fgets(buf,BUFSIZ-1,stdin))
	{
	  ++line;
	  f2_parse((unsigned char*)file,line,(unsigned char *)buf,f2p,NULL,NULL);
	  fprintf(stdout, "%s %s\n", (char*)f2p->form, (char*)f2p->base);
	}
    }
  return 0;
}
const char *prog = "sigx";
const char *usage_string = "sigx <signatures from stdin>";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }
int verbose = 0;


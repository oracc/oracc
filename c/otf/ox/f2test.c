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
  if (argc > 1)
    f2_parse((unsigned char *)"<arg>",0,(unsigned char *)argv[1],f2p,NULL,NULL);
  return 0;
}
const char *prog = "f2test";
const char *usage_string = "f2test [SIG]";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }
/*int verbose = 0;*/


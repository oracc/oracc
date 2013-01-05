#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#include "tokenizer.h"
#include "initterm.h"

extern const char *file;

#if 0
static void
printer(char *string, void *data)
{
  fprintf(stderr,"%s:",string);
  print_token(data);
}

static void
showhash()
{
  hash_sorted_enum( &table, printer );
}
#endif

int
main(int argc, char **argv)
{
  unsigned char s[1024];

  tree_init();
  atf_init();
  charsets_init();
  gdl_init();
  
  file = "stdin";
  while (NULL != fgets((char*)s,1024,stdin))
    {
      tokenize(s,s);
      showtoks();
      tokenize_reinit();
    }
#if 0
  showhash();
#endif

  atf_term();
  charsets_term();
  gdl_term();
  tree_term();
  return 0;
}
const char *prog = "tok";
const char *usage_string = "tok < [FILE WITH INLINE DATA]";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }

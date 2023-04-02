#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "atf.h"
#include "atf.tab.h"

int atftrace = 1;
extern int gdl_unicode;

void
atf_init(void)
{
  cat_init();
  gdl_unicode = 0;
}

void
atf_term(void)
{
  cat_term();
}

struct catchunk *
atf_read(const char *file)
{
  if (file)
    {
      FILE *fp;
      if (!(fp = fopen(file, "r")))
	{
	  fprintf(stderr, "open failed on %s\n", file);
	  return NULL;
	}
      atf_lex_init(fp, file);
    }
  atf_init();
  return atfyacc();
}


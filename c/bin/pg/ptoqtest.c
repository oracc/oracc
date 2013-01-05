#include <stdio.h>
#include <ctype.h>
#include <gdbm.h>
#include <psd_base.h>
#include <hash.h>
#include "sortinfo.h"
#include "pg.h"

int l2 = 0;

extern unsigned char *loadfile(unsigned const char*,size_t*);
extern unsigned char *loadstdin(size_t*);
const char *listfile = NULL;

struct item *items = NULL;
int items_used = 0;

#include "pg_load.c"

const char *project = "dcclt";

int
main(int argc, char **argv)
{
  const char *dot;
  int nitems = 0;
  int with_word_id = 0;

  struct item *items = pg_load(&nitems);
  if ((dot = strchr((const char *)items[0].s,'.')) && strchr(dot+1,'.'))
    with_word_id = 1;
  printf("items in = %d; with_word_id = %d\n", nitems, with_word_id);
  if (dot)
    {
      items = prune_p_with_q(items, &nitems, with_word_id);
      printf("items out = %d\n", nitems);
    }
  return 0;
}

const char *prog = "ptoqtest";
const char *usage_string = "ptoqtest";
int opts() { return 0; }
void help() {}
int major_version=1,minor_version=0;

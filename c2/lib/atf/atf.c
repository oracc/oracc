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

void
atf_init(void)
{
  cat_init();
}

void
atf_term(void)
{
  cat_term();
}

struct catchunk *
atf_read(const char *file)
{
  atf_init();
  return atfyacc();
}


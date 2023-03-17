#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gdl.tab.h"
#include "gdl.h"

extern void gdl_wrapup_buffer(void);

Tree *
gdlparse_string(char *s)
{
  Tree *tp = tree_init();
  (void)tree_root(tp, "gdl:gdl", 1, NULL);
  gdl_setup_buffer(s);
  gdl_set_tree(tp);
  gdlparse();
  gdl_wrapup_buffer();
  return tp;
}

void
gdlparse_init(void)
{
  gdl_init();
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
  gdl_term();
}

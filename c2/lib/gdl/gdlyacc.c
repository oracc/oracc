#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gdl.tab.h"
#include "gdl.h"

extern void gdl_wrapup_buffer(void);

void
gdlparse_string(char *s)
{
  gdl_setup_buffer(s);
  gdlparse();
  gdl_wrapup_buffer();
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

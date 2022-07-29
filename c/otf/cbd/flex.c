#include "gx.h"
extern int yyparse(void);
struct cbd* curr_cbd;
struct entry*curr_entry;

void
flex(const char *file)
{
  curr_cbd = cbd_init();
  (void)yyparse();
}

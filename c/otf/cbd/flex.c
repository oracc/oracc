#include "gx.h"
extern int yyparse(void);
struct cbd* curr_cbd;
struct entry*curr_entry;

const char *efile = NULL;
extern void yyrestart(FILE*);
void
flex(const char *file)
{
  if (file)
    {
      FILE *fp;
      efile = file;
      if ((fp = xfopen(efile, "r")))
	yyrestart(fp);
    }
  curr_cbd = cbd_init();
  if (yyparse())
    {
      fprintf(stderr, "gx: exiting after parse errors\n");
      exit(1);
    }
}

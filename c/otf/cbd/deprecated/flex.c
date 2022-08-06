#include "gx.h"
extern int yyparse(void);
struct cbd* curr_cbd;
struct entry*curr_entry;

const char *efile = NULL;
extern void yyrestart(FILE*);
void
flex(const char *file)
{
  extern int parser_status;
  if (file)
    {
      FILE *fp;
      efile = file;
      if ((fp = xfopen(efile, "r")))
	yyrestart(fp);
    }
  curr_cbd = bld_cbd();
  phase = "syn";
  if (yyparse() || parser_status)
    {
      if (!keepgoing)
	{
	  msglist_print(stderr);
	  fprintf(stderr, "gx: exiting after syntax errors\n");
	  exit(1);
	}
    }
}

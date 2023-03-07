%option prefix="cat"
%option outfile="cat-l.c"
%option never-interactive noyywrap yylineno

%{
#include <npool.h>
#include "cat.tab.h"

#define yylloc catlloc
#define YY_USER_ACTION cat_user_action();

const char *efile = "<stdin>";
int flextrace = 0;
int tok;

extern struct npool *catpool;
#define catsave(s) (char*)npool_copy((const unsigned char*)(s),catpool);
static void cat_user_action(void);

%}

%%

^[+->]*\@.*/\n  { catlval.text=catsave(cattext); return TOK; }
\n([ \t]*\n)+	{ return PAR; }
^[ \t]+.+/\n    { catlval.text=catsave(cattext); return TAB; }
^#.*/\n		{ catlval.text=catsave(cattext); return CMT; }
^[^ \t\n].*\n	{ return BAD; }
\n		{ return EOL; }

%%

static void
cat_user_action(void)
{
  if (flextrace)
    fprintf(stderr, "%s:%d: flextrace: text=::%s::\n", efile, catlineno, cattext);
#if 0
  yylloc.file = efile;
  yylloc.first_line = yylloc.last_line = yylineno;
#endif
}

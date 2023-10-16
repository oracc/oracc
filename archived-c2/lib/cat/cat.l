%option prefix="cat"
%option outfile="cat-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <mesg.h>
#include <pool.h>

#define CATLTYPE Mloc
#include "cat.tab.h"
#define YY_USER_ACTION cat_user_action();
extern Mloc catlloc;
const char *catfile = "<stdin>";
const char *currcatfile = NULL;
int catflextrace = 0;

extern Pool *catpool;
#define catsave(s) (char*)pool_copy((const unsigned char*)(s),catpool);
static void cat_user_action(void);

%}

%%

^\@.*/\n  	{ catlval.text=catsave(cattext); return TOK; }
\n([ \t]*\n)+	{ return PAR; }
^[ \t]+.+/\n    { catlval.text=catsave(cattext); return TAB; }
^#.*/\n		{ catlval.text=catsave(cattext); return CMT; }
^[^ \t\n].*/\n	{ return BAD; }
\n		{ return EOL; }

%%

static void
cat_user_action(void)
{
  if (catflextrace)
    fprintf(stderr, "%s:%d: flextrace: text=::%s::\n", catfile, catlineno, cattext);
#if 0
  yylloc.file = catfile;
  yylloc.first_line = yylloc.last_line = yylineno;
#endif
}

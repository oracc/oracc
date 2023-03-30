%option prefix="atf"
%option outfile="atf-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <mesg.h>
#include <pool.h>

#define ATFLTYPE Mloc
#include "atf.tab.h"
#define YY_USER_ACTION atf_user_action();
extern Mloc atflloc;
const char *atffile = "<stdin>";

int atfflextrace = 0;

extern Pool *catpool;
#define atfsave(s) (char*)pool_copy((const unsigned char*)(s),catpool);
static void atf_user_action(void);

%}

%%

^[&@$].*/\n  		{ atflval.text=atfsave(atftext); return TOK; }
^#[^ \t:]+:[ \t].*/\n  	{ atflval.text=atfsave(atftext); return TOK; }
^[^ \t.:]+\.[ \t].*/\n 	{ atflval.text=atfsave(atftext); return TOK; }
^[^ \t.:]+:[ \t].*/\n 	{ atflval.text=atfsave(atftext); return TOK; }
^#.*/\n			{ atflval.text=atfsave(atftext); return CMT; }
^[ \t]+.+/\n    	{ atflval.text=atfsave(atftext); return TAB; }
^[^ \t\n].*/\n		{ return BAD; }
\n([ \t]*\n)+		{ return PAR; }
\n			{ return EOL; }

%%

static void
atf_user_action(void)
{
  if (atfflextrace)
    fprintf(stderr, "%s:%d: flextrace: text=::%s::\n", atffile, atflineno, atftext);
#if 0
  yylloc.file = atffile;
  yylloc.first_line = yylloc.last_line = yylineno;
#endif
}

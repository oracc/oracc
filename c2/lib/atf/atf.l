%option prefix="atf"
%option outfile="atf-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>

#define ATFLTYPE Mloc
#include "atf.tab.h"
#define YY_USER_ACTION atf_user_action();
extern Mloc atflloc;
const char *atffile = "<stdin>";
const char *curratffile = NULL;

int atfflextrace = 0;

extern Pool *catpool;
#define atfsave(s) (char*)pool_copy((const unsigned char*)(s),catpool);
static void atf_user_action(void);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

%}

BOM	("\xfe\xff")?
LINK	("<<"|">>"|"++"|"||")

%%

^{BOM}"&".*/\n         		{ atflval.text=atfsave(atftext); pmatch("###amp\n"); return TOK; }

^"@".*/\n  			{ atflval.text=atfsave(atftext); pmatch("###at\n"); return TOK; }

^"$".*/\n  			{ atflval.text=atfsave(atftext); pmatch("###dollar\n"); return TOK; }

^"#"[^ \t\n:]+:[ \t]+.*/\n  	{ atflval.text=atfsave(atftext); pmatch("###hash\n"); return TOK; }

^{LINK}.*/\n  			{ atflval.text=atfsave(atftext); pmatch("###link\n"); return TOK; }

^[^ \t\n.:]+\.([ \t]+.*)?/\n 	{ atflval.text=atfsave(atftext); pmatch("###line\n"); return TOK; }

^[^ \t\n.:]+:[ \t]+.*/\n 	{ atflval.text=atfsave(atftext); pmatch("###siglum\n"); return TOK; }

^\#.*/\n   			{ atflval.text=atfsave(atftext); pmatch("###comment\n"); return CMT; }

^[ \t]+.+/\n    		{ atflval.text=atfsave(atftext); pmatch("###tab\n"); return TAB; }

^[^ \t\n].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ pmatch("###par\n"); return PAR; }

\n				{ pmatch("###eol\n"); return EOL; }

.				{ mesg_vwarning(curratffile, atflineno,
						"atf.l: unmatched character %c\n", *atftext); }

%%

void
atf_lex_init(FILE *fp, const char *file)
{
#if 1
  static ssize_t fsize;
  char *buf = (char*)slurp("atf", file, &fsize);
#else
  YY_FLUSH_BUFFER;
  yyrestart(fp);
#endif
  atffile = curratffile = file;
  atflineno = 1;
#if 1
  /* slurp adds \n\0\0 but only counts \n in the returned fsize */
  atf_scan_buffer(buf,fsize+2);
#endif
}

static void
atf_user_action(void)
{
  if (atfflextrace)
    fprintf(stderr, "%s:%d: atflex: text=::%s::\n", curratffile, atflineno, atftext);

  atflloc.file = curratffile;
  atflloc.line = yylineno;
}

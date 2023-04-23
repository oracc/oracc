%option prefix="asl"
%option outfile="asl-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>

#define ASLLTYPE Mloc
#include "asl.tab.h"
#define YY_USER_ACTION asl_user_action();
extern Mloc asllloc;
const char *aslfile = "<stdin>";
const char *curraslfile = NULL;

int aslflextrace = 0;

extern Pool *catpool;
#define aslpool(s) (char*)pool_copy((const unsigned char*)(s),catpool);
static void asl_user_action(void);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define aslsave asllval.text=aslpool(asltext)

%}

GRAPHEME 	([a-zA-Z\x80-\xc2\xc4-\xe1\xe3-\xff]|\xc3[^\x97]|\xe2\xb8[^\xa2-\xa5]|\xe2[^\xb8])+

%s E
%s T

%%

<E>"sign"			{ aslsave; pmatch("###EFORM\n"); return ESIGN; }

<E>"form"			{ aslsave; pmatch("###EFORM\n"); return EFORM; }

<E>.+/\n			{ aslsave; pmatch("###EBAD\n"); return EBAD; }

<T>^[ \t]+.+/\n    		{ aslsave; pmatch("###TEXTCONT\n"); return TAB; }

<T>^[^ \t\n@].*/\n	       	{ aslsave; pmatch("###TEXTBAD\n"); return TEXT; }

<T>.*\n				{ aslsave; pmatch("###TEXT\n"); return TEXT; }

^[ \t]+.+/\n    		{ aslsave; pmatch("###tab\n"); return TAB; }

^"@" 	 			{ BEGIN 0; aslsave; pmatch("###at\n"); return asl_at_check(asllval.text); }

{GRAPHEME}	       		{ aslsave; return asl_grapheme(asllval.text); }

"~"[^ \t\n]+			{ aslsave; return VAR; }

"#"[^ \t\n]+			{ aslsave; return VCMT; }

"[".*/\n	       		{ aslsave; return VREF; }

^[^ \t\n].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ BEGIN 0; pmatch("###par\n"); return PAR; }

\n				{ pmatch("###eol\n"); return EOL; }

.				{ mesg_vwarning(curraslfile, asllineno,
						"asl.l: unmatched character %c\n", *asltext); }

%%

void
asl_lex_init(FILE *fp, const char *file)
{
#if 1
  static ssize_t fsize;
  char *buf = (char*)slurp("asl", file, &fsize);
#else
  YY_FLUSH_BUFFER;
  yyrestart(fp);
#endif
  aslfile = curraslfile = file;
  asllineno = 1;
#if 1
  /* slurp adds \n\0\0 but only counts \n in the returned fsize */
  asl_scan_buffer(buf,fsize+2);
#endif
}

static void
asl_user_action(void)
{
  if (aslflextrace)
    fprintf(stderr, "%s:%d: asllex: text=::%s::\n", curraslfile, asllineno, asltext);

  asllloc.file = curraslfile;
  asllloc.line = yylineno;
}

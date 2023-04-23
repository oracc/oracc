%option prefix="asl"
%option outfile="asl-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <oraccsys.h>
#include <mesg.h>
#include <pool.h>
#include "asl.h"

#define ASLLTYPE Mloc
#include "asl.tab.h"
#define YY_USER_ACTION asl_user_action();
extern Mloc asllloc;
const char *aslfile = "<stdin>";
const char *curraslfile = NULL;

int aslflextrace = 0;

extern Pool *aslpool;
#define xaslpool(s) (char*)pool_copy((const unsigned char*)(s),aslpool);
static void asl_user_action(void);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define aslsave asllval.text=xaslpool(asltext)

#define atcmd(at,st) aslsave; pmatch("###ATCMD\n"); BEGIN((st)); return(at)

%}

COMPOUND	"|"([a-zA-Z\x80-\xff]|[@().%&])+"|"
GRAPHEME 	([a-zA-Z\x80-\xc2\xc4-\xe1\xe3-\xff]|\xc3[^\x97]|\xe2\xb8[^\xa2-\xa5]|\xe2[^\xb8]|[@])+
LISTNUM		([A-Z]+[0-9]+[ab]?)

%s E
%s T

%%

<E>"sign"			{ atcmd(ESIGN,0); }

<E>"form"			{ atcmd(EFORM,0); }

<E>[^ \t]			{ aslsave; fprintf(stderr, "###EBAD %s\n", asltext); return EBAD; }

<T>^[ \t]+.+/\n    		{ BEGIN 0; aslsave; pmatch("###TEXTCONT\n"); return TAB; }

<T>^[^ \t\n@].*/\n	       	{ BEGIN 0; aslsave; pmatch("###TEXTBAD\n"); return TEXT; }

<T>.*/\n	       		{ BEGIN 0; aslsave; pmatch("###TEXT\n"); return TEXT; }

^[ \t]+.+/\n    		{ aslsave; pmatch("###tab\n"); return TAB; }

^@end/[ \t].*\n        	       	{ atcmd(END,E); }
^@sign/[ \t]       		{ atcmd(SIGN,0); }
^@nosign/[ \t]     		{ atcmd(NOSIGN,0); }
^@list/[ \t]       		{ atcmd(LIST,0); }
^@form/[ \t]	       		{ atcmd(FORM,0); }
^@noform/[ \t]         		{ atcmd(NOFORM,0); }
^@v/[ \t]	       		{ atcmd(V,0); }
^@nov/[ \t]	       		{ atcmd(NOV,0); }
^@v-/[ \t]	       		{ atcmd(NOV,0); }
^@v"?"/[ \t]	       		{ atcmd(QRYV,0); }
^@inote/[ \t]	       		{ atcmd(INOTE,T); }
^@ucode[ \t]+.*/\n		{ atcmd(UCODE,0); }
^@uname[ \t]+.*/\n		{ atcmd(UNAME,0); }
^@uphase[ \t]+.*/\n		{ atcmd(UPHASE,0); }

^@[a-z]+         		{ BEGIN 0; mesg_vwarning(curraslfile, asllineno,
							"[asl] malformed or unknown @-line: %s\n", asltext); }

{GRAPHEME}|{COMPOUND}          	{ aslsave; return asl_grapheme(asllval.text); }

{LISTNUM}   	 	      	{ aslsave; return LISTNUM; }

"~"[^ \t\n]+			{ aslsave; return VAR; }

^"#".*$				{ aslsave; return CMT; }

"#"[^ \t\n]+			{ aslsave; return VCMT; }

"[".*/\n	       		{ aslsave; return VREF; }

^[^ \t\n@].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ BEGIN 0; pmatch("###par\n"); return PAR; }

[ \t]+				{ }

\n				{ pmatch("###eol\n"); return EOL; }

.				{ mesg_vwarning(curraslfile, asllineno,
						"asl.l: unmatched character %c\n", *asltext); }

%%

static void
asl_user_action(void)
{
  if (aslflextrace)
    fprintf(stderr, "%s:%d: asllex: text=::%s::\n", curraslfile, asllineno, asltext);

  asllloc.file = curraslfile;
  asllloc.line = yylineno;
}

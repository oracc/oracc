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
int fake_flag = 0;

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

S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~]([a-z]+|90|180|270))*
N		(N[0-9]+)
COMPOUND	"|"([a-zA-Z\x80-\xff]|[@~().%&+()0-9])+"|"
GRAPHEME 	((({G}|{N}){M})|"15")
PHONGRAPH	"/"{G}"/"
LISTNUM		([A-Z]+[0-9]+[()A-Z0-9a-z_'"+*/]*){M}\??
NUMGRAPH	[0-9nN]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})\){M}
FRACTION	[0-9]\/[0-9]
PUNCT		("*"|:['".:]?|"/")

%s E
%s T

%%

<E>"sign"			{ fake_flag = 0; atcmd(ESIGN,0); }

<E>"form"			{ fake_flag = 0; atcmd(EFORM,0); }

<E>[^ \t]			{ fake_flag = 0; aslsave; fprintf(stderr, "###EBAD %s\n", asltext); return EBAD; }

<T>^[ \t]+.+/\n    		{ BEGIN 0; aslsave; pmatch("###TEXTCONT\n"); return TAB; }

<T>^[^ \t\n@].*/\n	       	{ BEGIN 0; aslsave; pmatch("###TEXTBAD\n"); return TEXT; }

<T>.*/\n	       		{ BEGIN 0; aslsave; pmatch("###TEXT\n"); return TEXT; }

^[ \t]+.+/\n    		{ aslsave; pmatch("###tab\n"); return TAB; }

^@end/[ \t].*\n        	       	{ atcmd(END,E); }
^@signlist/[ \t]       		{ atcmd(SIGNLIST,T); }
^@sign/[ \t]       		{ atcmd(SIGN,0); }
^@pname/[ \t]       		{ atcmd(PNAME,0); }
^@nosign/[ \t]     		{ atcmd(NOSIGN,0); }
^@list/[ \t]       		{ atcmd(LIST,0); }
^@form/[ \t]	       		{ atcmd(FORM,0); }
^@noform/[ \t]         		{ atcmd(NOFORM,0); }
^@v/[ \t]	       		{ atcmd(V,0); }
^@v-/[ \t]	       		{ atcmd(NOV,0); }
^@v"?"/[ \t]	       		{ atcmd(QRYV,0); }
^@inote/[ \t]	       		{ atcmd(INOTE,T); }
^@lit/[ \t]	       		{ atcmd(LIT,T); }
^@note/[ \t]*       		{ atcmd(NOTE,T); }
^@uchar[ \t]+.*/\n		{ atcmd(UCHAR,0); }
^@ucode[ \t]+.*/\n		{ atcmd(UCODE,0); }
^@uname[ \t]+.*/\n		{ atcmd(UNAME,0); }
^@unote/[ \t]	       		{ atcmd(UNOTE,T); }
^@uphase[ \t]+.*/\n		{ atcmd(UPHASE,0); }
^@fake[ \t]+1[ \t]*/\n		{ fake_flag = 1; }

^@[a-z]+         		{ BEGIN 0; mesg_vwarning(curraslfile, asllineno,
							"[asl] malformed or unknown @-line: %s\n", asltext); }


{NUMGRAPH}			{ aslsave; return asl_grapheme(asllval.text); }

{FRACTION}			{ aslsave; return GVALUE; }

{GRAPHEME}|{COMPOUND}          	{ aslsave; return asl_grapheme(asllval.text); }

{LISTNUM}   	 	      	{ aslsave; return LISTNUM; }

{PHONGRAPH}	       		{ aslsave; return GVALUE; }

{PUNCT}				{ aslsave; return GVALUE; }

[^ \t]+"[...]"[^ \t\n]+[ \t]*/\n { aslsave; return ATF; }
[^ \t]+"[...]"[ \t]*/\n       	{ aslsave; return ATF; }
"[...]"[^ \t\n]*[ \t]*/\n 	{ aslsave; return ATF; }
[^-_ \t]+[-_][^ \t\n]+		{ aslsave; return ATF; }
[^{} \t]*[{}][^ \t\n]*/\n      	{ aslsave; return ATF; }

"%"[-a-z0-9]+("/n")?   		{ aslsave; return LANG; }

"~"[^ \t\n]+			{ aslsave; return VAR; }

^"#".*$				{ aslsave; return CMT; }

"#"[^ \t\n]+			{ mesg_vwarning(curraslfile, asllineno,
						"asl.l: #comments no longer allowed in @v lines; use @inote or @v-\n"); }

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
#if 0
  if (aslflextrace)
    fprintf(stderr, "%s:%d: asllex: text=::%s::\n", curraslfile, asllineno, asltext);
#endif

  asllloc.file = curraslfile;
  asllloc.line = yylineno;
}

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

static void check_minus(char *t);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define aslsave asllval.text=xaslpool(asltext)

#define atcmd(at,st) aslsave; check_minus(asllval.text); pmatch("###ATCMD\n"); BEGIN((st)); return(at)

#define Tcmd(at,st) pmatch("###TCMD\n"); BEGIN((st)); return(at)

%}

S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~]([a-z]+|90|180|270))*
N		(N[0-9]+)
Q		\??
L		=?
COMPOUND	"|"([a-zA-Z\x80-\xff]|[@~().%&+()0-9])+"|"{Q}{L}
GRAPHEME 	((({G}|{N}){M})|"15"){Q}{L}
PHONGRAPH	"/"{G}"/"
LISTNUM		([A-Z]+[0-9]+[()A-Z0-9a-z_'"+*/]*){M}{Q}{L}
NUMGRAPH	[0-9nN]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})\){M}{Q}{L}
FRACTION	[0-9]\/[0-9]
PUNCT		("*"|:['".:]?|"/"){Q}{L}

%s E
%s S
%s T

%%

<E>"sign"			{ fake_flag = 0; atcmd(ESIGN,0); }

<E>"form"			{ fake_flag = 0; atcmd(EFORM,0); }

<E>[^ \t]			{ fake_flag = 0; aslsave; fprintf(stderr, "###EBAD %s\n", asltext); return EBAD; }

<S>[ \t]+			{ BEGIN T; }

<T>^[ \t]+.+/\n    		{ BEGIN 0; aslsave; pmatch("###TEXTCONT\n"); return TAB; }

<T>^[^ \t\n@].*/\n	       	{ BEGIN 0; aslsave; pmatch("###TEXTBAD\n"); return TEXT; }

<T>.*/\n	       		{ BEGIN 0; aslsave; pmatch("###TEXT\n"); return TEXT; }

^[ \t]+.+/\n    		{ aslsave; pmatch("###tab\n"); return TAB; }

^@@				{ /* no-op */ }
^@end/[ \t].*\n        	       	{ atcmd(END,E); }
^@signlist/[ \t]       		{ atcmd(SIGNLIST,T); }
^@sign-?/[ \t]       		{ atcmd(SIGN,0); }
^@aka/[ \t]       		{ atcmd(AKA,0); }
^@comp/[ \t]       		{ atcmd(COMP,0); }
^@pname/[ \t]       		{ atcmd(PNAME,0); }
^@nosign/[ \t]     		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@nosign' has been replaced by '@sign-'");
				  /*atcmd(SIGN,-1);*/ }
^@list-?/[ \t]       		{ atcmd(LIST,0); }
^@form-?/[ \t]	       		{ atcmd(FORM,0); }
^@noform/[ \t]         		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@noform' has been replaced by '@form-'");
				  /* atcmd(FORM,-1);*/ }
^@v-?/[ \t]+	       		{ atcmd(V,0); }
^@inote/[ \t]+       		{ Tcmd(INOTE,S); }
^@lit/[ \t]+	       		{ Tcmd(LIT,S); }
^@note/[ \t]+       		{ Tcmd(NOTE,S); }
^@uchar/[ \t]+			{ Tcmd(UCHAR,S); }
^@ucode/[ \t]+			{ Tcmd(UCODE,S); }
^@uname/[ \t]+			{ Tcmd(UNAME,S); }
^@unote/[ \t]+   		{ Tcmd(UNOTE,S); }
^@uphase/[ \t]+			{ Tcmd(UPHASE,S); }
^@fake[ \t]+1[ \t]*/\n		{ return FAKE; }

^@[a-z]+         		{ BEGIN 0; mesg_vwarning(curraslfile, asllineno,
							"[asl] malformed or unknown @-line: %s", asltext); }

{NUMGRAPH}			{ aslsave; return asl_grapheme(asllval.text); }

{FRACTION}			{ aslsave; mesg_vwarning(curraslfile, asllineno,
							 "[asl] unqualified FRACTION not allowed: %s", asltext);
				  return GVALUE; }

{GRAPHEME}|{COMPOUND}          	{ aslsave; return asl_grapheme(asllval.text); }

{LISTNUM}   	 	      	{ aslsave; return LISTNUM; }

{PHONGRAPH}	       		{ aslsave; return GVALUE; }

{PUNCT}				{ aslsave; return GVALUE; }

[^ \t]+"[...]"[^ \t\n]+[ \t]*/\n { aslsave; return ATF; }
[^ \t]+"[...]"[ \t]*/\n       	{ aslsave; return ATF; }
"[...]"[^ \t\n]*[ \t]*/\n 	{ aslsave; return ATF; }
[^-_ \t]+[-_\{\}][^ \t\n]+	{ aslsave; return ATF; }
[^{} \t]*[{}][^ \t\n]*/\n      	{ aslsave; return ATF; }

"%"[-a-z0-9]+("/n")?   		{ aslsave; return LANG; }

"~"[^ \t\n]+			{ mesg_vwarning(curraslfile, asllineno, "[asl] form variant tags like ~a are no longer used"); }

^"#".*$				{ aslsave; return CMT; }

"#"[^ \t\n]+			{ mesg_vwarning(curraslfile, asllineno,
						"[asl] #comments no longer allowed in @v lines; use @inote or @v-"); }

"[".*/\n	       		{ aslsave; return VREF; }

^[^ \t\n@].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ BEGIN 0; pmatch("###par\n"); return PAR; }

[ \t]+				{ }

\n				{ pmatch("###eol\n"); return EOL; }

\?				{ mesg_vwarning(curraslfile, asllineno,
						"[asl] misplaced '?'; say, e.g., @v ba?", *asltext); }

.				{ mesg_vwarning(curraslfile, asllineno,
						"[asl] unmatched character %c", *asltext); }

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

static void
check_minus(char *t)
{
  char *m = strrchr(t,'-');
  extern int minus_flag;
  if (m && !m[1])
    {
      if (strcmp(t, "@sign-") && strcmp(t, "@form-") && strcmp(t, "@list-") && strcmp(t, "@v-"))
	mesg_vwarning(curraslfile, asllineno, "[asl] '-' not allowed on %s (only on @sign/@form/@list/@v)", *asltext);
      else
        minus_flag = 1;
    }
  else
    minus_flag = 0;
}

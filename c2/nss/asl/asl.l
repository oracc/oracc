%option prefix="asl"
%option outfile="asl-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <ctype.h>
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

int endofinput = 0;
int aslflextrace = 0;
int fake_flag = 0;
int Ynext = 0;

extern Pool *aslpool;
#define xaslpool(s) (char*)pool_copy((const unsigned char*)(s),aslpool);
static void asl_user_action(void);
static int asl_grapheme(const char *gp);
static void check_minus(char *t);
static void strip_literal(char *t);

/* #if 1 to print every matched rule */
#if 0
#define pmatch(m) fprintf(stderr, (m))
#else
#define pmatch(m)
#endif

#define aslsave  asllval.text=xaslpool(asltext)

#define atcmd(at,st) aslsave; check_minus(asllval.text); pmatch("###ATCMD\n"); BEGIN((st)); return(at)

#define Tcmd(at,st) pmatch("###TCMD\n"); BEGIN((st)); return(at)

#define no_comments(where) mesg_vwarning( \
					  curraslfile, asllineno, \
					  "[asl] #comments no longer allowed; use @inote, @v- or the like")

%}

S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~]([a-z]+[0-9]*|90|180|270))*
N		(N[0-9]+[ABC]?)
Q		\??
L		=?
COMPOUND	"|"([a-zA-Z\x80-\xff]|[@~().%&+()0-9])+"|"{M}{Q}{L}
GRAPHEME 	((({G}|{N}){M})|"15"){Q}{L}
ZERO		"00"(~[a-d])?
PHONGRAPH	"/"{G}"/"
LISTNAME	(U\+|[A-Z]+)
LISTNUM		((U\+|[A-Z]+)[0-9]+[()A-Z0-9a-z_'"+*/]*){M}{Q}{L}
NUMGRAPH	[0-9XnN]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})\){M}{Q}{L}
FRACTION	[0-9]\/[0-9]
PUNCT		("*"|:['".:]?|"/"){Q}{L}
SYSNAME		[a-zA-Z]+

%s E
%s L
%s S
%s T
%s Y
%x X

%%

<E>"sign"			{ fake_flag = 0; atcmd(ESIGN,0); }

<E>"form"			{ fake_flag = 0; atcmd(EFORM,0); }

<E>[^ \t]			{ fake_flag = 0; aslsave; fprintf(stderr, "###EBAD %s\n", asltext); return EBAD; }

<L>{LISTNAME}/[ \t]+         	{ aslsave; BEGIN(S); return LISTNAME; }

<S>[ \t]+			{ BEGIN T; }

<T>^"#".*/\n   			{ BEGIN 0; no_comments("text"); }
<T>^"#".*/\n@   	       	{ BEGIN 0; no_comments("text"); }
<T>^[ \t]+.*/\n@          	{ BEGIN 0; aslsave; return TAB; }
<T>^[ \t]+.*/\n			{ aslsave; return TAB; }
<T>.+/\n@			{ BEGIN 0; aslsave; return TEXT; }
<T>.+/\n			{ aslsave; return TEXT; }
<T>\n				{ }
<T><<EOF>>			{ if (!endofinput++) return EOL; else yyterminate(); }

<X>([^"\n]|\\\")*\"?[ \t]*/\n   { BEGIN 0; aslsave; strip_literal(asllval.text); return LITERAL; }
\"				{ BEGIN X; }

<Y>{SYSNAME}/\n         	{ aslsave; BEGIN(Ynext); return SYSNAME; }
<Y>{SYSNAME}/[ \t]+         	{ aslsave; BEGIN(Ynext); return SYSNAME; }

^@@				{ /* no-op */ }
^@end/[ \t].*\n        	       	{ atcmd(END,E); }
^@signlist/[ \t]+      		{ atcmd(SIGNLIST,S); }
^@listdef/[ \t]+       		{ atcmd(LISTDEF,L); }
^@sysdef/[ \t]+       		{ Ynext=S; atcmd(SYSDEF,Y); }
^@sign-?/[ \t]       		{ atcmd(SIGN,0); }
^@aka/[ \t]       		{ atcmd(AKA,0); }
^@smap/[ \t]+			{ atcmd(SMAP,0); }
^@compoundonly/[ \t]           	{ atcmd(COMP,0); }
^@pname/[ \t]       		{ atcmd(PNAME,0); }
^@nosign/[ \t]     		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@nosign' has been replaced by '@sign-'");
				  /*atcmd(SIGN,-1);*/ }
^@list-?/[ \t]       		{ atcmd(LIST,0); }
^@form-?/[ \t]	       		{ atcmd(FORM,0); }
^@noform/[ \t]         		{ mesg_vwarning(curraslfile, asllineno, "[asl] '@noform' has been replaced by '@form-'");
				  /* atcmd(FORM,-1);*/ }
^@v-?/[ \t]+	       		{ atcmd(V,0); }
@sys/[ \t]+			{ Ynext=0; atcmd(SYS,Y); }
^@inote/[ \t]+       		{ Tcmd(INOTE,S); }
^@lit/[ \t]+	       		{ Tcmd(LIT,S); }
^@note/[ \t]+       		{ Tcmd(NOTE,S); }
^@ref/[ \t]+	       		{ Tcmd(REF,S); }
^@ucun/[ \t]+			{ Tcmd(UTF8,S); }
^@umap/[ \t]+			{ Tcmd(UMAP,S); }
^@uhex/[ \t]+			{ mesg_vwarning(curraslfile, asllineno, "[asl] '@uhex' should not appear in a signlist; say @list U+HHHHH"); }
^@useq/[ \t]+     		{ Tcmd(USEQ,S); }
^@uname/[ \t]+			{ Tcmd(UNAME,S); }
^@unote/[ \t]+   		{ Tcmd(UNOTE,S); }
^@uage/[ \t]+			{ Tcmd(UAGE,S); }
^@upua/[ \t]+			{ Tcmd(UPUA,S); }
^@fake[ \t]+1[ \t]*/\n		{ return FAKE; }
^@lref/[ \t]+			{ atcmd(LREF,0); }
^@sref/[ \t]+			{ atcmd(SREF,0); }

^@[a-z]+         		{ BEGIN 0; mesg_vwarning(curraslfile, asllineno,
							"[asl] malformed or unknown @-line: %s", asltext); }

{ZERO}      			{ aslsave; return GNAME; }

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

^"#".*/\n	       		{ no_comments("@v"); }

^[^ \t\n@].*/\n			{ pmatch("###bad\n"); return BAD; }

\n([ \t]*)+/\n			{ BEGIN 0; pmatch("###par\n"); return PAR; }

"=>"				{ return GOESTO; }

[ \t]+				{ }

\n				{ pmatch("###eol\n"); return EOL; }

\?				{ mesg_vwarning(curraslfile, asllineno,
						"[asl] misplaced '?'; say, e.g., @v ba?", *asltext); }

.				{ if (mesg_last_line()!=asllineno)
				    mesg_vwarning(curraslfile, asllineno,
						  "[asl] unmatched character %c", *asltext); }

<<EOF>>				{ if (!endofinput++) return EOL; else yyterminate(); }

%%

static int
asl_grapheme(const char *gp)
{
  const char *gp_orig = gp;
  while (*gp)
    if (*gp <= 127 && islower(*gp))
      return GVALUE;
    else if (*gp <= 127 && isupper(*gp))
      return GNAME;
    else
      ++gp;
  if (!strcmp(gp_orig, "15") || ('0' == gp[0] && '0' == gp[1]))
    return GNAME;
  else
    return GVALUE;
}

static void
asl_user_action(void)
{
#if 1
  if (asl_flex_debug)
    fprintf(stderr, "%s:%d: asllex: text=::%s::; start=%d\n", curraslfile, asllineno, asltext,yy_start);
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

static void
strip_literal(char *t)
{
  char *e = t + strlen(t);
  while (e > t && isspace(e[-1]))
    --e;
  if (e > t && e[-1] == '"')
    e[-1] = '\0';
  else
    {
      *e = '\0';
      mesg_vwarning(curraslfile, asllineno, "[asl] missing closing \"-quote on LITERAL");
    }
}

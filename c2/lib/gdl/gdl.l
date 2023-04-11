%option prefix="gdl"
%option outfile="gdl-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <ctype.h>
#include <mesg.h>
#include <pool.h>

#define GDLLTYPE Mloc
#include "gdl.tab.h"
#define YY_USER_ACTION gdl_user_action();
extern Mloc gdllloc;
const char *gdlfile = "<stdin>";
const char *currgdlfile = NULL;
extern int gdl_lexfld[];
extern int curr_lang;
extern int gdl_unicode;
int gdlflextrace = 0;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);
static int gdl_grapheme_type(void);
static int CIN = 0; /* 1 if in compound */ /* update: this is probably no longer necessary */
static int GIN = 0; /* 1 if in grapheme */
static int QIN = 0; /* non-zero if in a(BA) qualifier */

#define gconst gdllval.text=gdltext
#define gsave  gdllval.text=gdlsave(gdltext)
int term = 0;
#define yyterminate() return (term = !term) ? END : YY_NULL

/* Simple stack for nesting of compounds and valuquals */
#define CVQ_MAX 6
static int cvq_stack[CVQ_MAX] = { 0, 0, 0, 0, 0, 0 };
static int cvq_top = -1;
static int cvq_pop(void);
static int cvq_push(int);

%}

GRAPHEME	([a-zA-Z\x80-\xc2\xc4-\xff]|\xc3[^\x97])+([@~][a-z0-9]+)*
G		([a-zA-Z\x80-\xc2\xc4-\xe1\xe3-\xff]|\xc3[^\x97]|\xe2[^\xb8]|\xe2\xb8[^\xa2-\xa5])
B		("["|"]"|\xe2\xb8[\xa2-\xa5])
MOD		([@~][a-z0-9]+)*

L_UHS		"\xe2\xb8\xa2"
R_UHS		"\xe2\xb8\xa3"
L_LHS		"\xe2\xb8\xa4"
R_LHS		"\xe2\xb8\xa5"

LISTNUM		[A-Z]+[0-9]+[a-z]*([@~][a-z0-9]+)*

%s C
%s Q
%x TC
%x TD

%%

<C>"|"/[ \t]   		{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"|"			{ BEGIN cvq_pop(); CIN=0; return C_C; }
"|"/[ \t]	       	{ /*"*/ return gdl_lexfld[(int)*yytext]; }
"|"			{ BEGIN cvq_push(C); GIN=0; CIN=1; return C_O; }

[=^][aeiuAEIU]		{ if (gdl_unicode) { mesg_vwarning(currgdlfile,gdllineno,
						"non-Unicode accent codes %s",gdltext); }}

<C>"."			{ GIN=0; return C_PERIOD; }
<C>"+"			{ GIN=0; return C_PLUS; }
<C>":"/"("	       	{ GIN=0; return C_COLON; }
<C>":"			{ GIN=0; return C_COLON; }
<C>"&"			{ GIN=0; return C_ABOVE; }
<C>"%"			{ GIN=0; return C_CROSSING; }
<C>@/[^a-z0-9]		{ GIN=0; return C_OPPOSING; }
<C>"4\xc3\x97"		{ GIN=0; return C_4TIMES; }
<C>"3\xc3\x97"		{ GIN=0; return C_3TIMES; }
<C>"\xc3\x97"	       	{ GIN=0; return C_TIMES; }
<C>"("			{ return GIN ? QLP : CLP; }
<C>")"			{ return CRP; }

"\xc3\x97"	       	{ return C_TIMES; }

"<("			{ gconst; return L_ang_par; }

<Q>")"/">"	       	{ return QRP; }
<Q>")"			{ return QRP; }

")>"			{ gconst; return R_ang_par; }

"{{"			{ gconst; return L_dbl_cur; }
"}}"			{ gconst; return R_dbl_cur; }
"<<"			{ gconst; return L_dbl_ang; }
">>"			{ gconst; return R_dbl_ang; }
"<"			{ gconst; return '<'; }
">"			{ gconst; return '>'; }
"("			{ gconst; return GIN ? QLP : yytext[0]; }
")"			{ gconst; return QIN ? QRP : yytext[0]; }
"["			{ gconst; return yytext[0]; }
"]"			{ gconst; return yytext[0]; }
"{"			{ gconst; return yytext[0]; }
"}"			{ gconst; return yytext[0]; }
{L_UHS}			{ gsave; return L_uhs; }
{R_UHS}			{ gsave; return R_uhs; }
{L_LHS}			{ gsave; return L_lhs; }
{R_LHS}			{ gsave; return R_lhs; }

%[-a-z0-9]+		{ gsave; return LANG; }
_			{ gsave; return LANG_FLIP; }
,			{ return ','; }
&			{ return '&'; }
![a-z]+			{ return FTYPE; }

("*"|:['".:]?|\/)/[\]>#*!? \t\n(]	{ GIN=1; gsave; return PUNCT; }

[0-9]+(\/[0-9]+)?/"("	{ GIN=1; gsave; return NUMBER; }

[0-9]+(\/[0-9]+)?	{ GIN=1; gsave; return BARENUM; }

{LISTNUM}		{ GIN=1; gsave; return LISTNUM; }

({G}({G}|{B})*({G}|{MOD}+))	{ GIN=1; return gdl_grapheme_type(); }

{GRAPHEME} 		{ GIN=1; return gdl_grapheme_type(); }

{MOD}			{ GIN=1; gsave; return MOD; }

"#"			{ return GIN ? *yytext : gdl_lexfld[(int)*yytext]; }

[!?]			{ return *yytext; }

["~=@^]/[ \t]          	{ /*"*/ return gdl_lexfld[(int)*yytext]; }

"..."			{ GIN=0; gsave; return ELLIPSIS; }
"--"			{ GIN=0; return ENHYPHEN; }
[-+:.]     		{ GIN=0; return gdltext[0]; }

";"			{ return ';'; }
"/"			{ return '/'; }

[ \t]+			{ GIN=0; return SPACE; }

\n			{ return gdltext[0]; }

"($"			{ BEGIN TD; return L_inl_dol; }
<TD>"$)"	       	{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN TC; return L_inl_cmt; }
<TC>"#)"       		{ BEGIN 0; return R_inl_cmt; }

<TD>.+/"$)"   		{ gsave; return TEXT; }
<TC>.+/"#)"   		{ gsave; return TEXT; }

.			{ mesg_vwarning(currgdlfile, gdllineno, "gdl.l: unmatched character %c\n", *gdltext); }

<<EOF>>			{ yyterminate(); }

%%

int
gdl_grapheme_type(void)
{
  gsave;
  if (strlen(gdllval.text) == 1
      && ('n' == *gdllval.text || 'N' == *gdllval.text))
    return NUMBER;
  else
    return GRAPHEME;
}

void
gdl_setup_buffer(char *buf)
{
  (void)gdl_scan_string(buf);
}

void
gdl_wrapup_buffer(void)
{
  gdl_delete_buffer(YY_CURRENT_BUFFER);
}

static void
gdl_user_action(void)
{
  if (gdlflextrace)
    fprintf(stderr, "%s:%d: gdllex: YY_START=%d; text=::%s:: GIN=%d; CIN=%d\n",
		  gdlfile, gdllineno, YY_START, gdltext, GIN, CIN);

  gdllloc.file = currgdlfile;
  gdllloc.line = yylineno;
}

void
gdl_lex_init(const char *file, int line)
{
  CIN = GIN = QIN = 0;
  cvq_top = -1;
  currgdlfile = gdlfile = file;
  gdllineno = line;
  curr_lang = 's';
}

void
gdl_decr_qin(void)
{
  if (--QIN < 0)
    { /* underflow condition */
      QIN = 0;
    }
  else
    {
      BEGIN cvq_pop();
    }
  if (gdlflextrace)
    fprintf(stderr, "gdl_decr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

void
gdl_incr_qin(void)
{
  ++QIN;
  BEGIN cvq_push(Q);
  if (gdlflextrace)
    fprintf(stderr, "gdl_incr_qin: QIN=%d; new state=%d\n", QIN, YY_START);
}

int
cvq_push(int s)
{
  if (++cvq_top < CVQ_MAX)
    return cvq_stack[cvq_top] = s;
  else
    fprintf(stderr, "cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[--cvq_top];
  else
    fprintf(stderr, "cvq_pop: cvq stack underflow\n");
   return 0;
}

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
extern int gdl_bilingual, gdl_legacy, gdl_lexical, gdl_unicode;
int gdlflextrace = 0;
int g_literal_flag = 0, g_logoforce_flag = 0;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);
static int gdl_grapheme_type(void);
static int CIN = 0; /* 1 if in compound */ /* update: this is probably no longer necessary */
static int GIN = 0; /* 1 if in grapheme */
static int QIN = 0; /* non-zero if in a(BA) qualifier */
static int SIN = 0; /* left context is space */

#define gconst gdllval.text=gdltext
#define gsave  gdllval.text=gdlsave(gdltext)
#define gsaveg(garg)  gdllval.text=gdlsave((garg))
int term = 0;
#define yyterminate() return (term = !term) ? END : YY_NULL

static void gdl_closers(void);

/* Simple stack for nesting of compounds and valuquals */
#define CVQ_MAX 6
static int cvq_stack[CVQ_MAX] = { 0, 0, 0, 0, 0, 0 };
static int cvq_top = -1;
static int cvq_pop(void);
static int cvq_push(int);
static void cvq_reset(void);
%}

B		("["|"]"|\xe2\xb8[\xa2-\xa5])
S		\xe2\xb8[^\xa2-\xa5]
G		([a-zA-Z'\x80-\xc2\xc4-\xe1\xe3-\xff]|{S}|\xc3[^\x97]|\xe2[^\xb8])+
M		([@~\\]([a-z]+[0-9]*|90|180|270))
N		([N][0-9][0-9]?[ACF]?)
GRAPHEME 	((({G}|{N}))|"15")
LISTNUM		([A-Z][A-Z]+[0-9]+[A-Z0-9a-z_'"+*/]*)
NUMGRAPH	[0-9XNn]+(\/[0-9])?\(({GRAPHEME}|{COMPOUND})\)
FRACTION	[0-9]\/[0-9]

L_UHS		"\xe2\xb8\xa2"
R_UHS		"\xe2\xb8\xa3"
L_LHS		"\xe2\xb8\xa4"
R_LHS		"\xe2\xb8\xa5"

NOTEMARK	"^"[0-9]+"^"

%s C
%s Q
%s S
%x TC
%x TD

%%

<C>"|"/[ \t]   		{ BEGIN cvq_pop(); CIN=0; return C_C; }
<C>"|"			{ BEGIN cvq_pop(); CIN=0; return C_C; }
"|"/[ \t]	       	{ /*"*/ gsave; return gdl_lexfld[(int)*yytext]; }
"|"			{ BEGIN cvq_push(C); GIN=0; CIN=1; return C_O; }

[=^][aeiuAEIU]		{ if (gdl_unicode) { mesg_vwarning(currgdlfile,gdllineno,
						"non-Unicode accent codes %s",gdltext); }}

<C>"."			{ GIN=0; return C_PERIOD; }
<C>"+"			{ GIN=0; return C_PLUS; }
<C>":"/"("	       	{ GIN=0; return C_COLON; }
<C>":"			{ GIN=0; return C_COLON; }
<C>"&"			{ GIN=0; return C_ABOVE; }
<C>"%"			{ GIN=0; return C_CROSSING; }
<C>@/[^a-z0-9]+		{ GIN=0; return C_OPPOSING; }
<C>"4\xc3\x97"		{ GIN=0; return C_4TIMES; }
<C>"3\xc3\x97"		{ GIN=0; return C_3TIMES; }
<C>"\xc3\x97"	       	{ GIN=0; return C_TIMES; }
<C>"("			{ return GIN ? QLP : CLP; }
<C>")"			{ return CRP; }
<C>[ \t]+	       	{ CIN=GIN=0; mesg_vwarning(currgdlfile,gdllineno,"missing '|' in compound");
			  cvq_reset(); }

"\xc3\x97"	       	{ return C_TIMES; }

<Q>")"/[}>]	       	{ return QRP; }
<Q>")"			{ return QRP; }
<Q>[ \t]+	       	{ QIN=GIN=0; mesg_vwarning(currgdlfile,gdllineno,"missing ')' in qv");
			  cvq_reset(); }

"<("			{ gconst; GIN=0; return L_ang_par; }
")>"			{ gconst; GIN=0; return R_ang_par; }
"{{"[0-9+]:	       	{ gconst; GIN=0; return L_dbl_cur; }
"{{"			{ gconst; GIN=0; return L_dbl_cur; }
"}}"			{ gconst; GIN=0; return R_dbl_cur; }
"{("			{ gconst; GIN=0; return L_cur_par; }
")}"			{ gconst; GIN=0; return R_cur_par; }
"<<"			{ gconst; GIN=0; return L_dbl_ang; }
">>"			{ gconst; GIN=0; return R_dbl_ang; }
"("			{ gconst; if (GIN) { GIN=0; return QLP; } else { return yytext[0]; } }
")"			{ gconst; GIN=0; return yytext[0]; }
"<"			{ gconst; GIN=0; return '<'; }
">"			{ gconst; GIN=0; return '>'; }
"["			{ gconst; GIN=0; return yytext[0]; }
"]"			{ gconst; GIN=0; return yytext[0]; }
"{"			{ gconst; GIN=0; return yytext[0]; }
"{+"			{ gconst; GIN=0; return DET_PHON; }
"{-"			{ gconst; GIN=0; return DET_SEME; }
"}"			{ gconst; GIN=0; return yytext[0]; }

{L_UHS}			{ gsave; GIN=0; return L_uhs; }
{R_UHS}			{ gsave; GIN=0; return R_uhs; }
{L_LHS}			{ gsave; GIN=0; return L_lhs; }
{R_LHS}			{ gsave; GIN=0; return R_lhs; }

%[-a-z0-9]+("/"[gn])?		{ gsave; return LANG; }
_			{ gsave; return LANG_FLIP; }
,			{ return ','; }
&[0-9]+			{ gsave; return CELLSPAN; }
&			{ return '&'; }
![a-z]+			{ gsave; return FTYPE; }

{NOTEMARK}		{ gsave; return NOTEMARK; }

[!?]			{ gsave; return *yytext; }
^[#"~=@^]	       	{ gsave; return gdl_lexfld[(int)*yytext]; }
[#"~=@^]   		{ gsave; if (SIN) { SIN=0; return gdl_lexfld[(int)*yytext]; } else { return *yytext; } }
"*"			{ /*"*/ gsave; if (GIN) { return(*yytext); } else { GIN=1; return(PUNCT); } }

":"/[ \t\n\(]  		{ /*"*/ GIN=1; gsave; return PUNCT; }
":."			{ /*"*/ GIN=1; gsave; return PUNCT; }
":'"			{ /*"*/ GIN=1; gsave; return PUNCT; }
":\""			{ /*"*/ GIN=1; gsave; return PUNCT; }
"::"      		{ /*"*/ GIN=1; gsave; return PUNCT; }
"/"/[ \t\n\(]		{ /*"*/ GIN=1; gsave; return PUNCT; }

[0-9]+(\/[0-9]+)?/"("	{ GIN=1; gsave; return NUMBER; }

[0-9]+(\/[0-9]+)?	{ GIN=1; gsave; return BARENUM; }

{G}({G}|{B})*{G}	{ GIN=1; return gdl_grapheme_type(); }
{G}({G}|{B})*/{M}	{ GIN=1; return gdl_grapheme_type(); }
{GRAPHEME} 		{ GIN=1; return gdl_grapheme_type(); }

{M}			{ GIN=1; gsave; return MOD; }

{LISTNUM}		{ GIN=1; gsave; return LISTNUM; }

[#"=@^]/{G} 		{ gsave;
			  if (gdl_lexical) {
		       	    mesg_vwarning(currgdlfile, gdllineno,
			  	       	  "no space after %c field\n", *yytext);
			    return gdl_lexfld[(int)*yytext]; }
			  else { return *yytext; } }

"..."			{ GIN=0; gsave; return ELLIPSIS; }
"--"			{ GIN=0; return ENHYPHEN; }
[-+:.]     		{ GIN=0; return gdltext[0]; }

"/"/{G}			{ return '/'; }

~~/{G}			{ g_logoforce_flag = 1; }
"~"/{G}			{ if (gdl_lexical) { gsave; mesg_vwarning(currgdlfile, gdllineno,
			   "no space after ~ field; use ~~ for logogram force with atf: use lexical\n");
			    return gdl_lexfld[(int)*yytext];
			   } else { g_logoforce_flag = 1; } }
"$"/{G}			{ g_literal_flag = 1; }

";"			{ return INDENT; }
"//"			{ return NEWLINE; }

[ \t]+/[#"~=@^]	       	{ GIN=0; SIN=1; gsave; return SPACE; }
[ \t]+			{ GIN=0; gsave; return SPACE; }

\n			{ gdl_closers(); return gdltext[0]; }

"($"			{ BEGIN TD; return L_inl_dol; }
<TD>"$)"	       	{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN TC; return L_inl_cmt; }
<TC>"#)"       		{ BEGIN 0; return R_inl_cmt; }

<TD>.+/"$)"   		{ gsave; return TEXT; }
<TC>.+/"#)"   		{ gsave; return TEXT; }

.			{ mesg_vwarning(currgdlfile, gdllineno, "gdl.l: unmatched character %c\n", *gdltext); }

<<EOF>>			{ gdl_closers(); yyterminate(); }

%%

static void
gdl_closers(void)
{
  if (CIN)
    mesg_vwarning(currgdlfile,gdllineno,"missing '|' in compound (discovered at end of line)");
  else if (QIN)
    mesg_vwarning(currgdlfile,gdllineno,"missing ')' in qv (discovered at end of line)");
  GIN=CIN=QIN=0;
  cvq_reset();
}

int
gdl_grapheme_type(void)
{
  char *g = gdltext;
  gsaveg(g);

  if (strlen(g) == 1
      && ('n' == *g || 'N' == *g))
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
    fprintf(stderr, "%s:%d: gdllex: YY_START=%d; text=::%s:: GIN=%d; CIN=%d; QIN=%d; SIN=%d\n",
		  gdlfile, gdllineno, YY_START, gdltext, GIN, CIN, QIN, SIN);

#if 1
 /*  SIN=0; */
#else
  if (SIN)
    {
      BEGIN cvq_push(S);
      SIN=0;
    }
  else
    BEGIN cvq_pop();
#endif

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
      BEGIN 0;
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
    {
      cvq_stack[cvq_top] = YY_START;
      return s;
    }
  else
    fprintf(stderr, "%s:%d: cvq_push: cvq stack overflow (CVQ_MAX=%d)\n", currgdlfile, gdllineno, CVQ_MAX);
  return 0;
}

int
cvq_pop(void)
{
  if (cvq_top >= 0)
    return cvq_stack[cvq_top--];
  else
    fprintf(stderr, "%s:%d: cvq_pop: cvq stack underflow\n", currgdlfile, gdllineno);
   return 0;
}

void
cvq_reset(void)
{
  cvq_top = -1;
  BEGIN 0;
}

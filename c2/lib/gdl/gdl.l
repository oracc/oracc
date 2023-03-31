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
int gdlflextrace = 0;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);
static int gdl_grapheme_type(void);
static int CIN = 0; /* 1 if in compound */
static int GIN = 0; /* 1 if in grapheme */

#define gconst gdllval.text=gdltext

%}

FLAG		[#*!?]
GRAPHEME	[a-zA-Z\x80-\xc2\xc4-\xff]+([@~][a-z0-9]+)*

%s C
%x T

%%

<C>"|"			{ BEGIN 0; CIN=0; return C_C; }
"|"			{ BEGIN C; CIN=1; return C_O; }

<C>"."			{ return C_PERIOD; }
<C>"+"			{ return C_PLUS; }
<C>":"			{ return C_COLON; }
<C>"&"			{ return C_ABOVE; }
<C>"%"			{ return C_CROSSING; }
<C>@/[^a-z0-9]		{ return C_OPPOSING; }
<C>"4\xc3\x97"		{ return C_4TIMES; }
<C>"3\xc3\x97"		{ return C_3TIMES; }
<C>"\xc3\x97"	       	{ return C_TIMES; }
<C>"("			{ return '('; }
<C>")"			{ return ')'; }

"\xc3\x97"	       	{ return C_TIMES; }

"<<"			{ gconst; return L_dbl_ang; }
">>"			{ gconst; return R_dbl_ang; }
"<"			{ gconst; return '<'; }
">"			{ gconst; return '>'; }
"<("			{ gconst; return L_ang_par; }
")>"			{ gconst; return R_ang_par; }
"{{"			{ gconst; return L_dbl_cur; }
"}}"			{ gconst; return R_dbl_cur; }
"("			{ gconst; return CIN ? CLP : (GIN ? QLP : yytext[0]); }
")"			{ gconst; return CIN ? CRP : (GIN ? QRP : yytext[0]); }
"["			{ gconst; return yytext[0]; }
"]"			{ gconst; return yytext[0]; }
"{"			{ gconst; return yytext[0]; }
"}"			{ gconst; return yytext[0]; }
L_UHS			{ return L_uhs; }
R_UHS			{ return R_uhs; }
L_LHS			{ return L_lhs; }
R_LHS			{ return R_lhs; }

%[-a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return LANG; }
,			{ return ','; }
&			{ return '&'; }
![a-z]+			{ return FTYPE; }

"*"|:['".:]?|\/        	{ gdllval.text=gdlsave(gdltext); return PUNCT; }

[0-9]+(\/[0-9]+)?	{ GIN=1; gdllval.text=gdlsave(gdltext); return NUMBER; }

[A-Z]+[0-9]+[a-z]*	{ GIN=1; gdllval.text=gdlsave(gdltext); return LISTNUM; }

{GRAPHEME} 		{ GIN=1; return gdl_grapheme_type(); }

"#"			{ return GIN ? *yytext : gdl_lexfld[(int)*yytext]; }

[!?]			{ return *yytext; }

["~]   			{ /*"*/ return gdl_lexfld[(int)*yytext]; }

"..."			{ GIN=0; gdllval.text=gdlsave(gdltext); return ELLIPSIS; }
"--"			{ GIN=0; return ENHYPHEN; }
[-+:.]     		{ GIN=0; return gdltext[0]; }

[ \t]+			{ GIN=0; return SPACE; }

\n			{ return gdltext[0]; }

"($"			{ BEGIN T; return L_inl_dol; }
<T>"$)"			{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN T; return L_inl_cmt; }
<T>"#)"			{ BEGIN 0; return R_inl_cmt; }

<T>.+/[$#]\)   		{ gdllval.text=gdlsave(gdltext); return TEXT; }

.			{ mesg_vwarning(currgdlfile, gdllineno, "gdl.l: unmatched character %c\n", *gdltext); }

<<EOF>>			{ return 0; }
%%

int
gdl_grapheme_type(void)
{
  gdllval.text=gdlsave(gdltext);
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
    fprintf(stderr, "%s:%d: gdllex: text=::%s:: GIN=%d\n", gdlfile, gdllineno, gdltext, GIN);

  gdllloc.file = currgdlfile;
  gdllloc.line = yylineno;
}

void
gdl_lex_init(const char *file, int line)
{
  GIN = 0;
  currgdlfile = gdlfile = file;
  gdllineno = line;
}

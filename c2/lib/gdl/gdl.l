%option prefix="gdl"
%option outfile="gdl-l.c"
%option never-interactive noyywrap yylineno nounput

%{
#include <pool.h>
#include "gdl.tab.h"

#define yylloc gdllloc
#define YY_USER_ACTION gdl_user_action();

const char *efile = "<stdin>";
int flextrace = 1;
int tok;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);

%}

%x C
%x T

%%

"($"			{ BEGIN T; return L_inl_dol; }
<T>"$)"			{ BEGIN 0; return R_inl_dol; }

"(#"			{ BEGIN T; return L_inl_cmt; }
<T>"#)"			{ BEGIN 0; return R_inl_cmt; }

<T>.+			{ gdllval.text=gdlsave(gdltext); return TEXT; }

"<<"			{ return L_dbl_ang; }
">>"			{ return R_dbl_ang; }
"{{"			{ return L_dbl_cur; }
"}}"			{ return R_dbl_cur; }
"("			{ return yytext[0]; }
")"			{ return yytext[0]; }
"["			{ return yytext[0]; }
"]"			{ return yytext[0]; }
"{"			{ return yytext[0]; }
"}"			{ return yytext[0]; }
L_UHS			{ return L_uhs; }
R_UHS			{ return R_uhs; }
L_LHS			{ return L_lhs; }
R_LHS			{ return R_lhs; }

%[-a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return LANG; }
,			{ return FIELD; }
&			{ return ALIGN; }
![a-z]+			{ return FTYPE; }

[a-zA-Z0-9\x80-\xff]+   { gdllval.text=gdlsave(gdltext); return CHARS; }

@[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return MOD_AT; }
~[a-z0-9]+     		{ gdllval.text=gdlsave(gdltext); return MOD_TL; }

"--"			{ return ENYHPHEN;
[-+:]     		{ return gdltext[0]; }

[ \t]+			{ return SPACE; }

\n			{ return gdltext[0]; }

"|"			{ BEGIN C; return C_O; }
<C>"|"			{ BEGIN 0; return C_C; }

<C>"."			{ return C_PERIOD; }
<C>"+"			{ return C_PLUS; }
<C>":"			{ return C_COLON; }
<C>"&"			{ return C_ABOVE; }
<C>"%"			{ return C_CROSSING; }
<C>@/[^a-z0-9]		{ return C_OPPOSING; }
<C>4TIMES		{ return C_4TIMES; }
<C>3TIMES		{ return C_3TIMES; }
<C>TIMES		{ return C_TIMES; }
<C>"("			{ return C_L_par; }
<C>")"			{ return C_R_par; }

<C>[a-zA-Z0-9\x80-\xff]+ { gdllval.text=gdlsave(gdltext); return CHARS; }
<C>@[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return MOD_AT; }
<C>~[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return MOD_TL; }

%%

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
  if (flextrace)
    fprintf(stderr, "%s:%d: flextrace: text=::%s::\n", efile, gdllineno, gdltext);
#if 0
  yylloc.file = efile;
  yylloc.first_line = yylloc.last_line = yylineno;
#endif
}

%option prefix="cat"
%option outfile="cat-l.c"
%option never-interactive noyywrap yylineno

%{
#include <pool.h>
#include "gdl.tab.h"

#define yylloc gdllloc
#define YY_USER_ACTION gdl_user_action();

const char *efile = "<stdin>";
int flextrace = 0;
int tok;

extern Pool *gdlpool;
#define gdlsave(s) (char*)pool_copy((const unsigned char*)(s),gdlpool);
static void gdl_user_action(void);

%}

%%

<<			{ return L_dbl_ang; }
>>			{ return R_dbl_ang; }
{{			{ return L_dbl_cur; }
}}			{ return R_dbl_cur; }
($			{ return L_inl_dol; }
$)			{ return R_inl_dol; }
(#			{ return L_inl_cmt; }
#)			{ return R_inl_cmt; }
(			{ return L_par; }
)			{ return R_par; }
[			{ return L_squ; }
]			{ return R_squ; }
{			{ return L_cur; }
}			{ return R_cur; }
L_UHS			{ return L_uhs; }
R_UHS			{ return R_uhs; }
L_LHS			{ return L_lhs; }
R_LHS			{ return R_lhs; }

%[-a-z0-]		{ gdllval.text=gdlsave(gdltext); return LANG; }
,			{ return FIELD; }
&			{ return ALIGN; }
![a-z]+			{ return FTYPE; }

[a-zA-Z0-9\1f-\xff]+    { gdllval.text=gdlsave(gdltext); return CHARS; }

@[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return MOD_AT; }
~[a-z0-9]+     		{ gdllval.text=gdlsave(gdltext); return MOD_TL; }

"."			{ return PERIOD; }
-			{ return HYPHEN; }
"+"			{ return PLUS; }
":"			{ return COLON; }

[ \t]			{ return yytext[0]; }
\n			{ return EOL; }

|			{ BEGIN C; return C_O; }
<C>|			{ BEGIN 0; return C_C; }

<C>.			{ return C_PERIOD; }
<C>+			{ return C_PLUS; }
<C>:			{ return C_COLON; }
<C>&			{ return C_ABOVE; }
<C>%			{ return C_CROSSING; }
<C>@/[^a-z0-9]		{ return C_OPPOSING; }
<C>4TIMES		{ return C_4TIMES; }
<C>3TIMES		{ return C_3TIMES; }
<C>TIMES		{ return C_TIMES; }
<C>(			{ return C_L_par; }
<C>)			{ return C_R_par; }

<C>[a-zA-Z0-9\1f-\xff]+ { return gdllval.text=gdlsave(gdltext); return CHARS; }
<C>@[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return C_MOD_AT; }
<C>~[a-z0-9]+		{ gdllval.text=gdlsave(gdltext); return C_MOD_TL; }

%%

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

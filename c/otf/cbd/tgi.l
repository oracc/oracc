static struct cbdtag *ctp;

%x s_sol, s_flag
%x s_b, s_bp, s_ba
%x s_cf, s_gw, s_pos
%x s_f

%%
^[-+>]+		{ BEGIN ss(s_sol); 	 return(yytext); }
^@[a-z]+	{ ctp = cbdtags(yytext); BEGIN ss(ctp->ss); s_after(); xreturn(ctp->tok); }
^@[a-z]+/[!*]+	{ ctp = cbdtags(yytext); BEGIN ss(s_flag); }
<s_flag>[!*]+	{ set_flags(yytext); 	 BEGIN ss(ctp->ss); xreturn(ctp->tok); }
^@[A-Z]+	{ ctp = cbdrws(yytext);  BEGIN ss(ctp->ss); xreturn(ctp->tok); } 
<s_sol>@[a-z]+	{ ctp = cbdtags(yytext); BEGIN ss(ctp->ss); xreturn(ctp->tok); }
[^ \t\n]+  	{ xreturn(TEXT); }
[ \t]+		/* empty */
\n		{ yycolumn=1; ctp->eol_sp ? xreturn(TEXT) : xreturn(EOL) }

<s_cf>[^\[\] \t\n][^\[\]\n]*[^\[\] \t\n]        { dup(unspace(yytext)); xreturn(CF); }
<s_cf>[^\[\] \t\n]         { dup(unspace(yytext)); xreturn(CF); }
<s_cf>"["      	   	   { BEGIN ss(s_gw); xreturn(yytext[yyleng-1]); }
<s_gw>([^\]\n]+)	   { ytdup(); xreturn(GW); }
<s_cf,s_gw>"]"     	   { BEGIN ss(after_xGW); xreturn(yytext[0]); }
<s_pos>[^ \t\n]+[ \t]*/\n  { BEGIN ss(0); ytdup(); xreturn(POS); }
<s_pos>[^ \t\n]+[ \t]*	   { BEGIN ss(after_xPOS); ytdup(); xreturn(POS); }
<s_pos>[ \t]*/\n	   { BEGIN ss(0); }

%%

void
s_after(void)
{
  switch (ctp->tok)
    {
      case ENTRY:
      break;
      case SENSE:
      break;
      case ALIAS:
      break;
      case PARTS:
      break;
      case CF:
      break;
      case GW:
      break;
      case POS:
      break;
    }
}

void
set_flags(char *f)
{
  while (*f)
    {
      if (*f == '*')
	star = 1;
      else if (*f == '!')
	bang = 1;
      else
	fprintf(stderr, "gx: internal error: unhandled flag char %c\n", *f);
    }
}


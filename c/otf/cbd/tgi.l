static struct cbdtag *ctp;

/* keep-space-dup */
#define ksdup(s) yylval.text = (char*)npool_copy((unsigned char *)(s),curr_cbd->pool)
/* yytext-dup strips beginning/end spaces */
#define ytdup() (yylval.text = (char*)npool_copy((ucp)unspace(yytext),curr_cbd->pool))
#define ytdupn(n) yylval.text = (char*)npool_copy((ucp)unspace(yytext+(n)),curr_cbd->pool)
#define yi(tok) yylval.i = tok;
#define yln(s) if (ylntrace) {fprintf(stderr, "%s: %d\n",(s),yylineno);}

%x s_sol, s_flag
%x s_b, s_bp, s_ba
%x s_cf, s_gw, s_pos
%x s_f

%%
^[-+>]+		{ BEGIN ss(s_sol); 	 return(yytext); }
^@[a-z]+	{ set_ctp()   && xreturn(ctp->tok); }
^@[a-z]+/[!*]+	{ set_ctp()   && BEGIN ss(s_flag); }
<s_flag>[!*]+	{ set_flags() && xreturn(ctp->tok); }
^@[A-Z]+	{ set_ctp()   && xreturn(ctp->tok); } 
<s_sol>@[a-z]+	{ set_ctp()   && xreturn(ctp->tok); }
[^ \t\n]+  	{ ksdup(); xreturn(TEXT); }
[ \t]+		/* empty */
\n		{ yycolumn=1; ctp->eol_sp ? xreturn(TEXT) : xreturn(EOL) }

<s_cf>[^\[\] \t\n][^\[\]\n]*[^\[\] \t\n]        { dup(unspace(yytext)); xreturn(CF); }
<s_cf>[^\[\] \t\n]         { ytdup(); xreturn(CF); }
<s_cf>"["      	   	   { BEGIN ss(s_gw); xreturn(yytext[yyleng-1]); }
<s_gw>([^\]\n]+)	   { ytdup(); xreturn(GW); }
<s_cf,s_gw>"]"     	   { BEGIN ss(after_s_gw); xreturn(yytext[0]); }
<s_pos>[^ \t\n]+[ \t]*/\n  { BEGIN ss(0); ytdup(); xreturn(POS); }
<s_pos>[^ \t\n]+[ \t]*	   { BEGIN ss(after_s_pos); ytdup(); s_return(POS); }
<s_pos>[ \t]*/\n	   { BEGIN ss(0); }

^@bases[ \t]*";"[ \t]*	 { BEGIN ss(xBP); basesc("initial"); xreturn(BASES); }
^@bases[ \t]*	 	 { BEGIN ss(xBP); xreturn(BASES); }
<xBP>%[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(LANGSPEC); }
<xBP>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); }
<xBP>\;[ \t]*\;[ \t]*     { BEGIN ss(xBP); basesc("double"); }
<xBP>\;[ \t]+    	  { BEGIN ss(xBP); }
<xBP>[^; \t\n]+[ \t]*/"(" { BEGIN ss(xBA); ytdup(); xreturn(BASE_PRI); }
<xBP>[^; \t\n]+[ \t]* 	  { ytdup(); xreturn(BASE_PRI); }
<xBA>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }
<xBA>,[ \t]*,		  { baseco("double"); unput(','); }
<xBA>,[ \t]*		  { if (b_alt_tok) { alt_wrapup(0); xreturn(BASE_ALT); } }
<xBA>"("[ \t]*,		  { baseco("initial"); unput('('); }
<xBA>"("[^;, \t\n]+[ \t]* { b_alt_tok = strdup(yytext+1); }
<xBA>[^;, \t\n]+[ \t]* 	  { b_alt_tok = strdup(yytext); }
<xBA>\;[ \t]*\;[ \t]*     { BEGIN ss(xBP); basesc("double"); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }
<xBA>\;[ \t]*    	  { BEGIN ss(xBP); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }

^@form[ \t]+	  	  { BEGIN ss(xF); xreturn(FORM); }
^@form![ \t]+	  	  { BEGIN ss(xF); bang = 1; xreturn(FORM); }
<xF>%[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FLANG); }
<xF>@[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FRWS); }
<xF>\/[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FBASE); }
<xF>\+[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FCONT); }
<xF>\$[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FNORM); }
<xF>##[^ \t\n]+[ \t]*	  { ytdupn(2); xreturn(FMORPH2); }
<xF>#[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FMORPH); }
<xF>\*[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FSTEM); }
<xF>[^+$#*/\n][^ \t\n]*[ \t]* { ytdup(); xreturn(FFORM); }

^@include[ \t]*		  { BEGIN ss(xINCL); }
<xINCL>[^ \t\n]+   	  { includeo(yytext); }

<xFLUSH>.*		  { BEGIN ss(0); }

%%

void
set_after(void)
{
  switch (ctp->tok)
    {
      case ENTRY:
        after_s_RENMRG = after_s_ENTSNS = s_cf; after_s_GW = s_POS; after_s_POS = 0;
        break;
      case SENSE:
      case SENSEL:
	after_s_RENMRG = after_s_ENTSNS = after_s_gw = s_spos;
	break;
      case ALIAS:
	after_s_POS = 0;
        break;
      case PARTS:
	after_s_POS = 0;
        break;
      default:
        break;
    }
}

int
set_ctp()
{
  ctp = cbdtag(yytext);
  if (!ctp)
    {
      BEGIN ss(s_flush); yylloc.file=efile; yylloc.first_line=yylineno;
      vyyerror(yylloc,"unknown at-command %s\n",yytext, NULL);
      return 0;
    }
  else
    {
      BEGIN ss(ctp->ss); set_after();
    }
  return 1;
}

int
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
  return ctp != NULL;
}


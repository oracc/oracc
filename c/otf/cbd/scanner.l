%option never-interactive noyywrap yylineno
%{

#include <ctype128.h>
#include "gx.h"
#include "npool.h"
#define YY_NO_UNISTD_H

#define dup(s) yylval.text = (char*)npool_copy((unsigned char *)(s),curr_cbd->pool)
#define ytdup() yylval.text = strdup(unspace(yytext))
#define ytdupn(n) yylval.text = strdup(unspace(yytext+(n)))
#define yi(tok) yylval.i = tok;
#define yln(s) if (ylntrace) {fprintf(stderr, "%s: %d\n",(s),yylineno);}
static int after_xENTSNS = 0;
static int after_xGW = 0;
static int after_xPOS = 0;
static int after_xRENMRG = 0;
static int xdisc = 0;
static int yycolumn = 0;
extern char *efile;

static void baseco(const char *type);
static void basesc(const char *type);

static char *b_alt_tok = NULL;
static void alt_wrapup(int clean);
static void clean_alt_paren(void);
static char *unspace(char*s);
static void gx_user_action(void);
#define mtch(x) fprintf(stderr,"matched %s",(x))

#define YY_USER_ACTION gx_user_action();

int ylntrace = 0;
extern int flextrace;
int mystate = 0;
#define ss(arg) (mystate=arg)

%}
%x xALLOW
%x xB xBP xBA
%x xCF xGW xPOS xSPOS
%x xF
%x xS xSW
%x xPROJ xLANG xTEXT xWORD xENTSNS xWHY xCOMMENT xELANG xOID xFLUSH

%%

<xCF,xGW,xPOS>\n	  { BEGIN ss(0); yycolumn = 1; return EOL; }
<xBP,xALLOW,xFLUSH,xWORD,xTEXT>\n  { BEGIN ss(0); yycolumn = 1; }
<xBA>\n			  { BEGIN ss(0); yycolumn = 1; if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xF>\n			  { BEGIN ss(0); yycolumn = 1; return(END_FORM); }
<xS,xSPOS,xSW>\n          { BEGIN ss(0); yycolumn = 1; }

<xWORD>[^ \t\n]+[\t ]*   { ytdup(); return(WORDSPEC); }
<xTEXT>.+		 { ytdup(); return(TEXTSPEC); }
^@project[\t ]+		 { yln("@project"); BEGIN xPROJ; return PROJECT; }
<xPROJ>[^ \t\n]+[ \t]*   { BEGIN ss(0); dup(unspace(yytext)); return PROJSPEC; }
^@lang[ \t]+		 { BEGIN ss(xLANG); return LANG; }
<xLANG>[^ \t\n]+[ \t]*   { BEGIN ss(0); dup(unspace(yytext)); return LANGSPEC; }
^@name[\t ]+		 { BEGIN ss(xTEXT); return NAME; }
^@proplist[\t ]+	 { BEGIN ss(xTEXT); return PROPLIST; }

^[+-]			 { BEGIN ss(xENTSNS); return yytext[0]; }
<xENTSNS>@entry[\t ]+	 { BEGIN ss(after_xENTSNS); return(ENTRY); }
<xENTSNS>@sense[\t ]+	 { BEGIN ss(after_xENTSNS); return(SENSE); }
<xENTSNS>.		 { BEGIN ss(0); yyless(0); /* should error */ }

^@entry[\t ]+	 	 { yln("@entry"); BEGIN ss(xCF); after_xRENMRG = after_xENTSNS = xCF; after_xGW = xPOS; after_xPOS = 0; xdisc = EDISC; return(ENTRY); }
^@end[ \t]+entry[\t ]*   { yln("@endentry"); BEGIN ss(0); return(END_ENTRY); }

^@alias[\t ]+		 { BEGIN ss(xCF); after_xPOS = 0; return(ALIAS); }

^@parts[\t ]+		 { BEGIN ss(xCF); after_xPOS = xCF; return(PARTS); }

^>[\t ]*		 { BEGIN ss(after_xRENMRG); return(RENAME); }
^>>[\t ]*		 { BEGIN ss(after_xRENMRG); return(MERGE); }

<xCF>[^\[\]\n]+		 { dup(unspace(yytext)); return(CF); }
<xCF>[ \t]*"["      	 { BEGIN ss(xGW); return(yytext[yyleng-1]); }
<xGW>([^\]\n]+)	 	 { ytdup(); return(GW); }
<xCF,xGW>"]"[ \t]*     	 { BEGIN ss(after_xGW); return(yytext[0]); }
<xPOS>[^ \t\n]+[ \t]*	 { BEGIN ss(after_xPOS); ytdup(); return(POS); }
<xPOS>[ \t]*		 { BEGIN ss(0); return(EOL); }

^#why:[\t ]+		 { BEGIN ss(xWHY); return(WHY); }
<xWHY>.+		 { BEGIN ss(0); ytdup(); return WHYSPEC; }
^#			 { BEGIN ss(xCOMMENT); }
<xCOMMENT>.*		 { BEGIN ss(0); }
^@[A-Z]+[ \t]*		 { BEGIN ss(xTEXT); ytdupn(1); return(DCF); }

^@allow[ \t]+	      	 { BEGIN ss(xALLOW); return(ALLOW); }
<xALLOW>[^ \t\n]       	 { ytdup(); return(BASE_PRI); }
<xALLOW>=[ \t]+		 { return(yytext[0]); }
<xALLOW>[^= \t\n]+[ \t]* { ytdup(); return(BASE_PRI); }

^@bases[ \t]*";"[ \t]*	 { BEGIN ss(xBP); basesc("initial"); return(BASES); }
^@bases[ \t]*	 	 { BEGIN ss(xBP); return(BASES); }
<xBP>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); }
<xBP>\;[ \t]*\;[ \t]*     { BEGIN ss(xBP); basesc("double"); }
<xBP>\;[ \t]+    	  { BEGIN ss(xBP); }
<xBP>[^; \t\n]+[ \t]*/"(" { BEGIN ss(xBA); ytdup(); return(BASE_PRI); }
<xBP>[^; \t\n]+[ \t]* 	  { ytdup(); return(BASE_PRI); }
<xBA>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xBA>,[ \t]*,		  { baseco("double"); unput(','); }
<xBA>,[ \t]*		  { if (b_alt_tok) { alt_wrapup(0); return(BASE_ALT); } }
<xBA>"("[ \t]*,		  { baseco("initial"); unput('('); }
<xBA>"("[^;, \t\n]+[ \t]* { b_alt_tok = strdup(yytext+1); }
<xBA>[^;, \t\n]+[ \t]* 	  { b_alt_tok = strdup(yytext); }
<xBA>\;[ \t]*\;[ \t]*     { BEGIN ss(xBP); basesc("double"); if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xBA>\;[ \t]*    	  { BEGIN ss(xBP); if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }

^@phon[\t ]+		 { BEGIN ss(xTEXT); return(PHON); }
^@root[\t ]+		 { BEGIN ss(xTEXT); return(ROOT); }
^@stems[\t ]+		 { BEGIN ss(xWORD); return(STEM); }

^@form[ \t]+		  { BEGIN ss(xF); return(FORM); }
<xF>%[^ \t\n]+[ \t]*	  { ytdupn(1); return(FLANG); }
<xF>\/[^ \t\n]+[ \t]*	  { ytdupn(1); return(FBASE); }
<xF>\+[^ \t\n]+[ \t]*	  { ytdupn(1); return(FCONT); }
<xF>\$[^ \t\n]+[ \t]*	  { ytdupn(1); return(FNORM); }
<xF>##[^ \t\n]+[ \t]*	  { ytdupn(2); return(FMORPH2); }
<xF>#[^ \t\n]+[ \t]*	  { ytdupn(1); return(FMORPH); }
<xF>\*[^ \t\n]+[ \t]*	  { ytdupn(1); return(FSTEM); }
<xF>[^+$#*/\n][^ \t\n]*[ \t]* { ytdup(); return(FFORM); }

^@senses		  { return(SENSES); }
^@end[ \t]+senses	  { return(END_SENSES); }
^@sense[ \t]+		  { BEGIN ss(xS); after_xRENMRG = after_xENTSNS = after_xGW = xSPOS; xdisc = SDISC; return(SENSE); }
<xS>\%			  { BEGIN ss(xSW); return(yytext[0]); }
<xS>\.			  { BEGIN ss(xSW); return(yytext[0]); }
<xS>\#			  { BEGIN ss(xSW); return(yytext[0]); }
<xS>\[			  { BEGIN ss(xGW); return(yytext[0]); }
<xS>[^.#[\%][^ \t\n]*[ \t]* { BEGIN ss(xTEXT); ytdup(); return(POS); }
<xSPOS>[^ \t\n]+[ \t]*	  { BEGIN ss(xTEXT); ytdup(); return(POS); }
<xSW>[^ \t\n]+[ \t]*	  { BEGIN ss(xS); ytdup(); return(WORDSPEC); }

^@disc[ \t]+		  { BEGIN ss(xTEXT); return(xdisc); }
^@bib[ \t]+		  { BEGIN ss(xTEXT); yi(BIB); return(BIB); }
^@collo[ \t]+		  { BEGIN ss(xTEXT); yi(COLLO); return(COLLO); }
^@equiv[ \t]+		  { BEGIN ss(xELANG); yi(EQUIV); return(EQUIV); }
<xELANG>%[^ \t\n]+[ \t]+	  { BEGIN ss(xTEXT); ytdupn(1); return LANG; }
^@inote[ \t]+		  { BEGIN ss(xTEXT); yi(INOTE); return(INOTE); }
^@isslp[ \t]+		  { BEGIN ss(xTEXT); yi(ISSLP); return(ISSLP); }
^@note[ \t]+		  { BEGIN ss(xTEXT); yi(NOTE); return(NOTE); }
^@oid[ \t]+ 	          { BEGIN ss(xOID); yi(OID); return(OID); }
<xOID>o[^ \t\n]+[ \t]*	  { BEGIN ss(0); ytdup(); return(OIDSPEC); }
^@pl_coord[ \t]+	  { BEGIN ss(xTEXT); return(PL_COORD); }
^@pl_id[ \t]+     	  { BEGIN ss(xTEXT); return(PL_ID); }
^@pl_uid[ \t]+	  	  { BEGIN ss(xTEXT); return(PL_UID); }
^@prop[ \t]+		  { BEGIN ss(xTEXT); yi(PROP); return(PROP); }

^@[^ \t\n]+[ \t]*     	  { BEGIN ss(xFLUSH); yylloc.file=efile; yylloc.first_line=yylineno; vyyerror(yylloc,"unknown at-command %s\n",yytext, NULL); }
<xFLUSH>.*		  { BEGIN ss(0); }

.
\n			  { yycolumn = 1; }

<<EOF>>			  { if (flextrace) { fprintf(stderr, "<<EOF>>\n"); } return(0); }

%%

void
alt_wrapup(int clean)
{
  if (clean)
    clean_alt_paren(); 
  dup(b_alt_tok); 
  free(b_alt_tok); 
  b_alt_tok = NULL;
}

static void
basesc(const char *type)
{
  yylloc.file=efile;
  yylloc.first_line=yylineno; 
  vyyerror(yylloc,"misplaced %s semi-colon\n", type, NULL);
}

static void
baseco(const char *type)
{
  yylloc.file=efile;
  yylloc.first_line=yylineno; 
  vyyerror(yylloc,"misplaced %s comma\n", type, NULL);
}

static void
clean_alt_paren(void)
{
  b_alt_tok[strlen(b_alt_tok)-1] = '\0';
}

static char *
unspace(char*s)
{
  int i = strlen(s);
  while (isspace(s[i-1]))
    --i;
  if (i >= 0)
    s[i] = '\0';
  return s;
}

static void
gx_user_action(void)
{
  if (flextrace)
    {
      fprintf(stderr, "%s:%d: flextrace: state=%d tok=::%s::\n", efile, yylineno, mystate, yytext);
    }
  yylloc.file = efile;
  yylloc.first_line = yylloc.last_line = yylineno;
  yylloc.first_column = yylloc.last_column;
  for(int i = 0; yytext[i] != '\0'; i++)
    {
      if (yytext[i] == '\n')
        {
          yylloc.last_line++;
          yylloc.last_column = 0;
        }
      else 
        {
          yylloc.last_column++;
        }
    }
}

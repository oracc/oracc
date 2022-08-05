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

static char *b_alt_tok = NULL;
static void alt_wrapup(int clean);
static void clean_alt_paren(void);
static char *unspace(char*s);
static void gx_user_action(void);

#define YY_USER_ACTION gx_user_action();

int ylntrace = 0;
extern int flextrace;
%}
%x xALLOW
%x xB xBP xBA
%x xCF xGW xPOS xSPOS
%x xF
%x xS xSW
%x xPROJ xLANG xTEXT xWORD xENTSNS xWHY xCOMMENT xELANG xOID xFLUSH

%%

<xWORD>[^ \t\n]+[\t ]*   { ytdup(); return(WORDSPEC); }
<xTEXT>.+		 { ytdup(); return(TEXTSPEC); }
^@project[\t ]+		 { yln("@project"); BEGIN xPROJ; return PROJECT; }
<xPROJ>[^ \t\n]+[ \t]*   { BEGIN 0; dup(unspace(yytext)); return PROJSPEC; }
^@lang[ \t]+		 { BEGIN xLANG; return LANG; }
<xLANG>[^ \t\n]+[ \t]*   { BEGIN 0; dup(unspace(yytext)); return LANGSPEC; }
^@name[\t ]+		 { BEGIN xTEXT; return NAME; }
^@proplist[\t ]+	 { BEGIN xTEXT; return PROPLIST; }

^[+-]			 { BEGIN xENTSNS; return yytext[0]; }
<xENTSNS>@entry[\t ]+	 { BEGIN after_xENTSNS; return(ENTRY); }
<xENTSNS>@sense[\t ]+	 { BEGIN after_xENTSNS; return(SENSE); }
<xENTSNS>.		 { BEGIN 0; yyless(0); /* should error */ }

^@entry[\t ]+	 	 { yln("@entry"); BEGIN xCF; after_xRENMRG = after_xENTSNS = xCF; after_xGW = xPOS; after_xPOS = 0; xdisc = EDISC; return(ENTRY); }
^@end[ \t]+entry[\t ]*   { yln("@endentry"); BEGIN 0; return(END_ENTRY); }

^@alias[\t ]+		 { BEGIN xCF; after_xPOS = 0; return(ALIAS); }

^@parts[\t ]+		 { BEGIN xCF; after_xPOS = xCF; return(PARTS); }

^>[\t ]*		 { BEGIN after_xRENMRG; return(RENAME); }
^>>[\t ]*		 { BEGIN after_xRENMRG; return(MERGE); }

<xCF>[^\[\n]+		 { dup(unspace(yytext)); return(CF); }
<xCF>[ \t]*"["      	 { BEGIN xGW; return(yytext[yyleng-1]); }
<xGW>([^\]]+)		 { yylval.text = strdup(yytext); return(GW); }
<xGW>"]"[ \t]*	       	 { BEGIN after_xGW; return(yytext[0]); }
<xPOS>[^ \t\n]+[ \t]*	 { BEGIN after_xPOS; yylval.text = strdup(unspace(yytext)); return(POS); }
<xPOS>[ \t]*		 { BEGIN 0; return(EOL); }

^#why:[\t ]+		 { BEGIN xWHY; return(WHY); }
<xWHY>.+		 { BEGIN 0; yylval.text = strdup(unspace(yytext)); return WHYSPEC; }
^#			 { BEGIN xCOMMENT; }
<xCOMMENT>.*		 { BEGIN 0; }

^@allow[ \t]+	      	 { BEGIN xALLOW; return(ALLOW); }
<xALLOW>[^ \t\n]		 { ytdup(); return(BASE_PRI); }
<xALLOW>=[ \t]+		 { return(yytext[0]); }
<xALLOW>[^= \t\n]+[ \t]* { ytdup(); return(BASE_PRI); }
^@bases[ \t]+ 		 { BEGIN xBP; return(BASES); }
<xBP>\;[ \t]+    	 { BEGIN xBP; }
<xBP>[^; \t\n]+ 	 { ytdup(); return(BASE_PRI); }
<xBP>[ \t]+\(		 { BEGIN xBA; }
<xBA>,[ \t]+		 { if (b_alt_tok) { alt_wrapup(0); return(BASE_ALT); } }
<xBA>[^;, \t\n]+ 	 { b_alt_tok = strdup(yytext); }
<xBA>\;[ \t]+    	 { BEGIN xBP; if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xBP>\;			 { BEGIN xBP; unput(';'); }

^@phon[\t ]+		 { BEGIN xTEXT; return(PHON); }
^@root[\t ]+		 { BEGIN xTEXT; return(ROOT); }
^@stems[\t ]+		 { BEGIN xWORD; return(STEM); }

^@form[ \t]+		  { BEGIN xF; return(FORM); }
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
^@sense[ \t]+		  { BEGIN xS; after_xRENMRG = after_xENTSNS = after_xGW = xSPOS; xdisc = SDISC; return(SENSE); }
<xS>\%			  { BEGIN xSW; return(yytext[0]); }
<xS>\.			  { BEGIN xSW; return(yytext[0]); }
<xS>\#			  { BEGIN xSW; return(yytext[0]); }
<xS>\[			  { BEGIN xGW; return(yytext[0]); }
<xS>[^.#[\%][^ \t]*[ \t]* { BEGIN xTEXT; ytdup(); return(POS); }
<xSPOS>[^ \t]+[ \t]*	  { BEGIN xTEXT; ytdup(); return(POS); }
<xSW>[^ \t]+[ \t]*	  { BEGIN xS; ytdup(); return(WORDSPEC); }

^@disc[ \t]+		  { BEGIN xTEXT; return(xdisc); }
^@bib[ \t]+		  { BEGIN xTEXT; yi(BIB); return(BIB); }
^@collo[ \t]+		  { BEGIN xTEXT; yi(COLLO); return(COLLO); }
^@equiv[ \t]+		  { BEGIN xELANG; yi(EQUIV); return(EQUIV); }
<xELANG>%[^ \t]+[ \t]+	  { BEGIN xTEXT; ytdupn(1); return LANG; }
^@inote[ \t]+		  { BEGIN xTEXT; yi(INOTE); return(INOTE); }
^@isslp[ \t]+		  { BEGIN xTEXT; yi(ISSLP); return(ISSLP); }
^@note[ \t]+		  { BEGIN xTEXT; yi(NOTE); return(NOTE); }
^@oid[ \t]+ 	          { BEGIN xOID; yi(OID); return(OID); }
<xOID>o[^ \t\n]+[ \t]*	  { BEGIN 0; ytdup(); return(OIDSPEC); }
^@pl_coord[ \t]+	  { BEGIN xTEXT; return(PL_COORD); }
^@pl_id[ \t]+     	  { BEGIN xTEXT; return(PL_ID); }
^@pl_uid[ \t]+	  	  { BEGIN xTEXT; return(PL_UID); }
^@prop[ \t]+		  { BEGIN xTEXT; yi(PROP); return(PROP); }

^@[^ \t\n]+[ \t]*     	  { BEGIN xFLUSH; yylloc.file=efile; yylloc.first_line=yylineno; vyyerror(yylloc,"unknown at-command %s\n",yytext, NULL); }
<xFLUSH>.*		  { BEGIN 0; }

.
<INITIAL,xBP,xCF,xALLOW,xFLUSH,xWORD,xTEXT>\n	  { BEGIN 0; yycolumn = 1; }
<xBA>\n			  { BEGIN 0; yycolumn = 1; if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xF>\n			  { BEGIN 0; yycolumn = 1; return(END_FORM); }

<<EOF>>			  { return(ENDOF); }

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
      fprintf(stderr, "%s:%d: flextrace: ::%s::\n", efile, yylineno, yytext);
    }
  yylloc.file = efile;
  yylloc.first_line = yylloc.last_line;
  yylloc.first_column = yylloc.last_column;
  for(int i = 0; yytext[i] != '\0'; i++)
    {
      if(yytext[i] == '\n')
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

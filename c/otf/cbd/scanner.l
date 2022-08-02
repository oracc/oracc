%option never-interactive noyywrap
%{
#include <ctype128.h>
#include "gx.h"
#include "npool.h"
#include "grammar.tab.h"
#define YY_NO_UNISTD_H

#define dup(s) yylval.text = (char*)npool_copy((unsigned char *)(s),curr_cbd->pool)
#define ytdup() yylval.text = strdup(unspace(yytext))
#define ytdupn(n) yylval.text = strdup(unspace(yytext+(n)))

static char *b_alt_tok = NULL;
static void alt_wrapup(int clean);
static void clean_alt_paren(void);
static char *unspace(char*s);
%}
%x xALLOW
%x xB xBP xBA
%x xCF xGW xPOS
%x xF
%x xPROJ xLANG xTEXT xWORD xENTSNS xWHY xCOMMENT
%%
^@project[\t ]+		 { BEGIN xPROJ; return PROJECT; }
<xPROJ>[^ \t\n]+[ \t]*\n { BEGIN 0; dup(unspace(yytext)); return PROJSPEC; }
^@lang[ \t]+		 { BEGIN xLANG; return LANG; }
<xLANG>[^ \t\n]+[ \t]*\n { BEGIN 0; dup(unspace(yytext)); return LANGSPEC; }
^@name[\t ]+		 { BEGIN xTEXT; return NAME; }
<xTEXT>.+\n 		 { BEGIN 0; dup(unspace(yytext)); return TEXTSPEC; }
^@proplist[\t ]+	 { BEGIN xTEXT; return PROPLIST; }

^[+-]			 { BEGIN xENTSNS; return yytext[0]; }
<xENTSNS>@entry[\t ]+	 { BEGIN xCF; return(ENTRY); }
<xENTSNS>@sense[\t ]+	 { BEGIN xCF; return(SENSE); }
<xENTSNS>.		 { BEGIN 0; yyless(0); /* should error */ }

^@entry[\t ]+	 	 { BEGIN xCF; return(ENTRY); }
^@end[ \t]+entry[\t ]*\n { BEGIN 0;   return(END_ENTRY); }

^@alias[\t ]+		 { BEGIN xCF; return(ALIAS); }

^@parts[\t ]+		 { BEGIN xCF; return(PARTS); }

^>[\t ]*		 { BEGIN xCF; return(RENAME); }
^>>[\t ]*		 { BEGIN xCF; return(MERGE); }

<xCF>[^\[\n]+		 { dup(unspace(yytext)); return(CF); }
<xCF>[ \t]*"["      	 { BEGIN xGW; return(yytext[yyleng-1]); }
<xGW>([^\]]+)		 { yylval.text = strdup(yytext); return(GW); }
<xGW>"]"[ \t]*	       	 { BEGIN xPOS; return(yytext[0]); }
<xPOS>[^ \t\n]+[ \t]*\n	 { BEGIN 0; yylval.text = strdup(unspace(yytext)); return(POS); }
<xPOS>[^ \t\n]+[ \t]*  	 { BEGIN xCF; yylval.text = strdup(unspace(yytext)); return(POS); }

^#why:[\t ]+		 { BEGIN xWHY; return(WHY); }
<xWHY>.+\n		 { BEGIN 0; yylval.text = strdup(unspace(yytext)); return WHYSPEC; }
^#			 { BEGIN xCOMMENT; }
<xCOMMENT>.*\n		 { BEGIN 0; }

^@allow[ \t]+	      	 { BEGIN xALLOW; return(ALLOW); }
<xALLOW>[^ \t\n]		 { ytdup(); return(BASE_PRI); }
<xALLOW>=[ \t]+		 { return(yytext[0]); }
<xALLOW>[^= \t\n]+[ \t]* { ytdup(); return(BASE_PRI); }
<xALLOW>\n		 { BEGIN 0; }
^@bases[ \t]+ 		 { BEGIN xBP; return(BASES); }
<xBP>\;[ \t]+    	 { BEGIN xBP; }
<xBP>\n			 { BEGIN 0; }
<xBP>[^; \t\n]+ 	 { ytdup(); return(BASE_PRI); }
<xBP>[ \t]+\(		 { BEGIN xBA; }
<xBA>,[ \t]+		 { if (b_alt_tok) { alt_wrapup(0); return(BASE_ALT); } }
<xBA>[^;, \t\n]+ 	 { b_alt_tok = strdup(yytext); }
<xBA>\;[ \t]+    	 { BEGIN xBP; if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }
<xBP>\;			 { BEGIN xBP; unput(';'); }
<xBA>\n			 { BEGIN 0; if (b_alt_tok) { alt_wrapup(1); return(BASE_ALT); } }

^@phon[\t ]+		 { BEGIN xTEXT; return(PHON); }
^@root[\t ]+		 { BEGIN xTEXT; return(ROOT); }
^@stems[\t ]+		 { BEGIN xWORD; return(STEM); }
<xWORD>[^ \t\n]+[\t ]*   { ytdup(); return(WORDSPEC); }
<xWORD>\n		 { BEGIN 0; }

^@form[ \t]+		  { BEGIN xF; return(FORM); }
<xF>%[^ \t\n]+[ \t]*	  { ytdupn(1); return(FLANG); }
<xF>\/[^ \t\n]+[ \t]*	  { ytdupn(1); return(FBASE); }
<xF>\+[^ \t\n]+[ \t]*	  { ytdupn(1); return(FCONT); }
<xF>\$[^ \t\n]+[ \t]*	  { ytdupn(1); return(FNORM); }
<xF>##[^ \t\n]+[ \t]*	  { ytdupn(2); return(FMORPH2); }
<xF>#[^ \t\n]+[ \t]*	  { ytdupn(1); return(FMORPH); }
<xF>\*[^ \t\n]+[ \t]*	  { ytdupn(1); return(FSTEM); }
<xF>[^+$#*/\n][^ \t\n]*[ \t]* { ytdup(); return(FFORM); }
<xF>\n			  { BEGIN 0; return(END_FORM); }

.|\n

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

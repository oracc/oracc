%option never-interactive noyywrap
%{
#include <ctype128.h>
#include "gx.h"
#include "npool.h"
#include "grammar.tab.h"
#define YY_NO_UNISTD_H
#define dup(s) (char*)npool_copy((unsigned char *)(s),curr_cbd->pool)
static char *unspace(char*s);
%}
%x xCF xGW xPOS
%x xPROJ xLANG xTEXT
%%
^@project[\t ]+		 { BEGIN xPROJ; return PROJECT; }
<xPROJ>[^ \t\n]+[ \t]*\n { BEGIN 0; yylval.text = dup(unspace(yytext)); return PROJSPEC; }
^@lang[ \t]+		 { BEGIN xLANG; return LANG; }
<xLANG>[^ \t\n]+[ \t]*\n { BEGIN 0; yylval.text = dup(unspace(yytext)); return LANGSPEC; }
^@name[\t ]+		 { BEGIN xTEXT; return NAME; }
<xTEXT>.+\n 		 { BEGIN 0; yylval.text = dup(unspace(yytext)); return TEXTSPEC; }

^\+?@entry[\t ]+	 { BEGIN xCF; return(ENTRY); }
^@end[ \t]+entry[\t ]*\n { BEGIN 0;   return(END_ENTRY); }

<xCF>[^\[\n]+		{ yylval.text = dup(unspace(yytext)); return(CF); }
<xCF>[ \t]*"["      	{ BEGIN xGW; return(yytext[yyleng-1]); }
<xGW>([^\]]+)		{ yylval.text = strdup(yytext); return(GW); }
<xGW>"]"[ \t]*	       	{ BEGIN xPOS; return(yytext[0]); }
<xPOS>[^ \t\n]+		{ BEGIN 0; yylval.text = strdup(yytext); return(POS); }

.|\n
%%
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

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
%x xPROJ xLANG xTEXT xENTSNS xWHY xCOMMENT
%%
^@project[\t ]+		 { BEGIN xPROJ; return PROJECT; }
<xPROJ>[^ \t\n]+[ \t]*\n { BEGIN 0; yylval.text = dup(unspace(yytext)); return PROJSPEC; }
^@lang[ \t]+		 { BEGIN xLANG; return LANG; }
<xLANG>[^ \t\n]+[ \t]*\n { BEGIN 0; yylval.text = dup(unspace(yytext)); return LANGSPEC; }
^@name[\t ]+		 { BEGIN xTEXT; return NAME; }
<xTEXT>.+\n 		 { BEGIN 0; yylval.text = dup(unspace(yytext)); return TEXTSPEC; }
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

<xCF>[^\[\n]+		 { yylval.text = dup(unspace(yytext)); return(CF); }
<xCF>[ \t]*"["      	 { BEGIN xGW; return(yytext[yyleng-1]); }
<xGW>([^\]]+)		 { yylval.text = strdup(yytext); return(GW); }
<xGW>"]"[ \t]*	       	 { BEGIN xPOS; return(yytext[0]); }
<xPOS>[^ \t\n]+[ \t]*\n	 { BEGIN 0; yylval.text = strdup(unspace(yytext)); return(POS); }
<xPOS>[^ \t\n]+[ \t]*  	 { BEGIN xCF; yylval.text = strdup(unspace(yytext)); return(POS); }

^#why:[\t ]+		 { BEGIN xWHY; return(WHY); }
<xWHY>.+\n		 { BEGIN 0; yylval.text = strdup(unspace(yytext)); return WHYSPEC; }
^#			 { BEGIN xCOMMENT; }
<xCOMMENT>.*\n		 { BEGIN 0; }

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

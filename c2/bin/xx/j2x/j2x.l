%option never-interactive yylineno nounput noyywrap

%{
#include <stdlib.h>
#include <string.h>
#include "j2x.tab.h"
%}

characters	({character}|{escape})*
character 	[\x20\x21\x23-\x5b\x5d-\xff]
escape		"\\"([\\/bfnrt]|[0-9a-fA-F]{4})

number		({integer}{fraction}?{exponent}?)
integer		(-?([0-9]|[1-9][0-9]+))
fraction	("."[0-9]+)
exponent	([eE][-+]?[0-9]+)

ws		[ \n\r\t]+

%x S

%%

"\"\""		{ return EMPTY_STRING; }
"\""		{ BEGIN(S); return '"'; }
<S>{characters} { yylval.s = strdup(yytext); return CHARACTERS; }
<S>"\""		{ BEGIN(0); return '"'; }

{number}	{ yylval.s = strdup(yytext); return NUMBER; }

"["|"]"|"{"|"}"|":"|"," { return *yytext; }

{ws}		{ }

"false"		{ return FALSE; }
"null"		{ return NUL; }
"true"		{ return TRUE; }

%%

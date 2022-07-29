%{
#include <stdio.h>
#include "gx.h"
#define YYDEBUG 1
extern int yylex(void);
void yyerror(char *s);
#define dup(s) npool_copy((unsigned char *)(s),curr_cbd->pool)
%}
%union { char *text; int i; }

%token <text> CF
%token <text> GW
%token <text> LANGSPEC
%token <text> TEXTSPEC
%token <text> POS
%token <text> PROJSPEC

%token ENTRY_B ENTRY_E LANG PROJECT NAME

%start cbd

%%

cbd: header entrylist

header: atproject atlang atname { cbd_setup(curr_cbd); } ;

atproject: PROJECT PROJSPEC { curr_cbd->project = yylval.text; } ;

atlang:    LANG    LANGSPEC { curr_cbd->lang = yylval.text; } ;

atname:    NAME    TEXTSPEC { curr_cbd->name = yylval.text; } ;

entrylist: entry
	| entrylist entry

entry:	entry_b cgp entry_e

entry_b: ENTRY_B { curr_entry = entry_init(curr_cbd); } ;

entry_e: ENTRY_E { printf("end entry %s\n", curr_entry->cgp.closed); curr_entry = NULL; } ;

cgp:    CF '[' GW ']' POS { cgp_init(&curr_entry->cgp, $1, $3, $5); } ;

%%

void
yyerror(char *s)
{
  printf("error: %s\n", s);
}

%{

#include <stdio.h>

#define jxc(x) printf("</%s>",x)
#define jxn(x) printf("<n>%s</n>",x)
#define jxo(x) printf("<%s>",x)
#define jxs(x) printf("%s",x)

extern int yylex(void);
extern void yyerror(const char *);
%}

%union { char *s; int i; }
%token <i> TRUE FALSE NUL EMPTY_STRING
%token <s> CHARACTERS NUMBER

%type <s> string member

%start json

%%

json:
	element

value:
	object
	| array
	| string
	| NUMBER	{ printf("%s", $1); }
	| TRUE		{ printf("true"); }
	| FALSE		{ printf("false"); }
	| NUL		{ printf("null"); }
	;

object:
	'{' 	{ jxo("o"); }
	'}' 	{ jxc("o"); }
	|
	'{' 	{ jxo("o"); }
	 members
	'}' 	{ jxc("o"); }
	;

members:
	member
	| member ',' members
	;

member:
	string { jxn($1); }
	':' 	 
	element 	 { jxc("m"); }
	;

array:
	'[' 		 { jxo("a"); }
	']' 		 { jxc("a"); }
	|
	'[' 		 { jxo("a"); }
	elements
	']' 		 { jxc("a"); }
	;

elements:
	element
	| element ',' elements
	;

element:
	value
	;

string:
	  EMPTY_STRING 	     { }
	| '"' '"' 	     { }
	| '"' CHARACTERS '"' { jxs($2); }
	;

%%

void
yyerror(const char *e)
{
  fprintf(stderr, "j2x: %s\n", e);
}

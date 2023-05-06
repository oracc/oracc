%{

#include <stdio.h>

#define jxc(x) printf("</%s>",x)
#define jxn(x) printf("<n>%s</n>",x)
#define jxo(x) printf("<%s>",x)
#define jxs(x) printf("<s>%s</s>",x)

extern int yylex(void);
extern void yyerror(const char *);
static void vstr(const char *);

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
	| string	{ vstr($1); }
	| NUMBER	{ printf("<v>%s</v>", $1); }
	| TRUE		{ printf("<v>true</v>"); }
	| FALSE		{ printf("<v>false</v>"); }
	| NUL		{ printf("<v>null</v>"); }
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
	string { jxo("m"); jxn($1); }
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
	  EMPTY_STRING 	     { $$=""; }
	| '"' CHARACTERS '"' { $$=$2; }
	;

%%

void
vstr(const char *s)
{
#undef puts
#define puts(s) fputs((s),stdout)
  puts("<v>");
  while (*s)
    {
      if (*s > 127)
	putchar(*s);
      else
	switch (*s)
	  {
	  case '\\':
	    ++s;
	    if ('"' == *s)
	      puts("&quot;");
	    else
	      putchar(*s);
	    break;
	  case '\'':
	    puts("&apos;");
	    break;
	  case '<':
	    puts("&lt;");
	    break;
	  case '>':
	    puts("&gt;");
	    break;
	  case '&':
	    puts("&amp;");
	    break;
	  default:
	    putchar(*s);
	    break;
	  }
      ++s;
    }
  puts("</v>");
}

void
yyerror(const char *e)
{
  fprintf(stderr, "j2x: %s\n", e);
}

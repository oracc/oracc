%define api.prefix {cat}
%file-prefix"cat"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include "cat.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *cattext;
extern int catlineno, cattrace;

#define CATLTYPE_IS_DECLARED 1
/*typedef struct Mloc GDLLTYPE;*/
#define CATLTYPE Mloc
#define yylineno catlineno
CATLTYPE catlloc;

%}

%union { char *text; int i; }

%token	<text> TOK TAB EOL PAR CMT BAD

%start fields

%%

fields: field
	| fields field

field: 	line EOL
	| cont EOL
	| line PAR	{ if (cattrace) fprintf(stderr, "PAR\n");
			  cat_chunk(catlineno-1, ""); }
	| cont PAR	{ if (cattrace) fprintf(stderr, "PAR\n");
			  cat_chunk(catlineno-1, ""); }
	| BAD		{ fprintf(stderr, "%d: lines must begin with '@' or whitespace\n", catlineno); }

line:	TOK		{ if (cattrace) fprintf(stderr, "field/EOL: %s\n", catlval.text);
   			  cat_chunk(catlineno,(char*)catlval.text);
 			}
	| CMT 		{ if (cattrace) fprintf(stderr, "comment/EOL: %s\n", catlval.text);
   			  cat_chunk(catlineno,(char*)catlval.text);
 			}


cont: 	TAB		{ if (cattrace) fprintf(stderr, "field/TAB: %s\n", catlval.text);
    			  cat_cont(catlineno,(char*)catlval.text);
 			}
	| cont TAB

%%

void
caterror(const char *e)
{
  fprintf(stderr, "%s\n", e);
}

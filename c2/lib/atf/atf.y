%define api.prefix {atf}
%file-prefix"atf"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <string.h>
#include "atf.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *atftext, *curratffile;
extern int atflineno, atftrace;
extern int gdl_unicode;

#define ATFLTYPE_IS_DECLARED 1
#define ATFLTYPE Mloc
#define yylineno atflineno
ATFLTYPE atflloc;

%}

%union { char *text; int i; }

%token	<text> TOK TAB EOL PAR CMT BAD LINE SIGLUM

%start fields

%%

fields: blank
	| field
	| fields blank
	| fields field
	;

blank:  EOL
       	| PAR
	;

field: 	line EOL
	| cont EOL
	| line PAR	{ if (atftrace) fprintf(stderr, "PAR\n");
			  cat_chunk(curratffile,atflineno-1, ""); }
	| cont PAR	{ if (atftrace) fprintf(stderr, "PAR\n");
			  cat_chunk(curratffile,atflineno-1, ""); }
	| BAD		{ mesg_warning(curratffile, atflineno, "atf: lines must begin with '@' or whitespace"); }
	;

line:	TOK		{ if (atftrace) fprintf(stderr, "field/EOL: %s\n", atflval.text);
   			  if (!strncmp(atflval.text,"#atf:",strlen("#atf:")) && strstr(atflval.text,"unicode")) { gdl_unicode=1; }
   			  cat_chunk(curratffile,atflineno,(char*)atflval.text);
 			}
	| CMT 		{ if (atftrace) fprintf(stderr, "comment/EOL: %s\n", atflval.text);
   			  cat_chunk(curratffile,atflineno,(char*)atflval.text);
 			}
	;

cont: 	TAB		{ if (atftrace) fprintf(stderr, "field/TAB: %s\n", atflval.text);
    			  cat_cont(atflineno,(char*)atflval.text);
 			}
	| cont TAB
	;

%%

void
atferror(const char *e)
{
  mesg_vwarning(curratffile, atflineno, "atf: %s\n", e);
}

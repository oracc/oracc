%define api.prefix {asl}
%file-prefix"asl"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <string.h>
#include "asl.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *asltext, *curraslfile;
extern int asllineno, asltrace;
extern int gdl_unicode;

#define ASLLTYPE_IS_DECLARED 1
#define ASLLTYPE Mloc
#define yylineno asllineno
ASLLTYPE asllloc;

%}

%union { char *text; int i; }

%token	<text>  TOK TRANS TAB EOL PAR CMT BAD LINE SIGLUM
		SIGN NOSIGN FORM V NOV INOTE ENDS ENDF TEXT

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
	| line PAR	{ if (asltrace) fprintf(stderr, "PAR\n");
			  cat_chunk(curraslfile,asllineno-1, ""); }
	| cont PAR	{ if (asltrace) fprintf(stderr, "PAR\n");
			  cat_chunk(curraslfile,asllineno-1, ""); }
	| BAD		{ mesg_warning(curraslfile, asllineno, "asl: lines must begin with '@' or whitespace"); }
	;

line:	atcmd		{ if (asltrace) fprintf(stderr, "field/EOL: %s\n", asllval.text);
   			  if (!strncmp(asllval.text,"#asl:",strlen("#asl:"))) { asl_protocol(asllval.text); }
   			  cat_chunk(curraslfile,asllineno,(char*)asllval.text);
 			}
	| CMT 		{ if (asltrace) fprintf(stderr, "comment/EOL: %s\n", asllval.text);
   			  cat_chunk(curraslfile,asllineno,(char*)asllval.text);
 			}
	| TRANS        	{ if (asltrace) fprintf(stderr, "trans/EOL: %s\n", asllval.text);
   			  cat_chunk(curraslfile,asllineno,(char*)asllval.text);
 			}
	;

atcmd:
	  atsign
	| atnosign
	| atv
	| atnov
	| form
	| inote
	| end
        ;

atsign:
	  SIGN NAME
	;

atnosign;
	  NOSIGN NAME
	;

atform:
	  FORM VAR NAME
	;

atv
	  V VALUE
	;

end:
	  END FORM
	| END SIGN
	| END BAD
	;

inote:

	  INOTE
        ;

cont: 	TAB		{ if (asltrace) fprintf(stderr, "field/TAB: %s\n", asllval.text);
    			  cat_cont(asllineno,(char*)asllval.text);
 			}
	| cont TAB
	;

%%

void
aslerror(const char *e)
{
  mesg_vwarning(curraslfile, asllineno, "asl: %s\n", e);
}

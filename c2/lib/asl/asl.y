%define api.prefix {asl}
%file-prefix"asl"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <string.h>
#include <mesg.h>
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
		SIGN NOSIGN FORM VAR GNAME GVALUE GBAD
		V NOV VCMT VREF
		INOTE TEXT END EBAD EFORM ESIGN

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
	| line PAR	{ if (asltrace) fprintf(stderr, "PAR\n"); }
	| cont PAR	{ if (asltrace) fprintf(stderr, "PAR\n"); }
	| BAD		{ mesg_warning(curraslfile, asllineno, "asl: lines must begin with '@' or whitespace"); }
	;

line:	atcmd		{ if (asltrace) fprintf(stderr, "field/EOL: %s\n", asllval.text); }
	| CMT 		{ if (asltrace) fprintf(stderr, "comment/EOL: %s\n", asllval.text); }
	;

atcmd:
	  atsign
	| atnosign
	| atv
	| atnov
	| atform
	| atinote
	| atend
        ;

atsign:
	  SIGN GNAME
	| SIGN GBAD
	;

atnosign:
	  NOSIGN GNAME
	| NOSIGN GBAD
	;

atform:
	  FORM VAR GNAME
	| FORM VAR GBAD
	;

atv:
	  V GVALUE vref
	| V VCMT GVALUE vref
	| V GBAD vref
	| V VCMT GBAD vref
	;

atnov:
	  NOV GVALUE
	| NOV GBAD
	;

vref:
	  VREF
	| /* empty */
	;

atend:
	  END EFORM
	| END ESIGN
	| END EBAD
	;

atinote:

	  INOTE
        ;

cont: 	TAB		{ if (asltrace) fprintf(stderr, "field/TAB: %s\n", asllval.text); }
	| cont TAB
	;

%%

void
aslerror(const char *e)
{
  mesg_vwarning(curraslfile, asllineno, "asl: %s\n", e);
}

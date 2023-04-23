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
		SIGN NOSIGN FORM NOFORM VAR GNAME GVALUE GBAD
		V NOV QRYV VCMT VREF LIST LISTNUM
		INOTE TEXT END EBAD EFORM ESIGN
		UCODE UPHASE UNAME

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
	| error EOL	{ yyerrok; }
	;

line:	atcmd		{ if (asltrace) fprintf(stderr, "atcmd/EOL: %s\n", asllval.text); }
	| CMT 		{ if (asltrace) fprintf(stderr, "comment/EOL: %s\n", asllval.text); }
	;

atcmd:
	  atsign
	| atnosign
	| atlist
	| atvv
	| atform
	| atinote
	| atunicode
	| atend
        ;

atsign:
	  SIGN GNAME	{ fprintf(stderr, "sv: %s\n", $2); }
	| SIGN GBAD
	;

atlist:
	  LIST LISTNUM	{ fprintf(stderr, "sv: [list] %s\n", $2); }
	;

atnosign:
	  NOSIGN GNAME
	| NOSIGN GBAD
	;

atform:
	  FORM VAR GNAME
	| FORM VAR GBAD
	;

atvv:
 	  atv
	| atnov
	| atqryv

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

atqryv:
	  QRYV GVALUE
	| QRYV GBAD
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

atunicode:
	  atucode
	| atuname
	| atuphase
	;

atucode:
	  UCODE
	;

atuname:
	  UNAME
	;

atuphase:
	  UPHASE
	;

atinote:
	  INOTE	TEXT
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

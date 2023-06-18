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

int asl_raw_tokens = 1;
int nosign = 0;
 
%}

%union { char *text; int i; }

%token	<text>  TOK TRANS TAB EOL PAR CMT BAD LINE SIGLUM
		SIGN NOSIGN PNAME FORM NOFORM VAR GNAME GVALUE GVALUEQ GVALUEX
		GBAD ATF LANG
		V NOV QRYV ATFV VCMT VREF LIST LISTNUM LISTNUMQ
		INOTE LIT NOTE TEXT END EBAD EFORM ESIGN
		UCHAR UCODE UPHASE UNAME UNOTE

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
	  atsign	{ nosign = 0; }
	| atnosign
	| atpname
	| atlist
	| atvv
	| atform
	| atmeta
	| atunicode
	| atend
        ;

atsign:
	  SIGN gname	
	| SIGN LISTNUM 
	| SIGN GBAD
	;

atpname:
	  PNAME GNAME	{ }
	;

atlist:
	  LIST LISTNUM	{ }
	| LIST LISTNUMQ	{ }
	;

atnosign:
	  NOSIGN GNAME
	| NOSIGN GBAD
	;

atform:
	  FORM VAR gname vref
	| FORM VAR LISTNUM
	| FORM VAR GBAD
	;

atvv:
 	  atv
	| atnov
	| atqryv

atv:
	  V lang gvalue vref
	| V lang GVALUEQ vref
	| V lang GVALUEX vref
	| V lang ATF vref
	| V VCMT ATF vref
	| V VCMT gvalue vref
	| V VCMT GVALUEQ vref
	| V GBAD vref
	| V VCMT GBAD vref
	;

atnov:
	  NOV ATF vref
	| NOV GVALUE vref
	| NOV GVALUEX vref
	| NOV GBAD
	;

atqryv:
	  QRYV GVALUE vref
	| QRYV ATF vref
	| QRYV GBAD
	;

gname:
	  GNAME			{ if (asl_raw_tokens && !nosign) fprintf(stdout, "%s\n", $1); }
	;

gvalue:
	  GVALUE 		{ if (asl_raw_tokens && !nosign) fprintf(stdout, "%s\n", $1); }
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
	  atuchar
	| atucode
	| atuname
	| atunote
	| atuphase
	;

atuchar:
	  UCHAR
	;

atucode:
	  UCODE
	;

atuname:
	  UNAME
	;

atunote:
	  UNOTE	TEXT
        ;

atuphase:
	  UPHASE
	;

atmeta:
	  atinote
	| atlit
	| atnote
	;

atinote:
	  INOTE	TEXT
        ;

atlit:
	  LIT	TEXT
        ;

atnote:
	  NOTE	TEXT
        ;

cont: 	TAB		{ if (asltrace) fprintf(stderr, "field/TAB: %s\n", asllval.text); }
	| cont TAB
	;

lang:
	  LANG
	| /* empty */
	;

%%

void
aslerror(const char *e)
{
  mesg_vwarning(curraslfile, asllineno, "asl: %s\n", e);
}

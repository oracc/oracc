%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include "gdl.h"
extern int yylex(void);
extern void yyerror(const char *);
extern const char *gdltext;
extern int gdllineno, gdltrace;
%}

%union { char *text; int i; }

%token	<text> 	ALIGN FIELD PERIOD HYPHEN PLUS COLON CHARS MOD_AT MOD_TL
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES

%start fields

structure:
	ALIGN
	| FIELD
	| FIELD FTYPE
	;

delim:
	PERIOD
	| HYPHEN
	| L_cur
	| R_cur
	;

state:
	L_ang
	| R_ang
	| L_cur
	| R_cur
	| L_dbl_ang
	| R_dbl_ang
	| L_dbl_cur
	| R_dbl_cur
	;

comment:
	L_inl_dol
	| R_inl_dol
	| L_inl_cmt
	| R_inl_cmt
	;

breakage:
	L_Squ
	| R_Squ
	| L_uhs
	| R_uhs
	| L_lhs
	| R_lhs
	;

grapheme:
	graph
	| graph gmeta
	;

graph:
	CHARS
	| graph breakage CHARS
	| graph gmods

gmeta:
	flags
	| breakage
	| gmeta flags
	| gmeta breakage
	;

gmods:
	MOD_AT
	| MOD_TL
	| gmods MOD_TL

compound:
	C_O cgraphemes C_C
	;

cgraphemes:
	| cgraph cdelim
	| cgraphemes cgraph
	;

cgraph:
	| graph
	| C_L_par cgraphemes C_R_par
	;

cdelim:
	C_PERIOD
	| C_ABOVE
	| C_CROSSING
	| C_OPPOSING
	| C_TIMES
	| C_4TIMES
	| C_3TIMES
	;

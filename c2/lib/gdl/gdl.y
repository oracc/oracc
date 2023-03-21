%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdio.h>
#include "gdl.h"
extern int gdllex(void);
extern void yyerror(const char *);
extern const char *gdltext;
extern int gdllineno, gdltrace;
static Tree *ytp;
static Node *ynp;
%}

%union { char *text; int i; }

%token	<text> 	ALIGN FIELD FTYPE LANG GRAPHEME TEXT SPACE ENHYPHEN
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		L_uhs R_uhs L_lhs R_lhs

%start top

%%

top:	  structure

structure:
	  ALIGN
	| FIELD
	| FIELD FTYPE
	| line
	;

line:
	  lineseg
	| line lineseg
	;

lineseg:
	  words
	| LANG
	| comment
	;

comment:
	  L_inl_dol TEXT R_inl_dol
	| L_inl_cmt TEXT R_inl_cmt
	;

words:    word
	| words SPACE word
	;

word:
	  sorc
	| word delim sorc
	;

delim:
	  '.' 						{ ynp = gdl_delim(ytp, "."); }
        | '-' 						{ ynp = gdl_delim(ytp, "-"); }
	| '+' 						{ ynp = gdl_delim(ytp, "+"); }
	| ':' 						{ ynp = gdl_delim(ytp, ":"); }
	| '{' 						
	| '}' 						
	| '\n'
	| ENHYPHEN 			       		{ ynp = gdl_delim(ytp, "--"); }
	;

sorc:
	  s
	| c
	;

s:	  stateo simplexg gflags statec
	;

c:	  stateo compound gflags statec
	;


simplexg: GRAPHEME					{ ynp = gdl_graph(ytp, gdllval.text); }
	;

gflags:	  '*'						{ gdl_prop(ynp, GP_FLAGS , "*"); }
	| '#'						{ gdl_prop(ynp, GP_FLAGS , "#"); }
	| '!'						{ gdl_prop(ynp, GP_FLAGS , "!"); }
	| '?'						{ gdl_prop(ynp, GP_FLAGS , "?"); }
	| /* empty */
	;

stateo:  
	  '<'						{ gdl_prop(ynp, GP_STATE , "<"); }
	| L_dbl_ang				       	{ gdl_prop(ynp, GP_STATE , "<<"); }
	| L_dbl_cur			       		{ gdl_prop(ynp, GP_STATE , "{{"); }
	| '['						{ gdl_prop(ynp, GP_BREAK , "["); }
	| L_uhs						{ gdl_prop(ynp, GP_BREAK , "[#"); }
	| L_lhs						{ gdl_prop(ynp, GP_BREAK , "[##"); }
        | /* empty */
	;


statec:
	  '>'						{ gdl_prop(ynp, GP_STATE , ">"); }
	| R_dbl_ang			       		{ gdl_prop(ynp, GP_STATE , ">>"); }
	| R_dbl_cur					{ gdl_prop(ynp, GP_STATE , "}}"); }
	| ']'						{ gdl_prop(ynp, GP_BREAK , "]"); }
	| R_uhs						{ gdl_prop(ynp, GP_BREAK , "#]"); }
	| R_lhs						{ gdl_prop(ynp, GP_BREAK , "##]"); }
        | /* empty */
	;

compound:
	C_O 						{ gdl_push(ytp,"g:c"); }
	cword
	C_C 						{ gdl_pop(ytp,"g:c"); }
	;

cword:
	  sorg
	| cword cdelim sorg
        ;

sorg:
          s | g
        ;

g:        '('						{ gdl_push(ytp,"g:gp"); }
	  cword
	  ')'						{ gdl_pop(ytp,"g:gp"); }
	;

cdelim:
	  C_PERIOD					{ ynp = gdl_delim(ytp, "."); }
	| C_ABOVE					{ ynp = gdl_delim(ytp, "&"); }
	| C_COLON					{ ynp = gdl_delim(ytp, ":"); }
	| C_CROSSING					{ ynp = gdl_delim(ytp, "%"); }
	| C_OPPOSING					{ ynp = gdl_delim(ytp, "@"); }
	| C_PLUS					{ ynp = gdl_delim(ytp, "+"); }
	| C_TIMES					{ ynp = gdl_delim(ytp, "×"); }
	| C_3TIMES					{ ynp = gdl_delim(ytp, "3×"); }
	| C_4TIMES					{ ynp = gdl_delim(ytp, "4×"); }
	;

%%

void
gdl_set_tree(Tree *tp)
{
  ytp = tp;
}

void
gdlerror(const char *e)
{
  fprintf(stderr, "%s\n", e);
}

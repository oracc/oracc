%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include "gdl.h"
extern int gdllex(void);
extern void yyerror(const char *);
extern const char *gdltext;
extern int gdllineno, gdltrace;
static Tree *ytp;
static Node *ynp, *yrem;
%}

%union { char *text; int i; }

%token	<text> 	ALIGN FIELD FTYPE LANG TEXT SPACE ENHYPHEN
		GRAPHEME NUMBER LISTNUM PUNCT
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt

%token <i>	'*' '!' '?' '#' '<' '>' '{' '}' '[' ']'
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
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
	  scorqorsgs
	| word delim scorqorsgs
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

scorqorsgs:  stateo corqors gflags statec

corqors:
	  compound
	| simplexg
	| valuqual
	;

gflags:	  '*'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '#'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '!'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '?'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| /* empty */
	;

stateo:  
	  '<'						{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| L_dbl_ang				       	{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| L_dbl_cur			       		{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| '['						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| L_uhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| L_lhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
        | /* empty */
	;

statec:
	  '>'						{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| R_dbl_ang			       		{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| R_dbl_cur					{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| ']'						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| R_uhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| R_lhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
        | /* empty */
	;

simplexg:
	s						{ gvl_simplexg(ynp); }
	;

s:
	  GRAPHEME					{ ynp = gdl_graph(ytp, gdllval.text); }
	| LISTNUM					{ ynp = gdl_listnum(ytp, gdllval.text); }
	| NUMBER					{ ynp = gdl_number(ytp, gdllval.text); }
	| PUNCT						{ ynp = gdl_punct(ytp, gdllval.text); }
	;

compound:
	c	    				       	 { gvl_compound(ytp->curr);
	  						   ynp = gdl_pop(ytp,"g:c"); }
	;

c:
	  C_O 						{ gdl_push(ytp,"g:c"); }
	  cword
	  C_C 						
	  ;

cword:
	  cgors
	| cword cdelim cgors
        ;

cgors:
          cg | simplexg
        ;

cg:       '('						{ gdl_push(ytp,"g:gp"); }
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

cors:
	  compound
	| simplexg
	;

valuqual:
	q	    				       	 { gvl_valuqual(ytp->curr);
	  						   ynp = gdl_pop(ytp,"g:q"); }
	;

q:
	cors
	'(' 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem); }
	cors
	')' 						
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

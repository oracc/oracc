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
%}

%union { char *text; int i; }

%token	<text> 	ALIGN FIELD FTYPE LANG CHARS TEXT SPACE ENHYPHEN
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		L_uhs R_uhs L_lhs R_lhs
		C_L_par C_R_par MOD_AT MOD_TL
%token <i>    '.' '-' '+' ':' '{' '}' '\n'

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

words:    word
	| words SPACE word
	;

comment:
	  L_inl_dol TEXT R_inl_dol
	| L_inl_cmt TEXT R_inl_cmt
	;

delim:
	  '.'
	| '-' 						{ fprintf(stderr, "DELIM: %c\n", '-');
	  						  (void)tree_add(ytp, "g:delim", ytp->curr->depth, NULL); 
	  						  ytp->curr->data = "-"; }
	| '+'
	| ':'
	| '{'
	| '}'
	| '\n'
	| ENHYPHEN
	;

word:
	  grapheme
	| word delim grapheme
	;

grapheme:
	  graph
	| graph gmeta
	| compound
	;

graph:
	  CHARS						{ fprintf(stderr, "CHARS: %s\n", gdllval.text);
	  						  (void)tree_add(ytp, "g:chars", ytp->curr->depth, NULL);
	  						  ytp->curr->data = (ccp)pool_copy((uccp)gdllval.text,gdlpool); }
	| graph breakage CHARS
	| graph gmods breakage
	;

gmeta:
	  flags
	| state
	| gmeta flags
	| gmeta state
	;

state:
	  '<'
	| '>'
	| L_dbl_ang
	| R_dbl_ang
	| L_dbl_cur
	| R_dbl_cur
	;

breakage:
	  '['
	| ']'
	| L_uhs
	| R_uhs
	| L_lhs
	| R_lhs
	;

flags:	  '*' | '#' | '!' | '?' ;

gmods:
	  MOD_AT
	| MOD_TL
	| MOD_AT MOD_TL
	;

compound:
	C_O cgraphemes C_C
	;

cgraphemes:
	  graph cdelim
	| cgraphemes graph
	| C_L_par cgraphemes C_R_par
	;

cdelim:
	C_PERIOD
	| C_ABOVE
	| C_COLON
	| C_CROSSING
	| C_OPPOSING
	| C_PLUS
	| C_TIMES
	| C_4TIMES
	| C_3TIMES
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

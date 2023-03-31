%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include "gdl.h"
extern int gdllex(void);
extern void yyerror(const char *);
extern const char *gdltext, *currgdlfile;
extern int gdllineno, gdltrace;
static Tree *ytp;
static Node *ynp, *yrem;

#define GDLLTYPE_IS_DECLARED 1
#define GDLLTYPE Mloc
#define yylineno gdllineno
GDLLTYPE gdllloc;
%}

%union { char *text; int i; }

%token	<text> 	FTYPE LANG TEXT SPACE ENHYPHEN ELLIPSIS
		GRAPHEME NUMBER LISTNUM PUNCT
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		LF_HASH LF_QUOTE LF_STAR LF_TILDE

%token <i>	'*' '!' '?' '#' '<' '>' '{' '}' '[' ']'
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
		L_uhs R_uhs L_lhs R_lhs
		f_L_uhs f_R_uhs f_L_lhs f_R_lhs
		f_L_ang f_R_ang f_L_cur f_R_cur f_L_sqb f_R_sqb
       	        f_L_dbl_ang f_R_dbl_ang f_L_dbl_cur f_R_dbl_cur

%start line

%%

line:
	lineparts
	| line lineparts
      	;

lineparts:
	  cell
	| typedseg
	| plainseg
	;

cell:
	'&' anyseg
	;

anyseg:
	typedseg
	| plainseg
	;

typedseg:
	field plainseg
	;

field:
	  ','
	| ',' FTYPE
	| FTYPE
	| LF_HASH
	| LF_QUOTE
	| LF_STAR
	| LF_TILDE
	;

plainseg:
	SPACE
	| lang
	| word
	| comment
	| freemeta
	;

freemeta:
	  fstateo
	| fstatec
	;

lang:
	LANG
	;

comment:
	  L_inl_dol TEXT R_inl_dol
	| L_inl_cmt TEXT R_inl_cmt
	;

word:
	  corqorsg
	| corqorsg dwords
	;

dwords:
	  dword
	| dwords dword
	;

dword:
	  delims corqorsg
	;

delims:
	  delim
	| delims delim
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
	| statec
	| stateo
	;

corqorsg:  corqors maybegflags

corqors:
	  compound
	| simplexg
	| valuqual
	;

maybegflags:
	gflags
	| /* empty */
	;

gflags:
	  gflag
	| gflags gflag
	;

gflag:
	  '*'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '#'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '!'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	| '?'						{ gdl_prop(ynp, $1, PG_GDL_FLAGS, NULL, NULL); }
	;

fstateo:  
	  f_L_ang	       				{ ynp = gdl_state(ytp, gdllval.text); }
	| f_L_dbl_ang				       	{ ynp = gdl_state(ytp, gdllval.text); }
	| f_L_dbl_cur			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| f_L_sqb      					{ ynp = gdl_state(ytp, gdllval.text); }
	| f_L_uhs	       				{ ynp = gdl_state(ytp, gdllval.text); }
	| f_L_lhs		      			{ ynp = gdl_state(ytp, gdllval.text); }
	;

fstatec:
	  f_R_ang	       				{ ynp = gdl_state(ytp, gdllval.text); }
	| f_R_dbl_ang			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| f_R_dbl_cur					{ ynp = gdl_state(ytp, gdllval.text); }
	| f_R_sqb		       			{ ynp = gdl_state(ytp, gdllval.text); }
	| f_R_uhs			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| f_R_lhs				      	{ ynp = gdl_state(ytp, gdllval.text); }
	;


stateo:  
	  '<'						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_dbl_ang				       	{ ynp = gdl_state(ytp, gdllval.text); }
	| L_dbl_cur			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| '['						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_uhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_lhs						{ ynp = gdl_state(ytp, gdllval.text); }
	;

statec:
	  '>'						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_dbl_ang			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| R_dbl_cur					{ ynp = gdl_state(ytp, gdllval.text); }
	| ']'						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_uhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_lhs						{ ynp = gdl_state(ytp, gdllval.text); }
	;

simplexg:
	  s						{ gvl_simplexg(@1, ynp); }
	;

s:
	  GRAPHEME					{ ynp = gdl_graph(ytp, gdllval.text); }
	| LISTNUM					{ ynp = gdl_listnum(ytp, gdllval.text); }
	| NUMBER					{ ynp = gdl_number(ytp, gdllval.text); }
	| PUNCT						{ ynp = gdl_punct(ytp, gdllval.text); }
	| ELLIPSIS					{ ynp = gdl_nongraph(ytp, gdllval.text); }
	;

compound:
	c	    				       	 { gvl_compound(@1, ytp->curr);
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
          sg | cg
        ;

sg:
	simplexg maybegflags;

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
	q	    				       	 { gvl_valuqual(@1, ytp->curr);
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
  mesg_vwarning(currgdlfile, gdllineno, "gdl: %s\n", e);
}

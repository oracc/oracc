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
int Q = 0;

#define GDLLTYPE_IS_DECLARED 1
#define GDLLTYPE Mloc
#define yylineno gdllineno
GDLLTYPE gdllloc;
%}

%union { char *text; int i; }

%token	<text> 	FTYPE LANG TEXT ENHYPHEN ELLIPSIS
		GRAPHEME NUMBER LISTNUM PUNCT
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		LF_HASH LF_QUOTE LF_STAR LF_TILDE

%token <i>	'*' '!' '?' '#' '<' '>' '{' '}' '[' ']' '(' ')' CLP CRP QLP QRP
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur L_ang_par R_ang_par
		L_uhs R_uhs L_lhs R_lhs
		SPACE EOL EOFI

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
	  comment
	| space
	| transliteration
	;

comment:
	  L_inl_dol TEXT R_inl_dol
	| L_inl_cmt TEXT R_inl_cmt
	;

space:
	  SPACE
	| EOL
	| EOFI
	;

transliteration:
	  delim
	| grapheme
	| lang
        | meta
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

grapheme:
	scgrapheme
	| valuqual
	;

scgrapheme:
	simplexg
	| compound
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

simplexg:
	  s maybegflags	       				{ gvl_simplexg(@1, ynp); }
	;

s:
	  GRAPHEME					{ ynp = gdl_graph(ytp, gdllval.text); }
	| LISTNUM					{ ynp = gdl_listnum(ytp, gdllval.text); }
	| NUMBER					{ ynp = gdl_number(ytp, gdllval.text); }
	| PUNCT						{ ynp = gdl_punct(ytp, gdllval.text); }
	| ELLIPSIS					{ ynp = gdl_nongraph(ytp, gdllval.text); }
	;

compound:
	  c maybegflags         			{ gvl_compound(@1, ytp->curr);
	  						   ynp = gdl_pop(ytp,"g:c"); }
	;

c:
	  C_O 						{ gdl_push(ytp,"g:c"); }
	  cbits
	  C_C 						
	  ;

cbits:
	  cbit
        | cbits cbit
	;

cbit:
	  simplexg
	| cdelim
	| CLP						{ gdl_push(ytp,"g:gp"); }
	| CRP						{ gdl_pop(ytp,"g:gp");  }
	| meta
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

valuqual:
	q	    				       	 { gvl_valuqual(@1, ytp->curr);
	  						   ynp = gdl_pop(ytp,"g:q"); }
	;

q:
	scgrapheme
	QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem); }
	scgrapheme     					{ gdl_remove_q_error(@1); }
	QRP maybegflags
	;

lang:
	LANG
	;

meta:
	  statec
	| stateo
	;

stateo:  
	  '<'						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_ang_par				       	{ ynp = gdl_state(ytp, gdllval.text); }
	| L_dbl_ang				       	{ ynp = gdl_state(ytp, gdllval.text); }
	| L_dbl_cur			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| '['						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_uhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| L_lhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| '('						{ ynp = gdl_state(ytp, gdllval.text); }
	;

statec:
	  '>'						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_ang_par				       	{ ynp = gdl_state(ytp, gdllval.text); }
	| R_dbl_ang			       		{ ynp = gdl_state(ytp, gdllval.text); }
	| R_dbl_cur					{ ynp = gdl_state(ytp, gdllval.text); }
	| ']'						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_uhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| R_lhs						{ ynp = gdl_state(ytp, gdllval.text); }
	| ')'						{ ynp = gdl_state(ytp, gdllval.text); }
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

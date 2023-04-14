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
static Node *ynp, *yrem, *ycp;
/*int Q = 0;*/
int gdl_legacy = 0;
int gdl_unicode = 0;

#define GDLLTYPE_IS_DECLARED 1
#define GDLLTYPE Mloc
#define yylineno gdllineno
GDLLTYPE gdllloc;

%}

%union { char *text; int i; }

%token	<text> 	FTYPE LANG TEXT ENHYPHEN ELLIPSIS NOTEMARK CELLSPAN
		GRAPHEME NUMBER BARENUM LISTNUM PUNCT MOD INDENT NEWLINE
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		',' LF_AT LF_CARET LF_EQUALS LF_HASH LF_QUOTE LF_TILDE LF_VBAR

%token <i>	'*' '!' '?' '#' '<' '>' '{' '}' '[' ']' '(' ')' CLP CRP QLP QRP
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur L_ang_par R_ang_par
		L_uhs R_uhs L_lhs R_lhs LANG_FLIP
		SPACE EOL END

%nterm <text> 	field lexfld

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
	cellspec anyseg
	;

cellspec:
	'&'					{ gdl_cell(ytp,"1"); }
	| CELLSPAN				{ gdl_cell(ytp,$1); }
	;				
					
anyseg:
	typedseg
	| plainseg
	;

typedseg:
	field plainseg
	;

field:
	  ','					    { gdl_field(ytp,"default"); }
	| ',' FTYPE				    { gdl_field(ytp,$2); }
	| FTYPE					    { gdl_field(ytp,$1); }
        | lexfld				    { gdl_field(ytp,gdl_lexfld_name($1)); }
        ;

lexfld:
	  LF_AT
	| LF_CARET
	| LF_EQUALS
	| LF_HASH
	| LF_QUOTE
	| LF_TILDE
	| LF_VBAR
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
	  SPACE						{ ynp = gdl_delim(ytp, " ");
	    						  gdl_prop(ynp, GP_ATTRIBUTE, PG_GDL_INFO,
								   "literal", gdllval.text); }
	| EOL
	| END
	;

transliteration:
	  delim
	| grapheme
	| lang
        | meta
	| alternative
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

alternative:
	  grapheme '/' grapheme
	  | alternative '/' grapheme
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
	| mods
	| mods gflags
	| /* empty */
	;

gflags:
	  gflag
	| gflags gflag
	;

gflag:
	  '*'						{ gdl_prop(ynp, '*', PG_GDL_FLAGS, NULL, NULL); }
	| '#'						{ gdl_prop(ynp, '#', PG_GDL_FLAGS, NULL, NULL); }
	| '!'						{ gdl_prop(ynp, '!', PG_GDL_FLAGS, NULL, NULL); }
	| '?'						{ gdl_prop(ynp, '?', PG_GDL_FLAGS, NULL, NULL); }
	;

simplexg:
	  s maybegflags	       			       	{ if (gdl_legacy) gdl_unlegacy(ynp);
	    			  			  gvl_simplexg(@1, ynp); }
	;

s:
	  GRAPHEME					{ ynp = gdl_graph(ytp, gdllval.text); }
	| LISTNUM					{ ynp = gdl_listnum(ytp, gdllval.text); }
	| NUMBER					{ ynp = gdl_number(ytp, gdllval.text); }
	| BARENUM					{ ynp = gdl_barenum(ytp, gdllval.text); }
	| PUNCT						{ ynp = gdl_punct(ytp, gdllval.text); }
	| ELLIPSIS					{ ynp = gdl_nongraph(ytp, gdllval.text); }
	;

compound:
	  c maybegflags         			{ gvl_compound(@1, ytp->curr);
	    						  ynp = gdl_pop(ytp,"g:c"); }
	;

c:
	  C_O 						{ ycp = gdl_push(ytp,"g:c"); }
	  cbits
	  C_C 						{ ynp = ycp; }
	  ;

cbits:
	  cbit
        | cbits cbit
	;

cbit:
	  s	       					{ gvl_simplexg(@1, ynp); }
	| gflag
	| cdelim
	| CLP						{ gdl_push(ytp,"g:gp"); }
	| CRP	     		  			{ gdl_pop(ytp,"g:gp");  }
	| QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_remove_q_error(@1, yrem);
  							  gdl_incr_qin();
							  gdl_corrq
							    = (prop_find_pg(yrem->props,'!',PG_GDL_FLAGS)!=NULL);}
	| QRP				      		{ gdl_decr_qin(); }
	| meta
	| mod
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
	  						   ynp = ytp->curr; }
	qmaybemodflags					 { ynp = gdl_pop(ytp,"g:q"); }
        ;

q:
	grapheme
	QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_incr_qin();
							  gdl_corrq
							    = (prop_find_pg(yrem->props,'!',PG_GDL_FLAGS)!=NULL);}
	grapheme 	 	       			{ gdl_remove_q_error(@1, yrem); }
	QRP { gdl_decr_qin(); }		      		
	;

qmaybemodflags:
	  gflags
	| mods
	| mods gflags
	| /* empty */
	;

lang:
	  LANG						{ ynp = gdl_lang(ytp, gdllval.text); }
	| LANG_FLIP
	;

meta:
	  statec
	| stateo
	| INDENT       					{ ynp = gdl_nongraph(ytp, ";"); }
	| NEWLINE	       				{ ynp = gdl_nongraph(ytp, ";"); }
	| NOTEMARK
	;

mods:
	  mod
	| mods mod
        ;

mod:
	  MOD						{ gdl_mod(ytp, gdllval.text); }
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

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
static Node *ynp, *yrem, *ycp, *mnp;
/*int Q = 0;*/
int gdl_legacy = 0;
int gdl_lexical = 0;
int gdl_unicode = 0;

extern int c_delim_sentinel;

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

%token <i>	'*' '!' '?' '#' '<' '>' '[' ']' '(' ')' CLP CRP QLP QRP
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
		L_ang_par R_ang_par L_ang_par_s R_ang_par_s L_cur_par R_cur_par
		L_uhs R_uhs L_lhs R_lhs LANG_FLIP
		 '{' DET_SEME DET_PHON '}'
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
								   "literal", gdllval.text);
							  /*gdl_balance_ws(@1);*/ }
	| '\n'						{ gdl_balance_flush(@1); }
	| END						{ gdl_balance_flush(@1); }
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
	| '{'	      					{ gdl_balance_state(@1,'{',"{");
	    						  gdl_push(ytp,"g:det");
	  						  gdl_gp_type(ytp,GP_DET_SEMI);}
	| DET_SEME    					{ gdl_balance_state(@1,'{',"{");
	    						  gdl_push(ytp,"g:det");
	  						  gdl_gp_type(ytp,GP_DET_SEME); }
	| DET_PHON      	      			{ gdl_balance_state(@1,'{',"{");
	    						  gdl_push(ytp,"g:det"); 
	  						  gdl_gp_type(ytp,GP_DET_PHON); }
	| '}' 	 		  			{ if (!gdl_balance_state(@1,'}',"}")) gdl_pop(ytp,"g:det");  }
	| ENHYPHEN 			       		{ ynp = gdl_delim(ytp, "--"); }
	;

alternative:
	  grapheme '/' grapheme				/*TODO; what about GDL 1.0 [+:]-groups?*/
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
	  s maybegflags	       			       	{ ynp->mloc = mloc_mloc(&@1);
           					          if (gdl_legacy) gdl_unlegacy(ynp);
							  if (ynp->kids) gdl_mod_wrap(ynp, 1);
	    			  			  gvl_simplexg(ynp); }
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
	  c maybegflags         			{ ynp->mloc = mloc_mloc(&@1);
	    						  gdl_modq_flush();
							  gvl_compound(ycp);
	    						  ynp = gdl_pop(ytp,"g:c"); }
	;

c:
	C_O 						{ ycp = gdl_push(ytp,"g:c"); c_delim_sentinel = 0; }
	  cbits
	  C_C 						{ ynp = ycp; }
	  ;

cbits:
	  cbit
        | cbits cbit
	;

cbit:
	s	       			       		{ ynp->mloc = mloc_mloc(&@1);
	  						  if (gdl_legacy) gdl_unlegacy(ynp);
							  gvl_simplexg(ynp); }
	| gflag
	| cdelim					{ ++c_delim_sentinel; }
	| CLP			       			{ gdl_balance_state(@1,CLP,"(");
	    					  	  gdl_push(ytp,"g:gp"); }
	| CRP	     			    		{ if (!gdl_balance_state(@1,CRP,")"))
	      					    	   gdl_pop(ytp,"g:gp");  }
	| QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_remove_q_error(@1, yrem);
  							  gdl_incr_qin();
							  /* IS GDL_CORRQ NECESSARY NOW? */
							  gdl_corrq
							    = (prop_find_pg(yrem->props,'!',
									    PG_GDL_FLAGS)!=NULL);}
	| QRP				      		{ gdl_decr_qin();
	  						  ynp->mloc = mloc_mloc(&@1);
	  						  gvl_valuqual(ytp->curr);
							  ynp = gdl_pop(ytp,"g:q");
							}
	| meta
	| mod						{ gdl_modq_add(mnp); mnp = NULL; };
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
	q	    				       	 { ynp->mloc = mloc_mloc(&@1);
	  						   gvl_valuqual(ytp->curr);
	  						   ynp = ytp->curr; }
	qmaybemodflags					 { gdl_mod_wrap_q(ynp);
							   ynp = gdl_pop(ytp,"g:q"); }
        ;

q:
	grapheme
	QLP 						{ yrem=kids_rem_last(ytp);
	    						  gdl_push(ytp,"g:q");
							  kids_add_node(ytp,yrem);
							  gdl_incr_qin();
							  /* IS GDL_CORRQ NECESSARY NOW? */
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
	| LANG_FLIP					/*TODO; ALSO #atf: lang akk _%s_ vel sim*/
	;

meta:
	  breakc
	| breako
	| glossc
	| glosso
	| statec
	| stateo
	| INDENT       					{ ynp = gdl_nongraph(ytp, ";"); }
	| NEWLINE	       				{ ynp = gdl_nongraph(ytp, "//"); }
	| NOTEMARK					/*TODO*/
	;

mods:
	  mod
	| mods mod
        ;

mod:
	  MOD						{ ynp->mloc = mloc_mloc(&@1);
	  						  mnp = gdl_mod(ytp, gdllval.text); }
	;

breako:
	  '['				{ ynp = gdl_break_o(@1, ytp, '[', gdllval.text, GP_BREAK_FULL); }
	| L_uhs				{ ynp = gdl_break_o(@1, ytp, L_uhs, gdllval.text, GP_BREAK_PART); }
	| L_lhs				{ ynp = gdl_break_o(@1, ytp, L_lhs, gdllval.text, GP_BREAK_PART); }
	;

breakc:
	  ']'				{ ynp = gdl_break_c(@1, ytp, ']', gdllval.text); }
	| R_uhs				{ ynp = gdl_break_c(@1, ytp, R_uhs, gdllval.text); }
	| R_lhs				{ ynp = gdl_break_c(@1, ytp, R_lhs, gdllval.text); }
	;

glosso:
	  L_cur_par		       	{ ynp = gdl_gloss_o(@1, ytp, L_cur_par, gdllval.text, GP_GLOSS_DOCGLO); }
	| L_dbl_cur			{ ynp = gdl_gloss_o(@1, ytp, L_dbl_cur, gdllval.text, GP_GLOSS_LING); }
	| L_ang_par_s		       	{ ynp = gdl_gloss_o(@1, ytp, L_ang_par, gdllval.text, GP_GLOSS_SURRO); }
	;

glossc:
	  R_cur_par		       	{ ynp = gdl_gloss_c(@1, ytp, R_cur_par, gdllval.text); }
	| R_dbl_cur			{ ynp = gdl_gloss_c(@1, ytp, R_dbl_cur, gdllval.text); }
	| R_ang_par_s		       	{ ynp = gdl_gloss_c(@1, ytp, R_ang_par, gdllval.text); }
	;

stateo:  
	  '<'				{ ynp = gdl_state_o(@1, ytp, '<', gdllval.text, GP_STATE_IMPLIED); }
	| '('				{ ynp = gdl_state_o(@1, ytp, '(', gdllval.text, GP_STATE_MAYBE); }
	| L_ang_par		       	{ ynp = gdl_state_o(@1, ytp, L_ang_par, gdllval.text, GP_STATE_SUPPLIED); }
	| L_dbl_ang		       	{ ynp = gdl_state_o(@1, ytp, L_dbl_ang, gdllval.text, GP_STATE_EXCISED); }
	;

statec:
	  '>'				{ ynp = gdl_state_c(@1, ytp, '>', gdllval.text); }
	| ')'				{ ynp = gdl_state_c(@1, ytp, ')', gdllval.text); }
	| R_ang_par		       	{ ynp = gdl_state_c(@1, ytp, R_ang_par, gdllval.text); }
	| R_dbl_ang			{ ynp = gdl_state_c(@1, ytp, R_dbl_ang, gdllval.text); }
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

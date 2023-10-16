%define api.prefix {gdl}
%file-prefix"gdl"
%locations
%define parse.error verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include <mesg.h>
#include "gdl.h"
#include "gdlstate.h"
extern int gdllex(void);
extern void yyerror(const char *);
extern const char *gdltext, *currgdlfile;
extern int gdllineno, gdltrace;

extern gdlstate_t gst; 	/* global gdl state */
extern Node *lgp;   		/* last grapheme node pointer */

static Tree *ytp;
static Node *ynp, *yrem, *ycp, *mnp;
/*int Q = 0;*/
int gdl_bilingual = 0;
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
		GRAPHEME NNUM NUMBER BARENUM LISTNUM PUNCT MOD INDENT NEWLINE
		C_O C_C C_PERIOD C_ABOVE C_CROSSING C_OPPOSING C_COLON C_PLUS
		C_TIMES C_4TIMES C_3TIMES CMOD
		L_inl_dol R_inl_dol L_inl_cmt R_inl_cmt
		',' LF_AT LF_CARET LF_EQUALS LF_HASH LF_QUOTE LF_TILDE LF_VBAR

%token <i>	'*' '!' '?' '#' '<' '>' '[' ']' '(' ')' CLP CRP QLP QRP
       	        L_dbl_ang R_dbl_ang L_dbl_cur R_dbl_cur
		L_dbl_par R_dbl_par L_dbl_par_c R_dbl_par_c eras_canc_pivot
		L_ang_par R_ang_par L_ang_par_s R_ang_par_s L_cur_par R_cur_par
		L_uhs R_uhs L_lhs R_lhs LANG_FLIP
		'{' DET_SEME DET_PHON '}'
		PLUS_FLAG UFLAG1 UFLAG2 UFLAG3 UFLAG4 SPACE EOL END

%nterm <text> 	field lexfld cmods

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
	'&'					    	{ gdl_cell(ytp,"1"); }
	| CELLSPAN				    	{ gdl_cell(ytp,$1); }
	;				
					
anyseg:
	typedseg
	| plainseg
	;

typedseg:
	field plainseg
	;

field:
	  ','					    	{ gdl_field(ytp,"default"); }
	| ',' FTYPE				    	{ gdl_field(ytp,$2); }
	| FTYPE					    	{ gdl_field(ytp,$1); }
        | lexfld				    	{ gdl_field(ytp,gdl_lexfld_name($1)); }
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
	    						  gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO,
								      "literal", gdllval.text); }
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
	  						  gs_on(gs_det|gs_det_o);
	  						  gs_on(gs_g_semd_i); }
	| DET_SEME    					{ gdl_balance_state(@1,'{',"{");
	    						  gdl_push(ytp,"g:det");
	  						  gs_on(gs_det|gs_det_o);
	  						  gs_on(gs_g_semd_e); }
	| DET_PHON      	      			{ gdl_balance_state(@1,'{',"{");
	    						  gdl_push(ytp,"g:det"); 
	  						  gs_on(gs_det|gs_det_o);
	  						  gs_on(gs_g_phond); }
	| '}' 	 		  			{ if (!gdl_balance_state(@1,'}',"}"))
	      						    gdl_pop(ytp,"g:det");
	     						    /* set pst->det = SB_CL; lgp is last
							       node with g content or equivalent, i.e.,
							       where the closer state must be set */
	    						    gdl_update_state(lgp, gs_det_c);
							    gs_no(gs_det|gs_g_semd_e|gs_g_semd_i|gs_g_phond);
	  						  }
	| ENHYPHEN 			       		{ ynp = gdl_delim(ytp, "--"); }
	;

alternative:
	    grapheme '/' grapheme			/*TODO; what about GDL 1.0 [+:]-groups?*/
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
	  '*'						{ gdl_update_state(lgp, gs_f_star); }
	| '#'						{ gdl_update_state(lgp, gs_f_hash); }
	| '!'						{ gdl_update_state(lgp, gs_f_bang); }
	| '?'						{ gdl_update_state(lgp, gs_f_query); }
	| PLUS_FLAG    					{ gdl_update_state(lgp, gs_f_plus); }
	;

simplexg:
	  s maybegflags	       			       	{ /*ynp->mloc = mloc_mloc(&@1);*/ /*should now be covered in gdl_graph_node*/
           					          if (gdl_legacy) gdl_unlegacy(ynp);
							  if (ynp->kids) gdl_mod_wrap(ynp, 1);
	    			  			  gvl_simplexg(ynp); }
	;

s:
	  GRAPHEME					{ ynp = gdl_graph(&@1, ytp, gdllval.text); }
	| LISTNUM					{ ynp = gdl_listnum(&@1, ytp, gdllval.text); }
	| NNUM						{ ynp = gdl_nnum(&@1, ytp, gdllval.text); }
	| NUMBER					{ ynp = gdl_number(&@1, ytp, gdllval.text); }
	| BARENUM					{ ynp = gdl_barenum(&@1, ytp, gdllval.text); }
	| PUNCT						{ ynp = gdl_punct(&@1, ytp, gdllval.text); }
	| ELLIPSIS					{ ynp = gdl_nongraph(&@1, ytp, gdllval.text); }
	;

compound:
	  c cmaybemodflags         			{ ycp->mloc = ynp->mloc = mloc_mloc(&@1);
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
	s	       			       		{ /*ynp->mloc = mloc_mloc(&@1);*/
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
							}
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

cmaybemodflags:
	  gflags
	| cmods
	| cmods gflags
	| /* empty */
	;

cmods:
	  MOD						{ mnp = gdl_mod_cmod(&@1, ynp, NULL, $1); }
	| cmods MOD					{ (void)gdl_mod_cmod(&@1, ynp, mnp, $1); }
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
							  gdl_incr_qin(); }
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
	| INDENT       					{ ynp = gdl_nongraph(&@1, ytp, ";"); }
	| NEWLINE	       				{ ynp = gdl_nongraph(&@1, ytp, "//"); }
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
	  '['				{ ynp = gdl_break_o(@1, ytp, '[', gs_lost_o, gdllval.text); }
	| L_uhs				{ ynp = gdl_break_o(@1, ytp, L_uhs, gs_damaged_o, gdllval.text); }
	| L_lhs				{ ynp = gdl_break_o(@1, ytp, L_lhs, gs_damaged_o, gdllval.text); }
	;

breakc:
	  ']'				{ ynp = gdl_break_c(@1, ytp, ']', gs_lost_c, gdllval.text); }
	| R_uhs				{ ynp = gdl_break_c(@1, ytp, R_uhs, gs_damaged_c, gdllval.text); }
	| R_lhs				{ ynp = gdl_break_c(@1, ytp, R_lhs, gs_damaged_c, gdllval.text); }
	;

glosso:
	  L_cur_par		       	{ ynp = gdl_gloss_o(@1, ytp, L_cur_par, gs_glodoc_o, gdllval.text); }
	| L_dbl_cur			{ ynp = gdl_gloss_o(@1, ytp, L_dbl_cur, gs_glolin_o, gdllval.text); }
	| L_ang_par_s		       	{ ynp = gdl_gloss_o(@1, ytp, L_ang_par, gs_surro_o, gdllval.text); }
	;

glossc:
	  R_cur_par		       	{ ynp = gdl_gloss_c(@1, ytp, R_cur_par, gs_glodoc_c, gdllval.text); }
	| R_dbl_cur			{ ynp = gdl_gloss_c(@1, ytp, R_dbl_cur, gs_glolin_c, gdllval.text); }
	| R_ang_par_s		       	{ ynp = gdl_gloss_c(@1, ytp, R_ang_par, gs_surro_c, gdllval.text); }
	;

stateo:  
	  '<'				{ ynp = gdl_state_o(@1, ytp, '<', gs_supplied_o, gdllval.text); }
	| '('				{ ynp = gdl_state_o(@1, ytp, '(', gs_maybe_o, gdllval.text); }
	| L_ang_par		       	{ ynp = gdl_state_o(@1, ytp, L_ang_par, gs_implied_o, gdllval.text); }
	| L_dbl_ang		       	{ ynp = gdl_state_o(@1, ytp, L_dbl_ang, gs_excised_o, gdllval.text); }
	;

statec:
	  '>'				{ ynp = gdl_state_c(@1, ytp, '>', gs_supplied_c, gdllval.text); }
	| ')'				{ ynp = gdl_state_c(@1, ytp, ')', gs_maybe_c, gdllval.text); }
	| R_ang_par		       	{ ynp = gdl_state_c(@1, ytp, R_ang_par, gs_implied_c, gdllval.text); }
	| R_dbl_ang			{ ynp = gdl_state_c(@1, ytp, R_dbl_ang, gs_excised_c, gdllval.text); }
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

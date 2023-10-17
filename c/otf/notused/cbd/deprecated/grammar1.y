%name-prefix tg1
%locations
%define parse.error verbose
%{
#include <stdio.h>
#include "gx.h"

static struct f2 *curr_form;
static struct meta *curr_meta;
static struct sense *curr_sense;
static struct pleiades *curr_pleiades;

int parser_status = 0;
int yydebug = 0;

int bang = 0, star = 0;
extern int yylex(void);
#define dup(s) npool_copy((unsigned char *)(s),curr_cbd->pool)
%}
%union { char *text; int i; }

%token	ENDOF  0
%token  <text> 		CF
%token  <text> 		GW
%token  <text> 		LANGSPEC
%token  <text> 		POS
%token  <text> 		TEXTSPEC
%token  <text> 		PROJSPEC
%token	<text>		OIDSPEC
%token	<text>		DCF
%token	<text>		SENSE
%token	<text>		LANG
%token	<text>		BASE_PRI
%token	<text>		BASE_ALT
%token  <text> 		FFORM
%token  <text> 		FLANG
%token  <text> 		FRWS
%token  <text> 		FBASE
%token  <text> 		FSTEM
%token  <text> 		FCONT
%token  <text> 		FMORPH
%token  <text> 		FMORPH2
%token  <text> 		FNORM
%token	<text> 		PL_ID
%token	<text>		PL_COORD
%token	<text>		PL_ALIAS
%token	<i>		BIB
%token	<i>		OID
%token	<i>		COLLO
%token	<i>		PROP
%token	<i>		NOTE
%token	<i>		INOTE
%token	<i>		ISSLP
%token	<i>		EQUIV

%token ENTRY END_ENTRY PROJECT NAME PROPS ALIAS BASES
       FORM END_FORM MERGE PARTS RENAME CMTWHY ALLOW EOL

%start cbd

%%

cbd: 		header entrylist { return(parser_status); }
	|      	header  	 { return(parser_status); }
	|	/* empty */ 	 { return(parser_status); }

header:		reqheader
	|	reqheader optheader

reqheader: 	atproject atlang atname

optheader:	atprops
	|	optheader atprops

atproject: PROJECT PROJSPEC { curr_cbd->project = (ucp)yylval.text; }

atlang:    LANG    LANGSPEC { curr_cbd->lang = (ucp)yylval.text; }

atname:    NAME    TEXTSPEC { curr_cbd->name = (ucp)yylval.text; }

atprops:	PROPS TEXTSPEC { bld_props(curr_cbd, yylval.text); }

cgplist: cgp
	 | cgplist cgp

cgp:    CF '[' GW ']' POS { cgp_save((ucp)$1, (ucp)$3, (ucp)$5); } ;
	|	CF '[' GW ']' EOL { lyyerror(@1, "expected POS but found end of line"); }
	|	CF '[' GW EOL { lyyerror(@1, "missing ']' (and maybe POS) after GW"); }
	|	CF ']' POS { lyyerror(@1, "missing '[' before GW"); }

entrylist:	entry
	|	entrylist entry

entry:		atentry blocks end_entry
	|	atentry modentry blocks end_entry

blocks:	any_block
	|	blocks any_block
		
any_block:	entry_block
	|	lang_block
	|	senses_block
	|	meta

entry_block:    alias
	|	DCF TEXTSPEC 	{ bld_dcf(@1,curr_entry, (ucp)$1, (ucp)$2); }
	|	parts

atentry: 	ent_cgp		     { bld_entry_cgp(curr_entry); }
        |	'+' ent_cgp 	     { bld_entry_cgp(curr_entry); 
    				       bld_edit_entry(curr_entry, '+'); } ;
	|	'-' ent_cgp  why     {
	    			      bld_entry_cgp(curr_entry);
    				      bld_edit_entry(curr_entry, '-');
				      bld_edit_why(curr_entry, yylval.text); } ;

ent_cgp: 	begin_entry cgp

begin_entry:  	entry_wrapper { curr_entry = bld_entry(@1,curr_cbd); 
	                        curr_meta = curr_entry->meta = bld_meta_create(curr_entry); } ;

entry_wrapper:	 ENTRY
	|	 error ENTRY  { yyerrok; }
	;

why:		CMTWHY TEXTSPEC

modentry: 	RENAME cgp { bld_edit_entry(curr_entry, '>'); } ;
	| 	MERGE  cgp { bld_edit_entry(curr_entry, '|'); } ;

alias:  	atalias cgp 	{ bld_alias(@1,curr_entry); }

atalias:	ALIAS

parts:  	atparts cgplist { curr_parts->cgps = cgp_get_all(); }

atparts: 	PARTS { curr_parts = bld_parts(@1,curr_entry); }

end_entry:	END_ENTRY { curr_entry->end_entry = bld_locator(@1); curr_entry = NULL; }
		
lang_block: allow | bases | form

allow: 	     ALLOW BASE_PRI '=' BASE_PRI { bld_allow(@1,curr_entry,(ucp)$2,(ucp)$4); } 

bases:	     atbases baselist

atbases:     BASES
		
baselist:    base
	     | baselist base

base: 	     base_pri
	     | base_pri base_alt

base_pri:	BASE_PRI          { bld_bases_pri(@1, curr_entry, NULL, (ucp)$1); }
	|	LANGSPEC BASE_PRI { bld_bases_pri(@1, curr_entry, (ucp)$1, (ucp)$2); }

base_alt: 	BASE_ALT          { bld_bases_alt(@1, curr_entry, (ucp)$1); }
	     |	base_alt BASE_ALT { bld_bases_alt(@1, curr_entry, (ucp)$2); }

form:		atform formlang form_args end_form

atform:		FORM		{ curr_form = bld_form(@1, curr_entry); }
end_form:	END_FORM	{ bld_form_setup(curr_entry, curr_form); curr_form = NULL; }

formlang:	fform
	|	fform flang
	|	fform flang frws
	|	fform frws
	;

fform:	     	FFORM 		{ curr_form->form = (ucp)$1; }
flang: 		FLANG 		{ curr_form->lang = (ucp)$1; }
frws: 		FRWS 		{ curr_form->rws  = (ucp)$1; }

form_args:     	form_any
	|	form_args form_any

form_any:	fbase
	|	fstem
	| 	fcont
	| 	form_morph
	|      	fnorm

fbase: 		FBASE 		{ curr_form->base = (ucp)$1; }
fstem: 		FSTEM 		{ curr_form->stem = (ucp)$1; }
fcont: 		FCONT 		{ curr_form->cont = (ucp)$1; }

form_morph: fmorph
	|	fmorph fmorph2

fmorph:        	FMORPH  	{ curr_form->morph = (ucp)$1; }
fmorph2: 	FMORPH2 	{ curr_form->morph2 = (ucp)$1; }
fnorm: 		FNORM 		{ curr_form->norm = (ucp)$1; }

senses_block: sense

sense:	      senseline
	      | senseline modsense

senseline:     	atsense pos mng
	|	atsense sgw pos mng
	;

atsense:      	ssense
        |	'+' ssense	{ bld_edit_sense(curr_entry, '+'); }
	|	'-' ssense	{ bld_edit_sense(curr_entry, '-'); }

ssense:		SENSE 		{ curr_sense = bld_sense(@1, curr_entry); 
		    		  if (curr_entry->begin_senses) { curr_meta = curr_sense->meta = bld_meta_create(curr_entry); } }
sgw:		'[' GW ']'	{ curr_sense->sgw = (ucp)$2; }
pos:		POS /* should be restricted to legal POS */	  { curr_sense->pos = (ucp)$1; }
mng:		TEXTSPEC /* should restrict to disallow [ and ] */ { curr_sense->mng = (ucp)$1; }
		
modsense: 	RENAME POS TEXTSPEC { curr_sense = bld_edit_sense(curr_entry, '>');
    				      curr_sense->pos = (ucp)$2;
			              curr_sense->mng = (ucp)$3; }
           | 	MERGE  POS TEXTSPEC { curr_sense = bld_edit_sense(curr_entry, '|'); 
    				      curr_sense->pos = (ucp)$2;
			              curr_sense->mng = (ucp)$3; }

meta:	anymeta

anymeta: 	pleiades
	| 	prop
	| 	oid
	| 	collo
	| 	equiv
	| 	bib
        | 	inote
        | 	isslp
        | 	note

equiv: 		EQUIV LANG TEXTSPEC		{ bld_meta_add(@1,curr_entry, curr_meta, $1, "equiv",
		    					   bld_equiv(curr_entry,(ucp)$2,(ucp)$3)); }
isslp:		ISSLP TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "isslp", (ucp)$2); }
bib:		BIB TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "bib", (ucp)$2); }

inote:		INOTE TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "inote", (ucp)$2); }

note:		NOTE TEXTSPEC			{ bld_note(@1, curr_entry, curr_meta, (ucp)$2); }

pleiades:	pl_id pl_coord
	|	pl_id pl_coord pl_aliases
pl_id:		PL_ID TEXTSPEC			{ curr_pleiades = bld_pl_id(@1,curr_entry,(ucp)$2); }
pl_coord:	PL_COORD TEXTSPEC		{ bld_pl_coord(@1,curr_pleiades,(ucp)$2); }
pl_aliases: 	pl_alias
	|	pl_aliases pl_alias
pl_alias:	PL_ALIAS TEXTSPEC		{ bld_pl_alias(@1,curr_pleiades,NULL,(ucp)$2); }
	|	PL_ALIAS LANGSPEC TEXTSPEC     	{ bld_pl_alias(@1,curr_pleiades,(ccp)$2,(ucp)$3); } /* not in scanner.l yet */

prop:		PROP TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "prop", (ucp)$2); }
oid:		OID OIDSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "oid", (ucp)$2); }
collo:		COLLO TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "collo", (ucp)$2); }

%%

void
yyerror(const char *s)
{
  YYLTYPE loc;
  extern int yylineno;
  loc.file = (char *)file;
  loc.first_line = yylineno;
  if (!strncmp(s, "syntax error, ", strlen("syntax error, ")))
    s += strlen("syntax error, ");
  msglist_verr(&loc, "%s\n", s);
  /*fprintf(stderr, "%s\n", s);*/
}

void
lyyerror(YYLTYPE loc, char *s)
{
  msglist_err(&loc, s);
  ++parser_status;
}

void
vyyerror(YYLTYPE loc, char *s, ...)
{
  if (s)
    {
      va_list ap;
      va_start(ap, s);
      msglist_averr(&loc, s, ap);
      va_end(ap);
      ++parser_status;
    }
}

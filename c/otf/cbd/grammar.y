%locations
%{
#include <stdio.h>
#include "gx.h"
static struct f2 *curr_form;
static struct meta *curr_meta;
static struct sense *curr_sense;
int parser_status = 0;
int yydebug = 0;
extern int yylex(void);
#define dup(s) npool_copy((unsigned char *)(s),curr_cbd->pool)
%}
%union { char *text; int i; }

%token  <text> 		CF
%token  <text> 		GW
%token  <text> 		LANGSPEC
%token  <text> 		POS
%token  <text> 		TEXTSPEC
%token  <text> 		WORDSPEC
%token  <text> 		PROJSPEC
%token	<text>		MNGSPEC
%token	<text>		OIDSPEC
%token	<text>		DCF
%token	<text>		SENSE
%token	<text>		LANG
%token	<text>		SGWSPEC
%token	<text>		SIDSPEC
%token	<text>		SOLSPEC
%token	<text>		WHYSPEC
%token	<text>		BASE_PRI
%token	<text>		BASE_ALT
%token  <text> 		FFORM
%token  <text> 		FLANG
%token  <text> 		FBASE
%token  <text> 		FSTEM
%token  <text> 		FCONT
%token  <text> 		FMORPH
%token  <text> 		FMORPH2
%token  <text> 		FNORM
%token	<text>		PL_COORD
%token	<text> 		PL_ID
%token	<text>		PL_UID
%token	<i>		BIB
%token	<i>		OID
%token	<i>		COLLO
%token	<i>		PROP
%token	<i>		NOTE
%token	<i>		INOTE
%token	<i>		ISSLP
%token	<i>		EQUIV
%token	<i>		PLEIADES

%token ENTRY END_ENTRY SENSES END_SENSES PROJECT NAME ALIAS BASES FORM END_FORM
       PROPLIST MERGE PARTS RENAME WHY ALLOW PHON ROOT STEM EDISC SDISC EOL ENDOF

%start cbd

%%

cbd: header entrylist ENDOF { return(0); }
	 |	header ENDOF  { return(1); }

header: 	atproject atlang atname
	| 	atproject atlang atname proplist

atproject: PROJECT PROJSPEC { curr_cbd->project = (ucp)yylval.text; } ;

atlang:    LANG    LANGSPEC { curr_cbd->lang = (ucp)yylval.text; } ;

atname:    NAME    TEXTSPEC { curr_cbd->name = (ucp)yylval.text; } ;

proplist: 	atproplist
	|	proplist atproplist

atproplist:	PROPLIST TEXTSPEC { proplist_add(curr_cbd, yylval.text); }

cgplist: cgp
	 | cgplist cgp

cgp:    CF '[' GW ']' POS { cgp_save((ucp)$1, (ucp)$3, (ucp)$5); } ;
	|	CF '[' GW ']' EOL { lyyerror(@$, "expected POS but found end of line"); }

entrylist:	entry
	|	entrylist entry

entry: 		entry_block end_entry
	|	entry_block lang_block end_entry 
	|	entry_block lang_block senses_block end_entry 
	|	entry_block lang_block senses_block meta end_entry 
	|	entry_block senses_block end_entry
	|	entry_block senses_block meta end_entry
	|	entry_block meta end_entry
	|	entry_block entry_block end_entry { lyyerror(@2,"duplicate @entry or missing @end entry"); }
	|	entry_block senses_block lang_block end_entry  { lyyerror(@3, "lang block fields must come before senses block"); }
	|	entry_block ENDOF 	{ lyyerror(@2,"input ended without @end entry"); return(1); }

entry_block:    atentry
	|	atentry aliases
	| 	atentry aliases parts
	| 	atentry aliases parts disc
	|	atentry modentry
	|	atentry modentry aliases
	| 	atentry modentry aliases parts
	| 	atentry modentry aliases parts disc
	| 	atentry parts
	| 	atentry parts disc
	| 	atentry disc

disc: 		EDISC TEXTSPEC /* | FILESPEC | URLSPEC */ { curr_entry->disc = tag_init(@1, curr_entry, "disc", (ucp)$2); }
	    |	SDISC TEXTSPEC { curr_sense->disc = tag_init(@1, curr_entry, "disc",(ucp)$2); }

atentry: 	begin_entry cgp     { curr_entry->cgp = cgp_get_one(); } ;
        |	'+' begin_entry cgp { curr_entry->cgp = cgp_get_one();
    				      entry_edit(curr_entry, '+'); } ;
	|	'-' begin_entry cgp why {
	    			      curr_entry->cgp = cgp_get_one();
    				      entry_edit(curr_entry, '-');
				      edit_why(curr_entry, yylval.text); } ;

begin_entry:  	ENTRY { curr_entry = entry_init(@1,curr_cbd); 
		        curr_meta = curr_entry->meta = meta_init(curr_entry); } ;

why:		WHY WHYSPEC

modentry: 	RENAME cgp { entry_edit(curr_entry, '>'); } ;
	| 	MERGE  cgp { entry_edit(curr_entry, '|'); } ;

aliases: 	alias
	| 	aliases alias

alias:  	atalias cgp 	{ alias_init(@1,curr_entry); }
	|	DCF TEXTSPEC 	{ dcf_init(@1,curr_entry, (ucp)$1, (ucp)$2); }

atalias:	ALIAS ;

parts:  	atparts cgplist { curr_parts->cgps = cgp_get_all(); }

atparts: 	PARTS { curr_parts = parts_init(@1,curr_entry); }

end_entry:	END_ENTRY { curr_entry = NULL; }
		
lang_block: bases_block
	    | bases_block forms
	    | forms

bases_block: allows
	     |	 allows bases_fields
	     |	 bases_fields

bases_fields: 	phon
	|	phon root
	|	phon root stems
	|	phon root stems bases
	|	root
	|	root stems
	|	root stems bases
	|	stems
	|	stems bases
	|	bases

allows:	     allow
	     | allows allow

allow: 	     ALLOW BASE_PRI '=' BASE_PRI { allow_init(@1,curr_entry,(ucp)$2,(ucp)$4); } 

bases:	     atbases baselist

atbases:     BASES
		
baselist:    base
	     | baselist base

base: 	     base_pri
	     | base_pri base_alt

base_pri:	BASE_PRI          { bases_pri_save(@1, curr_entry, (ucp)$1); }

base_alt: 	BASE_ALT          { bases_alt_save(@1, curr_entry, (ucp)$1); }
	     |	base_alt BASE_ALT { bases_alt_save(@1, curr_entry, (ucp)$2); }

phon:		PHON TEXTSPEC { curr_entry->phon = tag_init(@1, curr_entry, "phon", (ucp)$2); }

root:		ROOT TEXTSPEC { curr_entry->root = tag_init(@1, curr_entry, "root", (ucp)$2); }

stems:	atstem stemlist

stemlist: stem
	|	stemlist stem

stem:		WORDSPEC { stem_init(@1, curr_entry, (ucp)$1); }

atstem: 	STEM
								
forms:		form
	|	forms form

form:		atform formlang form_args end_form

atform:		FORM		{ curr_form = form_init(@1, curr_entry); }
end_form:	END_FORM	{ curr_form = NULL; }
formlang:	fform
	|	fform flang
	;

fform:	     	FFORM 		{ curr_form->form = (ucp)$1; }
flang: 		FLANG 		{ curr_form->lang = (ucp)$1; }

form_args:	fbase form_norm
	|      	fbase form_morph form_norm
	|  	fbase fcont form_morph form_norm
	|  	fbase fcont form_norm
	|  	fbase fstem fcont form_morph form_norm
	|  	fbase fstem fcont form_norm
	|  	fbase fstem form_morph form_norm
	|  	fbase fstem form_norm
	|	fstem form_norm
	|	fstem form_morph form_norm
	|	fstem fcont form_norm
	|	fstem fcont form_morph form_norm
	|	fcont form_norm
	|	fcont form_morph form_norm
	|	form_morph form_norm
	|	form_norm

fbase: 		FBASE 		{ curr_form->base = (ucp)$1; }
fstem: 		FSTEM 		{ curr_form->stem = (ucp)$1; }
fcont: 		FCONT 		{ curr_form->cont = (ucp)$1; }

form_morph: fmorph
	|	fmorph fmorph2

fmorph:        	FMORPH  	{ curr_form->morph = (ucp)$1; }
fmorph2: 	FMORPH2 	{ curr_form->morph2 = (ucp)$1; }	       

form_norm: fnorm
	|      /* empty */

fnorm: 		FNORM 		{ curr_form->norm = (ucp)$1; }

senses_block: senses
	      | begin_senses sensesmeta end_senses

begin_senses: SENSES		{ curr_entry->beginsenses = 1; curr_meta = NULL; }
end_senses:   END_SENSES	{ curr_meta = curr_entry->meta; }
		
senses:	      sense
	      | senses sense
	      ;

sense:	      senseinfo
	      | senseinfo disc
	      | senseinfo modsense
	      | senseinfo modsense disc

sensesmeta:   sensemeta
	      | sensesmeta sensemeta

sensemeta:    senseinfo
	      | senseinfo disc
	      | senseinfo meta
	      | senseinfo modsense
	      | senseinfo modsense disc
	      | senseinfo modsense disc meta
	      | senseinfo disc meta

senseinfo:	atsense pos mng
	|	atsense sid pos mng
	|	atsense sid sol pos mng
	|	atsense sid sol sgw pos mng
	|	atsense sid sgw pos mng
	|	atsense sol pos mng
	|	atsense sol sgw pos mng
	|	atsense sgw pos mng
	;

atsense:      	senselang
        |	'+' senselang	{ sense_edit(curr_entry, '+'); }
	|	'-' senselang	{ sense_edit(curr_entry, '-'); }

senselang:	ssense
	|	ssense slang

ssense:		SENSE 		{ curr_sense = sense_init(@1, curr_entry); 
		    		  if (curr_entry->beginsenses) { curr_meta = curr_sense->meta = meta_init(curr_entry); } }
slang:		'%' WORDSPEC   	{ curr_sense->lng = (ucp)$2; }

sid:		'#' WORDSPEC	{ curr_sense->sid = (ucp)$2; }
sol:		'.' WORDSPEC	{ curr_sense->num = (ucp)$2; }
sgw:		'[' GW ']'	{ curr_sense->sgw = (ucp)$2; }
pos:		POS /* should be restricted to legal POS */	  { curr_sense->pos = (ucp)$1; }
mng:		TEXTSPEC /* shouldrestrict to disallow [ and ] */ { curr_sense->mng = (ucp)$1; }
		
modsense: 	RENAME POS TEXTSPEC { curr_sense = sense_edit(curr_entry, '>');
    				      curr_sense->pos = (ucp)$2;
			              curr_sense->mng = (ucp)$3; }
           | 	MERGE  POS TEXTSPEC { curr_sense = sense_edit(curr_entry, '|'); 
    				      curr_sense->pos = (ucp)$2;
			              curr_sense->mng = (ucp)$3; }

meta:	anymeta
	|	meta anymeta

anymeta: 	pleiades
	| 	prop
	| 	oid
	| 	collo
	| 	equiv
	| 	bib
        | 	inote
        | 	isslp
        | 	note
		/*  REL GOES HERE */

equiv: 		EQUIV LANG TEXTSPEC		{ meta_add(@1,curr_entry, curr_meta, $1, "equiv",
		    					   equiv_init(curr_entry,(ucp)$2,(ucp)$3)); }
isslp:		ISSLP TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "isslp", (ucp)$2); }
bib:		BIB TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "bib", (ucp)$2); }
note:		NOTE TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "note", (ucp)$2); }
inote:		INOTE TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "inote", (ucp)$2); }
pleiades: 	PL_COORD TEXTSPEC PL_ID TEXTSPEC PL_UID TEXTSPEC
						{ meta_add(@1,curr_entry, curr_meta, PLEIADES, "pleiades",
		    				  	   pleiades_init(curr_entry,(ucp)$2,(ucp)$4,(ucp)$6)); }
prop:		PROP TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "prop", (ucp)$2); }
oid:		OID OIDSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "oid", (ucp)$2); }
collo:		COLLO TEXTSPEC			{ meta_add(@1,curr_entry, curr_meta, $1, "collo", (ucp)$2); }

%%

void
yyerror(char *s)
{
  fprintf(stderr, "%s\n", s);
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

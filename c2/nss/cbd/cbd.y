%define api.prefix {cbd}
%file-prefix "cbd"
%locations
%define parse.error verbose
%{
#include <stdio.h>
#include <stdarg.h>
#include "form.h"
#include "cbd.h"
#include "mesg.h"

#define CBDLTYPE_IS_DECLARED 1
#define CBDLTYPE Mloc

#define yylineno cbdlineno

static Form *curr_form;
static struct meta *curr_meta;
static struct sense *curr_sense;
static struct pleiades *curr_pleiades;

extern int parser_status;
extern int yydebug;

extern int bang, star;
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
%token  <text> 		WORDSPEC
%token  <text> 		PROJSPEC
%token	<text>		OIDSPEC
%token  <text> 		FILESPEC
%token  <text> 		URLSPEC
%token	<text>		DCF
%token	<text>		SENSE
%token	<text>		SENSEL
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
%token	<text> 		PL_UID
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

%token ENTRY END_ENTRY SENSES END_SENSES PROJECT NAME PROPS RELDEF
       ALIAS BASES FORM END_FORM MERGE PARTS RENAME WHY ALLOW PHON ROOT
       STEM STEMS EDISC SDISC EDISCL SDISCL EOL CBD TRANSLANG GWL I18N
       NOTEL BFF

%start cbd

%%

cbd: 		header entrylist { return(parser_status); }
	|      	header  	 { return(parser_status); }
	|	/* empty */ 	 { return(parser_status); }

header:		reqheader
	|	reqheader optheader

reqheader: 	atproject atlang atname

optheader:	optheader_one
	|	optheader_multi
	|	optheader_one optheader_multi
	|	optheader_multi optheader_one
			

optheader_one: 	atcbd
	|	attranslang
	|	ati18n
	|	atcbd attranslang
	|	atcbd ati18n
	|	attranslang atcbd
	|	attranslang ati18n
	|	ati18n atcbd
	|	ati18n attranslang
	|	atcbd attranslang ati18n
	|	atcbd ati18n attranslang
	|	attranslang atcbd ati18n
	|	attranslang ati18n atcbd
	|	ati18n atcbd attranslang
	|	ati18n attranslang atcbd
		
optheader_multi:
		atprops
	|	atreldef
	|	optheader_multi atprops
	|	optheader_multi atreldef
	;

atproject: PROJECT PROJSPEC { curr_cbd->project = (ucp)yylval.text; }

atlang:    LANG    LANGSPEC { curr_cbd->lang = (ucp)yylval.text; }

atname:    NAME    TEXTSPEC { curr_cbd->name = (ucp)yylval.text; }

atcbd:	   CBD     WORDSPEC { curr_cbd->version = (ucp)yylval.text; }

atprops:	PROPS TEXTSPEC { bld_props(curr_cbd, yylval.text); }

atreldef:	RELDEF TEXTSPEC { bld_reldef(curr_cbd, yylval.text); }

attranslang:	TRANSLANG LANGSPEC { curr_cbd->trans = (ucp)yylval.text; }

ati18n:		I18N TEXTSPEC	  { curr_cbd->i18nstr = yylval.text; }
		
cgplist: cgp
	 | cgplist cgp

cgp:    CF '[' GW ']' POS { cgp_save((ucp)$1, (ucp)$3, (ucp)$5); } ;
	|	CF '[' GW ']' EOL { lyyerror(@1, "expected POS but found end of line"); }
	|	CF '[' GW EOL { lyyerror(@1, "missing ']' (and maybe POS) after GW"); }
	|	CF ']' POS { lyyerror(@1, "missing '[' before GW"); }

disc:	disc_df
	|	disc_df disc_trs

disc_df:	EDISC TEXTSPEC /* | FILESPEC | URLSPEC */ { curr_entry->disc = bld_tag(@1, curr_entry, "disc", (ucp)$2); }
	|     	SDISC TEXTSPEC { curr_sense->disc = bld_tag(@1, curr_entry, "disc",(ucp)$2); }

disc_trs:	disc_tr
	|	disc_trs disc_tr
		
disc_tr:	EDISCL LANGSPEC TEXTSPEC /* | FILESPEC | URLSPEC */ { bld_discl(@1, curr_entry, (ccp)$2, (ucp)$3, 0); }
        |     	SDISCL LANGSPEC TEXTSPEC 			    { bld_discl(@1, curr_entry, (ccp)$2, (ucp)$3, 1); }

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
	|	entry_block ENDOF 		  { lyyerror(@2,"input ended without @end entry"); return(1); }

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

atentry: 	ent_cgp		     { bld_entry_cgp(curr_entry); }
        |	'+' ent_cgp 	     { bld_entry_cgp(curr_entry); 
    				       bld_edit_entry(curr_entry, '+'); } ;
	|	'-' ent_cgp  why     {
	    			      bld_entry_cgp(curr_entry);
    				      bld_edit_entry(curr_entry, '-');
				      bld_edit_why(curr_entry, yylval.text); } ;

ent_cgp: 	begin_entry cgp
	|	begin_entry cgp gwls

gwls:		gwl
	|	gwls gwl
	;

gwl:		GWL LANGSPEC TEXTSPEC { bld_gwl(@1,curr_entry,(ccp)$2,(ucp)$3); }
	;

begin_entry:  	entry_wrapper { curr_entry = bld_entry(@1,curr_cbd); 
	                        curr_meta = curr_entry->meta = bld_meta_create(curr_entry); } ;

entry_wrapper:	 ENTRY
	|	 error ENTRY  { yyerrok; }
	;

why:		WHY TEXTSPEC

modentry: 	RENAME cgp { bld_edit_entry(curr_entry, '>'); } ;
	| 	MERGE  cgp { bld_edit_entry(curr_entry, '|'); } ;

aliases: 	alias
	| 	aliases alias

alias:  	atalias cgp 	{ bld_alias(@1,curr_entry); }
	|	DCF TEXTSPEC 	{ bld_dcf(@1,curr_entry, (ucp)$1, (ucp)$2); }

atalias:	ALIAS

/*  THIS DOESN'T ALLOW MULTIPLE @parts */
parts:  	atparts cgplist { curr_parts->cgps = cgp_get_all(); }

atparts: 	PARTS { curr_parts = bld_parts(@1,curr_entry); }

end_entry:	END_ENTRY { curr_entry->end_entry = bld_locator(@1); curr_entry = NULL; }
		
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

phon:		PHON TEXTSPEC { curr_entry->phon = bld_tag(@1, curr_entry, "phon", (ucp)$2); }

root:		ROOT TEXTSPEC { curr_entry->root = bld_tag(@1, curr_entry, "root", (ucp)$2); }

stems:	atstem stemlist

stemlist: stem
	|	stemlist stem

stem:		WORDSPEC { bld_stem(@1, curr_entry, (ucp)$1); }

atstem: 	STEM
								
forms:		form
	|	forms form

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

begin_senses: SENSES		{ curr_entry->begin_senses = bld_locator(@1); curr_meta = NULL; }
end_senses:   END_SENSES	{ curr_meta = curr_entry->meta; curr_entry->end_senses = bld_locator(@1); }
		
senses:	      sense
	      | senses sense
	      ;

sense:	      senseline
	      | senseline disc
	      | senseline modsense
	      | senseline modsense disc

sensesmeta:   sensemeta
	      | sensesmeta sensemeta

sensemeta:    senseline
	      | senseline disc
	      | senseline meta
	      | senseline modsense
	      | senseline modsense disc
	      | senseline modsense disc meta
	      | senseline disc meta

senseline:	senseline_en
	|	senseline_en senseline_trs
		
senseline_en:	atsense pos mng
	|	atsense sid pos mng
	|	atsense sid sok pos mng
	|	atsense sid sok sgw pos mng
	|	atsense sid sgw pos mng
	|	atsense sok pos mng
	|	atsense sok sgw pos mng
	|	atsense sgw pos mng
	;

senseline_trs:	senseline_tr
	|     	senseline_trs senseline_tr
	;

senseline_tr:	atsensel slang pos mng
	|	atsensel slang sid pos mng
	|	atsensel slang sid sok pos mng
	|	atsensel slang sid sok sgw pos mng
	|	atsensel slang sid sgw pos mng
	|	atsensel slang sok pos mng
	|	atsensel slang sok sgw pos mng
	|	atsensel slang sgw pos mng
	;

atsense:      	ssense
        |	'+' ssense	{ bld_edit_sense(curr_entry, '+'); }
	|	'-' ssense	{ bld_edit_sense(curr_entry, '-'); }

atsensel:      	SENSEL 		{ curr_sense = bld_sensel(@1, curr_entry); }

ssense:		SENSE 		{ curr_sense = bld_sense(@1, curr_entry); 
		    		  if (curr_entry->begin_senses) { curr_meta = curr_sense->meta = bld_meta_create(curr_entry); } }
slang:		'%' WORDSPEC   	{ curr_sense->lng = (ucp)$2; }

sid:		'#' WORDSPEC	{ curr_sense->sid = (ucp)$2; }
sok:		'.' WORDSPEC	{ curr_sense->num = (ucp)$2; }
sgw:		'[' GW ']'	{ curr_sense->sgw = (ucp)$2; }
pos:		POS /* should be restricted to legal POS */	  { curr_sense->pos = (ucp)$1; }
mng:		TEXTSPEC /* shouldrestrict to disallow [ and ] */ { curr_sense->mng = (ucp)$1; }
		
modsense: 	RENAME POS TEXTSPEC { curr_sense = bld_edit_sense(curr_entry, '>');
    				      curr_sense->pos = (ucp)$2;
			              curr_sense->mng = (ucp)$3; }
           | 	MERGE  POS TEXTSPEC { curr_sense = bld_edit_sense(curr_entry, '|'); 
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
        | 	notes
		/*  REL GOES HERE */

equiv: 		EQUIV LANG TEXTSPEC		{ bld_meta_add(@1,curr_entry, curr_meta, $1, "equiv",
		    					   bld_equiv(curr_entry,(ucp)$2,(ucp)$3)); }
isslp:		ISSLP TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "isslp", (ucp)$2); }
bib:		BIB TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "bib", (ucp)$2); }

inote:		INOTE TEXTSPEC			{ bld_meta_add(@1,curr_entry, curr_meta, $1, "inote", (ucp)$2); }

notes:		note
	|	note notels

notels: 	notel
	|	notels notel

note:		NOTE TEXTSPEC			{ bld_note(@1, curr_entry, curr_meta, (ucp)$2); }
notel:		NOTEL LANGSPEC TEXTSPEC	       	{ bld_notel(@1, curr_entry, curr_meta, (ccp)$2, (ucp)$3); }

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
  extern char *file;
  extern int yylineno;
  loc.file = (char *)file;
  loc.line = yylineno;
  if (!strncmp(s, "syntax error, ", strlen("syntax error, ")))
    s += strlen("syntax error, ");
  mesg_verr(&loc, "%s\n", s);
  /*fprintf(stderr, "%s\n", s);*/
}

void
lyyerror(YYLTYPE loc, char *s)
{
  mesg_err(&loc, s);
  ++parser_status;
}

void
vyyerror(YYLTYPE loc, char *s, ...)
{
  if (s)
    {
      va_list ap;
      va_start(ap, s);
      mesg_averr(&loc, s, ap);
      va_end(ap);
      ++parser_status;
    }
}

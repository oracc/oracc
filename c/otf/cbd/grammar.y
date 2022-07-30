%{
#include <stdio.h>
#include "gx.h"
#define YYDEBUG 1
extern int yylex(void);
void yyerror(char *s);
#define dup(s) npool_copy((unsigned char *)(s),curr_cbd->pool)
%}
%union { char *text; int i; }

/*		       ADD DISC */
			
%token  <text> 		CF
%token  <text> 		GW
%token  <text> 		LANGSPEC
%token  <text> 		POS
%token  <text> 		TEXTSPEC
%token  <text> 		PROJSPEC
%token	<text>		MNGSPEC
%token	<text>		OIDSPEC
%token	<text>		SGWSPEC
%token	<text>		SIDSPEC
%token	<text>		SOLSPEC
%token	<text>		WHYSPEC
%token	<text>		BASE_PRI
%token	<text>		BASE_ALT

%token ENTRY END_ENTRY END_SENSES LANG PROJECT NAME ALIAS BASES FORM
       PROPLIST MERGE PARTS RENAME SENSE WHY ALLOW
       PHON ROOT STEM EQUIV ATBIB ATINOTE ATNOTE ATISSLP
       ATOID ATCOLLO ATPROP PL_COORD PL_ID PL_UID

%start cbd

%%

cbd: header entrylist

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

entrylist:	entry
	|	entrylist entry

entry: 		entry_block end_entry
	|	entry_block lang_block end_entry 
	|	entry_block lang_block senses_block end_entry 
	|	entry_block lang_block senses_block meta_block end_entry 
	|	entry_block senses_block end_entry
	|	entry_block senses_block meta_block end_entry
	|	entry_block meta_block end_entry


entry_block: 	atentry
	|	atentry parts
	|	atentry aliases
	| 	atentry aliases parts
	| 	atentry modentry
	| 	atentry modentry aliases
	| 	atentry modentry aliases parts

atentry: 	begin_entry cgp     { curr_entry->cgp = cgp_get_one(); } ;
        |	'+' begin_entry cgp { curr_entry->cgp = cgp_get_one();
    				      entry_edit(curr_entry, '+'); } ;
	|	'-' begin_entry cgp why {
	    			      curr_entry->cgp = cgp_get_one();
    				      entry_edit(curr_entry, '-');
				      edit_why(curr_entry, yylval.text); } ;

begin_entry:  	ENTRY { curr_entry = entry_init(curr_cbd); } ;

why:		WHY WHYSPEC

modentry: 	RENAME cgp { entry_edit(curr_entry, '>'); } ;
	| 	MERGE  cgp { entry_edit(curr_entry, '|'); } ;

aliases: 	alias
	| 	aliases alias

alias:  	atalias cgp { alias_init(curr_entry); } ;

atalias:	ALIAS ;

parts:  	atparts cgplist { curr_parts->cgps = cgp_get_all(); }

atparts: 	PARTS { curr_parts = parts_init(curr_entry); }

end_entry:	END_ENTRY { curr_entry = NULL; } ;
		
lang_block: bases_block
	    | bases_block forms
	    | forms

bases_block: bases_fields
	     | allows bases_fields

bases_fields: bases
              | bases phon
	      | bases phon stems
	      | bases phon stems root
	      | bases stems
	      | bases stems root
	      |	bases root

allows:	     allow
	     | allows allow

allow: 	     atallow base_pri '=' base_pri

atallow:     ALLOW
		
bases:	     atbases baselist

atbases:     BASES
		
baselist:    base
	     | baselist base

base: 	     base_pri
	     | base_pri base_alt

base_pri:	BASE_PRI

base_alt: 	BASE_ALT
	     |	base_alt BASE_ALT

phon:		atphon TEXTSPEC

atphon:		PHON

root:		atroot TEXTSPEC

atroot:		ROOT

stems:	atstem stemlist

stemlist: stem
	|	stemlist stem

stem:		TEXTSPEC

atstem: 	STEM
								
forms:		form
	|	forms form

form:		atform

atform: 	FORM
		
senses_block: senses end_senses

end_senses:   END_SENSES
		
senses:	      sense
	      | senses sense

sense:	      senseinfo
	      | senseinfo meta_block
	      | senseinfo modsense
	      | senseinfo modsense meta_block

senseinfo:	atsense pos mng
	|	atsense sid pos mng
	|	atsense sid sol pos mng
	|	atsense sid sol sgw pos mng
	|	atsense sid sgw pos mng
	|	atsense sol pos mng
	|	atsense sol sgw pos mng
	|	atsense sgw pos mng
	;

atsense:      	SENSE
	|	'+' SENSE
	|	'-' SENSE

sid:		SIDSPEC
sol:		SOLSPEC
sgw:		SGWSPEC
pos:		TEXTSPEC /* should be restricted to legal POS */
mng:		MNGSPEC /* should be restricted to disallow [ and ] */
		
modsense: 	RENAME cgp { sense_edit(curr_entry, '>'); } ;
	| 	MERGE  cgp { sense_edit(curr_entry, '|'); } ;

meta_block: pleiades_block
	    | props
	    | oid
	    | collos
	    | equivs
	    | notes
	    | pleiades_block props
	    | pleiades_block props oid
	    | pleiades_block props oid collos
	    | pleiades_block oid
	    | pleiades_block oid collos
	    | props oid
	    | props oid collos
	    ;

notes: bib
       | inote
       | isslp
       | note
       | notes bib
       | notes inote
       | notes isslp
       | notes note
       ;

equivs:		equiv
	|	equivs equiv

equiv: 		atequiv lang text
atequiv:	EQUIV
lang:		LANGSPEC
text:		TEXTSPEC

bib:	ATBIB TEXTSPEC
isslp:	ATISSLP TEXTSPEC
note:	ATNOTE TEXTSPEC
inote:	ATINOTE TEXTSPEC

pleiades_block: PL_COORD PL_ID PL_UID

props: 		prop
	|	props prop

prop:		ATPROP TEXTSPEC

oid:		ATOID OIDSPEC

collos:		collo
	|	collos collo

collo:		ATCOLLO TEXTSPEC
	;
%%

void
yyerror(char *s)
{
  printf("error: %s\n", s);
}

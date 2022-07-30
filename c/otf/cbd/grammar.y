%{
#include <stdio.h>
#include "gx.h"
#define YYDEBUG 1
extern int yylex(void);
void yyerror(char *s);
#define dup(s) npool_copy((unsigned char *)(s),curr_cbd->pool)
%}
%union { char *text; int i; }

%token  <text> 		CF
%token  <text> 		GW
%token  <text> 		LANGSPEC
%token  <text> 		POS
%token  <text> 		TEXTSPEC
%token  <text> 		PROJSPEC
%token	<text>		WHYSPEC

%token ENTRY END_ENTRY LANG PROJECT NAME ALIAS BASES FORM PROPLIST MERGE PARTS RENAME SENSE WHY

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


entry: 		entry_block
		/* lang_block senses_block meta_block */
		end_entry


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
		
		/*

lang_block: bases_block forms
	    | bases_block stems forms
	    | bases_block forms equiv
	    | bases_block stems forms equivs
	    | forms
	    | forms equivs

bases_block: bases_fields
	     | allows bases_fields

bases_fields: bases
	      | bases phon
	      | bases phon stems
	      | bases phon stems root
	      | bases stems
	      | bases stems root

allows:	     allow
	     | allows bases_fields

allow: 	     atallow base_pri '=' base_pri

bases:	     atbases baselist

baselist:    base
	     | baselist base

base: 	     base_pri
	     | base_pri base_alt

senses_block: senses senses_e

senses:	      sense
	      | senses sense

sense:	      atsense
	      | atsense notes
	      | atsense modsense
	      | atsense modsense equivs
	      | atsense equivs notes

senses_e: SENSES_END_BLOCK

notes: bib
       | inote
       | isslp
       | xnote
       | notes bib
       | notes inote
       | notes isslp
       | notes xnote
       ;

meta_block: pleiades_block
	    | props
	    | oid
	    | collos
	    | pleiades_block props
	    | pleiades_block props oid
	    | pleiades_block props oid collos
	    | pleiades_block oid
	    | pleiades_block oid collos
	    | props oid
	    | props oid collos
	    ;

	*/

%%

void
yyerror(char *s)
{
  printf("error: %s\n", s);
}

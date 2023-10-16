word:     grapheme
	| word delim graphmeta
	;

graphemes:
	  corqorsg
	| graphemes delim corqorsg
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

corqorsg:  maybegopener corqors maybegflags maybegcloser

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

maybegopener:
	  stateo
	| /* empty */
        ;

maybegcloser:
	  statec
	| /* empty */
        ;

stateo:  
	  '<'						{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| L_dbl_ang				       	{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| L_dbl_cur			       		{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| '['						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| L_uhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| L_lhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	;

statec:
	  '>'						{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| R_dbl_ang			       		{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| R_dbl_cur					{ gdl_prop(ynp, $1, PG_GDL_STATE, NULL, NULL); }
	| ']'						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| R_uhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	| R_lhs						{ gdl_prop(ynp, $1, PG_GDL_BREAK, NULL, NULL); }
	;


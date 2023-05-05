
%x s

characters	({character}|{escape})*
character 	[\x20\x21\x23-\x5b\x5d-\xff]
escape		"\\"([\\/bfnrt]\|[0-9a-fA-F]{4})

number		({integer}{fraction}?{exponent}?)
integer		(-?[0-9|[1-9][0-9]+)
fraction	("."[0-9]+)
exponent	([eE][-+]?[0-9]+)

ws		[ \t\r\n]+

"\""		{ BEGIN(s); return '"'; }
<s>{characters} { yysave(yytext); return CHARACTERS; }
<s>'"'		{ BEGIN(0); return '"'; }

"["|"]"|"{"|"}" { return *yytext; }

{ws}		{ }

"false"		{ return FALSE; }
"null"		{ return NULL; }
"true"		{ return TRUE; }

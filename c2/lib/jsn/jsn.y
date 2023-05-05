
%tokens NUMBER TRUE FALSE NULL CHARACTERS WS

%start json

%%

json:
	element

value:
	object
	| array
	| string
	| NUMBER
	| TRUE
	| FALSE
	| NULL
	;

object:
	'{' ws '}'
	| '{' members '}'
	;

members:
	member
	| member ',' members
	;

member:
	ws string ws ':' element
	;

array:
	'[' ws ']'
	| '[' elements ']'
	;

elements:
	element
	| element ',' elements
	;

element:
	ws value ws
	;

string:
	'"' CHARACTERS '"'
	;

ws:	WS
	| /* empty */
	;

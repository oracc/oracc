%{
#include "gx.h"
#define YY_NO_UNISTD_H
%}
%x B
%%
^@entry		printf("entry\n");
^@bases[ \t]+ 	{ BEGIN B; printf("B_BEG"); }
<B>\;[ \t]+    	printf("B_SEP");
<B>,[ \t]+    	printf("B_ALT_SEP");
<B>[ \t]+\(	printf("B_ALT_BEG");
<B>\);		printf("B_ALT_END"); unput(';');
<B>\)\n		printf("B_ALT_END"); unput('\n');
<B>\n		{ printf("B_END\n"); BEGIN 0; }
.|\n

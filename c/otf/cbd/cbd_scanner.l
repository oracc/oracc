%{
#include "gx.h"
#define YY_NO_UNISTD_H
int b_alt = 0;
int b_alt_end = 0;
char *b_alt_tok = NULL;
static void clean_alt_paren(void);
%}
%x B
%x BT
%x BA
%%
^\+?@entry		printf("entry\n");
^@bases[ \t]+ 	{ BEGIN B; printf("B_BEG"); BEGIN BT; }
<B>\;[ \t]+    	printf("B_SEP"); BEGIN BT;
<B>\n		{ printf("B_END\n"); BEGIN 0; }
<BT>[^; \t\n]+ 	printf("B_TOK[%s]", yytext);
<BT>[ \t]+\(	printf("B_ALT_BEG"); BEGIN BA;
<BA>,[ \t]+	printf("B_ALT<%s>",b_alt_tok); free(b_alt_tok); printf("B_ALTSEP");
<BA>[^;, \t\n]+ b_alt_tok = strdup(yytext);
<BA>\;[ \t]+    clean_alt_paren(); printf("B_ALT<%s>",b_alt_tok); free(b_alt_tok); printf("B_ALT_END"); printf("B_SEP"); BEGIN BT;
<BT>\;		BEGIN B; unput(';');
<BT>\n		{ printf("B_END\n"); BEGIN 0; }
<BA>\n		{ clean_alt_paren(); printf("B_ALT<%s>",b_alt_tok); free(b_alt_tok); printf("B_ALT_END"); printf("B_END\n"); BEGIN 0; }
.|\n
%%
static void
clean_alt_paren(void)
{
  b_alt_tok[strlen(b_alt_tok)-1] = '\0';
}

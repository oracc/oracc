%option prefix="cbd"
%option outfile="cbd-l.c"
%option never-interactive noyywrap yylineno nounput
%{

#define CBDLTYPE_IS_DECLARED 1

#include <ctype128.h>
#include <xsystem.h>
#include <iome.h>
#include "cbd.h"
#include "cbd.tab.h"
#include "pool.h"
#define YY_NO_UNISTD_H

#define yylloc cbdlloc
#define yylval cbdlval

/* keep-space-dup */
#define ksdup(s) yylval.text = (char*)pool_copy((unsigned char *)(s),curr_cbd->pool)

/* yytext-dup strips beginning/end spaces */
#define ytdup() (yylval.text = (char*)pool_copy((ucp)unspace(yytext),curr_cbd->pool))
#define ytdupn(n) yylval.text = (char*)pool_copy((ucp)unspace(yytext+(n)),curr_cbd->pool)
#define yi(tok) yylval.i = tok;
#define yln(s) if (ylntrace) {fprintf(stderr, "%s: %d\n",(s),yylineno);}

static int after_s_entsns = 0;
static int after_s_gw = 0;
static int after_s_pos = 0;
static int after_s_renmrg = 0;
extern char *efile;

static int base_state = BASE_PRI;
static int base_tok(void);

int bang, star;
extern List *cmt_queue;

static struct cbdtag *ctp;

#if 0
static int after_s_lang = 0;
static int yycolumn = 0;
static void baseco(const char *type);
static void basesc(const char *type);
static char *b_alt_tok = NULL;
static void alt_wrapup(int clean);
static void clean_alt_paren(void);
#endif

static int set_rws(void);
static int set_ctp(void);

static int tg_set_flags(char *f);

static char *unspace(char*s);
static void cbd_user_action(void);
#define mtch(x) fprintf(stderr,"matched %s",(x))

#define YY_USER_ACTION cbd_user_action();

int ylntrace = 0;
int mystate = 0;
#define ss(arg) (mystate=arg)

#define MAX_INCLUDE_DEPTH 10
struct bufstack
{
  char *file;
  int line;
  FILE *fp;
  YY_BUFFER_STATE yycb;
};
struct bufstack include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
/*static void includec(void);*/
static void includeo(char *file);

YY_BUFFER_STATE bp;

#if 1
#define xreturn(x) return(x)
#else
#define xreturn(x) return(tg_validate(x))
#endif

char *ssnames[] = {
"0",
"s_b",
"s_cf",
"s_f",
"s_flag",
"s_flush",
"s_gw",
"s_incl",
"s_pos",
"s_sol",
"s_s",
"s_lang",
"s_text",
"s_word"
"s_allow"
};

%}

%x s_b
%x s_cf
%x s_f
%x s_flag
%x s_flush
%x s_gw
%x s_incl
%x s_pos
%x s_sol
%x s_s
%x s_text
%x s_word
%x s_allow
%%

	/* OUTER LEVEL TOKENIZER */

^[-+]		{ BEGIN ss(s_sol); 	 return(yytext[0]); }
^>>[ \t]*      	{ after_s_gw=s_pos; after_s_pos=0; BEGIN ss(s_cf); return(RENAME); }
^>[ \t]*       	{ after_s_gw=s_pos; after_s_pos=0; BEGIN ss(s_cf); return(MERGE); }
^@end[ \t]+entry[ \t]*\n  { BEGIN ss(0); return(END_ENTRY); }
^@end[ \t]+senses[ \t]*\n { BEGIN ss(0); return(END_SENSES); }
^@[a-z]+	{ if (set_ctp()) xreturn(ctp->tok); }
^@[a-z]+/[!*]+	{ if (set_ctp()) BEGIN ss(s_flag); }
<s_flag>[!*]+	{ tg_set_flags(yytext) ; xreturn(ctp->tok); }
^@[A-Z]+	{ if (set_rws()) xreturn(DCF); } 
<s_sol>@[a-z]+	{ BEGIN ss(0); if (set_ctp()) xreturn(ctp->tok); }
[ \t]+       	{ /* empty */ }
\n		{ BEGIN ss(0); }

<s_word>[^ \t\n]+ { BEGIN ss(0); ksdup(yytext); xreturn(WORDSPEC); }
<s_word>[ \t]+	  { /* empty */ }
<s_text>[^\n]+    { BEGIN ss(0); ksdup(yytext); xreturn(TEXTSPEC); }

	/* CF-GW-POS TOKENIZER */

<s_cf>[^\[\] \t\n][^\[\]\n]*[^\[\] \t\n]        { ytdup(); xreturn(CF); }
<s_cf>[^\[\] \t\n]         { ytdup(); xreturn(CF); }
<s_cf>"["      	   	   { BEGIN ss(s_gw); xreturn(yytext[yyleng-1]); }
<s_gw>([^\]\n]+)	   { ytdup(); xreturn(GW); }
<s_cf,s_gw>"]"     	   { BEGIN ss(after_s_gw); xreturn(yytext[0]); }
<s_pos>[^ \t\n]+[ \t]*/\n  { BEGIN ss(0); ytdup(); xreturn(POS); }
<s_pos>[^ \t\n]+[ \t]*	   { BEGIN ss(after_s_pos); ytdup(); xreturn(POS); }
<s_pos>[ \t]*/\n	   { BEGIN ss(0); }
<s_gw>\n		   { BEGIN ss(0); xreturn(EOL); }
<s_cf,s_gw,s_pos>[ \t]+	   { /* empty */ }

	/* ALLOW TOKENIZER */

<s_allow>=	    { xreturn(yytext[0]); }
<s_allow>[^ \t\n=]+ { ksdup(yytext); xreturn(WORDSPEC); }
<s_allow>\n	    { BEGIN ss(0); }
<s_allow>[ \t]+	    { /* empty */ }

	/* BASES TOKENIZER */

<s_b>[^ \t\n]+	{ return base_tok(); }
<s_b>\n/"@"	{ BEGIN ss(0); base_state = BASE_PRI; }
<s_b>[ \t]+	   	  { /* empty */ }

	/* FORM TOKENIZER */

<s_f>%[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FLANG); }
<s_f>@[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FRWS); }
<s_f>\/[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FBASE); }
<s_f>\+[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FCONT); }
<s_f>\$[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FNORM); }
<s_f>##[^ \t\n]+[ \t]*	  { ytdupn(2); xreturn(FMORPH2); }
<s_f>#[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FMORPH); }
<s_f>\*[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FSTEM); }
<s_f>[^+$#*/\n][^ \t\n]*[ \t]* { ytdup(); xreturn(FFORM); }
<s_f>\n/"@"		  { BEGIN ss(0); xreturn(END_FORM); }
<s_f>[ \t]+	   	  { /* empty */ }

	/* SENSE TOKENIZER */

<s_s>[\%.#][^ \t\n]+[ \t]* { ytdupn(1); return(yytext[0]); }
<s_s>\[[^ \t\n\]]+\][ \t]* { ytdupn(1); return(yytext[0]); }
<s_s>\[[^ \t\n]+[ \t]*     { ytdupn(1); return(yytext[0]); } /* error missing ']' */
<s_s>[^ \t\n]+[ \t]*       { BEGIN ss(s_text); ytdup(); return(POS); }
<s_s>[ \t]+	   	  { /* empty */ }

	/* INCLUDES TOKENIZER */

^@include[ \t]*		  { BEGIN ss(s_incl); }
<s_incl>[^ \t\n]+   	  { includeo(yytext); }

<s_flush>.*		  { BEGIN ss(0); }

%%

void
set_after(void)
{
  switch (ctp->tok)
    {
      case ENTRY:
	after_s_renmrg = after_s_entsns = s_cf;
	after_s_gw = s_pos;
	after_s_pos = 0;
        break;
      case SENSE:
      case SENSEL:
	/*after_s_renmrg = after_s_entsns = after_s_gw = s_spos;*/
	break;
      case ALIAS:
	after_s_pos = 0;
        break;
      case PARTS:
	after_s_pos = s_cf;
        break;
      default:
        break;
    }
}

static int
set_rws(void)
{
  struct cbdrws *rws = NULL;
  const char *atname = yytext;
  while (*atname && '@' != *atname)
    ++atname;
  if (*atname)
    ++atname;
  rws = cbdrws(atname,strlen(atname));
  if (rws)
    {
      BEGIN ss(s_text);
      yylval.text = (char*)pool_copy((ucp)rws->lang, curr_cbd->pool);
      return 1;
    }
  else
    {
      BEGIN ss(s_flush); yylloc.file=efile; yylloc.line=yylineno;
      vyyerror(yylloc,"unknown RWS %s\n",atname, NULL);
      return 0;
    }
}

static int
set_ctp(void)
{
  const char *atname = yytext;
  while (*atname && '@' != *atname)
    ++atname;
  if (*atname)
    ++atname;
  ctp = cbdtags(atname,strlen(atname));
  if (!ctp)
    {
      BEGIN ss(s_flush); yylloc.file=efile; yylloc.line=yylineno;
      vyyerror(yylloc,"unknown at-command %s\n",atname, NULL);
      return 0;
    }
  else
    {
      if (!strcmp(atname, "end"))
	{
	  
	}
      BEGIN ss(ctp->ss); set_after();
    }
  return 1;
}

static int
tg_set_flags(char *f)
{
  while (*f)
    {
      if (*f == '*')
	star = 1;
      else if (*f == '!')
	bang = 1;
      else
	fprintf(stderr, "cbd: internal error: unhandled flag char %c\n", *f);
    }
  return ctp != NULL;
}

static int
base_tok(void)
{
  int ret_state = BASE_PRI;

  char *t = yytext;
  char *e = yytext + strlen(yytext);

  if ('(' == *t)
    {
      ++t;
      ret_state = BASE_ALT;
      if (';' == e[-1])
	{
	  if (')' == e[-2])
	    e[-2] = '\0';
	  else
	    e[-1] = '\0';
	  base_state = BASE_PRI;
	}
      else
	{
	  if (',' == e[-1])
	    e[-1] = '\0';	      
	  base_state = BASE_ALT;
	}
       }
  else if ('%' == *t)
    {
      ret_state = LANGSPEC;
      ++t;
    }
  else
    {
      ret_state = base_state;
    }
  ytdup();
  return ret_state;
}

#if 0
void
alt_wrapup(int clean)
{
  if (clean)
    clean_alt_paren(); 
  (void)pool_copy((ucp)unspace(b_alt_tok),curr_cbd->pool);
  free(b_alt_tok); 
  b_alt_tok = NULL;
}

static void
basesc(const char *type)
{
  yylloc.file=efile;
  yylloc.line=yylineno; 
  vyyerror(yylloc,"misplaced %s semi-colon\n", type, NULL);
}

static void
baseco(const char *type)
{
  yylloc.file=efile;
  yylloc.line=yylineno; 
  vyyerror(yylloc,"misplaced %s comma\n", type, NULL);
}

static void
clean_alt_paren(void)
{
  b_alt_tok[strlen(b_alt_tok)-1] = '\0';
}
#endif
       
static char *
unspace(char*s)
{
  int i = strlen(s);
  while (isspace(s[i-1]))
    --i;
  if (i >= 0)
    s[i] = '\0';
  return s;
}

static void
cbd_user_action(void)
{
  if (cbd_flex_debug)
    {
      if (cmt_queue)
	fprintf(stderr, "%s:%d: cmt_queue has %d items\n", efile, yylineno, (int)list_len(cmt_queue));
      fprintf(stderr, "%s:%d: cbd-flex: state=%s tok=::%s::\n", efile, yylineno, ssnames[mystate], yytext);
    }
  yylloc.file = efile;
  yylloc.line = yylineno;
}

#if 1
#define tg_validate(x) (x)
#else
static int
tg_validate(int retval)
{
  extern int input_validation;
  extern char *rnvcbd_yytext;

  if (input_validation && yytext && *yytext)
    {
      if (retval == DCF)
        { rnvcbd_yytext = yytext , rnvcbd_token(efile, yylineno, mystate, "@dcf"); }
      else
        rnvcbd_token(efile, yylineno, mystate, yytext);
    }
  return retval;
}
#endif

/* include_stack_ptr indexes into include_stack and gives the current
   state.  At the initial state, we push the top-level state into
   include_stack[0] and then the first include is include_stack[1].

   On closing an include, the state pointed to by include_stack_ptr is
   the one we are shutting down.
 */

static void
includeo(char *file)
{
  if ( include_stack_ptr >= MAX_INCLUDE_DEPTH )
    {
      fprintf( stderr, "Includes nested too deeply" );
      exit( 1 );
    }

  if (!include_stack_ptr)
    {
      /* ensure top-level state is saved first on stack */
      include_stack[include_stack_ptr].file = efile;
      include_stack[include_stack_ptr].line = yylineno;
      include_stack[include_stack_ptr].yycb = YY_CURRENT_BUFFER;
      include_stack[include_stack_ptr].fp = yyin;
    }

  ++include_stack_ptr;      

  efile = include_stack[include_stack_ptr].file = (char*)pool_copy((ucp)yytext,curr_cbd->pool);
  include_stack[include_stack_ptr].yycb = YY_CURRENT_BUFFER;
  include_stack[include_stack_ptr].line = yylineno;
  include_stack[include_stack_ptr].fp = yyin = xfopen( yytext, "r" );

  yylineno = 1;

  if (cbd_flex_debug) { fprintf(stderr, "%s:%d: include switching input to new file\n", efile, yylineno); }
  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
  BEGIN(ss(INITIAL));
}

#if 0
static void
includec(void)
{
  if (include_stack_ptr--)
    {
      fclose(include_stack[include_stack_ptr+1].fp);
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer(include_stack[include_stack_ptr].yycb);
      efile = include_stack[include_stack_ptr].file;
      yylineno = include_stack[include_stack_ptr].line;
      yyin = include_stack[include_stack_ptr].fp;
      if (cbd_flex_debug)
        {
          fprintf(stderr, "%s:%d: include of %s complete\n", efile, yylineno,
        	  	           		             include_stack[include_stack_ptr+1].file);
        }
    }
}
#endif

void
cbd_l_init(Iome_io *ip)
{
  if (!ip->use_stdio)
    {
      errmsg_fn = ip->fn;
      efile = (char*)ip->fn;
      if ((ip->fp = xfopen(efile, "r")))
	yyrestart(ip->fp);
    }
}
void
cbd_l_term(void)
{
  
}


%option prefix="tgi"
%option outfile="tgi-l.c"
%option never-interactive noyywrap yylineno
%{

#include <ctype128.h>
#include "grammar.h"
#include "gx.h"
#include "tgi.tab.h"
#include "rnvtgi.h"
#include "npool.h"
#define YY_NO_UNISTD_H

#define yylloc tgilloc
#define yylval tgilval

/* keep-space-dup */
#define ksdup(s) yylval.text = (char*)npool_copy((unsigned char *)(s),curr_cbd->pool)

/* yytext-dup strips beginning/end spaces */
#define ytdup() (yylval.text = (char*)npool_copy((ucp)unspace(yytext),curr_cbd->pool))
#define ytdupn(n) yylval.text = (char*)npool_copy((ucp)unspace(yytext+(n)),curr_cbd->pool)
#define yi(tok) yylval.i = tok;
#define yln(s) if (ylntrace) {fprintf(stderr, "%s: %d\n",(s),yylineno);}

static int after_s_entsns = 0;
static int after_s_gw = 0;
static int after_s_pos = 0;
static int after_s_lang = 0;
static int after_s_renmrg = 0;
static int yycolumn = 0;
extern char *efile;

extern int bang, star;
extern List *cmt_queue;

static struct cbdtag *ctp;

static void baseco(const char *type);
static void basesc(const char *type);
static int set_ctp(void);
static int tg_set_flags(char *f);
static int tg_validate(int retval);

static char *b_alt_tok = NULL;
static void alt_wrapup(int clean);
static void clean_alt_paren(void);
static char *unspace(char*s);
static void gx_user_action(void);
#define mtch(x) fprintf(stderr,"matched %s",(x))

#define YY_USER_ACTION gx_user_action();

int ylntrace = 0;
extern int flextrace;
int mystate = 0;
#define ss(arg) (mystate=arg)

char *ssnames[] = {
"0", "s_allow", "s_b", "s_bp", "s_b",
"s_cf", "s_gw", "s_pos", "s_spos",
"s_f", "s_s", "s_sw",
"s_proj", "s_lang", "s_text", "s_word", "s_entsns", "s_why",
"s_comment", "s_elang", "s_oid", "s_flush", "s_incl", 
};

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
static void includec(void);
static void includeo(char *file);

YY_BUFFER_STATE bp;

#define xreturn(x) return(tg_validate(x))
%}

%x s_sol s_flag
%x s_b s_bp s_ba
%x s_cf s_gw s_pos s_spos
%x s_f s_incl s_flush

%%
^[-+>]+		{ BEGIN ss(s_sol); 	 return(yytext[0]); }
^@[a-z]+	{ if (set_ctp()) xreturn(ctp->tok); }
^@[a-z]+/[!*]+	{ if (set_ctp()) BEGIN ss(s_flag); }
<s_flag>[!*]+	{ tg_set_flags(yytext) ; xreturn(ctp->tok); }
^@[A-Z]+	{ if (set_ctp()) xreturn(ctp->tok); } 
<s_sol>@[a-z]+	{ if (set_ctp()) xreturn(ctp->tok); }
[^ \t\n]+  	{ ksdup(yytext); xreturn(TEXT); }
[ \t]+		/* empty */
\n		{ yycolumn=1; if (ctp->eol_sp) xreturn(TEXT); else xreturn(EOL); }

<s_cf>[^\[\] \t\n][^\[\]\n]*[^\[\] \t\n]        { ytdup(); xreturn(CF); }
<s_cf>[^\[\] \t\n]         { ytdup(); xreturn(CF); }
<s_cf>"["      	   	   { BEGIN ss(s_gw); xreturn(yytext[yyleng-1]); }
<s_gw>([^\]\n]+)	   { ytdup(); xreturn(GW); }
<s_cf,s_gw>"]"     	   { BEGIN ss(after_s_gw); xreturn(yytext[0]); }
<s_pos>[^ \t\n]+[ \t]*/\n  { BEGIN ss(0); ytdup(); xreturn(POS); }
<s_pos>[^ \t\n]+[ \t]*	   { BEGIN ss(after_s_pos); ytdup(); xreturn(POS); }
<s_pos>[ \t]*/\n	   { BEGIN ss(0); }

^@bases[ \t]*";"[ \t]*	  { BEGIN ss(s_bp); basesc("initial"); xreturn(BASES); }
^@bases[ \t]*	 	  { BEGIN ss(s_bp); xreturn(BASES); }
<s_bp>%[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(LANGSPEC); }
<s_bp>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); }
<s_bp>\;[ \t]*\;[ \t]*     { BEGIN ss(s_bp); basesc("double"); }
<s_bp>\;[ \t]+    	  { BEGIN ss(s_bp); }
<s_bp>[^; \t\n]+[ \t]*/"(" { BEGIN ss(s_ba); ytdup(); xreturn(BASE_PRI); }
<s_bp>[^; \t\n]+[ \t]* 	  { ytdup(); xreturn(BASE_PRI); }
<s_ba>[ \t]*";"[ \t]*/\n	  { BEGIN ss(0); yycolumn = 1; basesc("final"); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }
<s_ba>,[ \t]*,		  { baseco("double"); unput(','); }
<s_ba>,[ \t]*		  { if (b_alt_tok) { alt_wrapup(0); xreturn(BASE_ALT); } }
<s_ba>"("[ \t]*,		  { baseco("initial"); unput('('); }
<s_ba>"("[^;, \t\n]+[ \t]* { b_alt_tok = strdup(yytext+1); }
<s_ba>[^;, \t\n]+[ \t]* 	  { b_alt_tok = strdup(yytext); }
<s_ba>\;[ \t]*\;[ \t]*     { BEGIN ss(s_bp); basesc("double"); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }
<s_ba>\;[ \t]*    	  { BEGIN ss(s_bp); if (b_alt_tok) { alt_wrapup(1); xreturn(BASE_ALT); } }

^@form[ \t]+	  	  { BEGIN ss(s_f); xreturn(FORM); }
^@form![ \t]+	  	  { BEGIN ss(s_f); bang = 1; xreturn(FORM); }
<s_f>%[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FLANG); }
<s_f>@[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FRWS); }
<s_f>\/[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FBASE); }
<s_f>\+[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FCONT); }
<s_f>\$[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FNORM); }
<s_f>##[^ \t\n]+[ \t]*	  { ytdupn(2); xreturn(FMORPH2); }
<s_f>#[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FMORPH); }
<s_f>\*[^ \t\n]+[ \t]*	  { ytdupn(1); xreturn(FSTEM); }
<s_f>[^+$#*/\n][^ \t\n]*[ \t]* { ytdup(); xreturn(FFORM); }

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
        after_s_renmrg = after_s_entsns = s_cf; after_s_gw = s_pos; after_s_pos = 0;
        break;
      case SENSE:
      case SENSEL:
	after_s_renmrg = after_s_entsns = after_s_gw = s_spos;
	break;
      case ALIAS:
	after_s_pos = 0;
        break;
      case PARTS:
	after_s_pos = 0;
        break;
      default:
        break;
    }
}

static int
set_ctp()
{
  ctp = cbdtags(yytext,strlen(yytext));
  if (!ctp)
    {
      BEGIN ss(s_flush); yylloc.file=efile; yylloc.first_line=yylineno;
      vyyerror(yylloc,"unknown at-command %s\n",yytext, NULL);
      return 0;
    }
  else
    {
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
	fprintf(stderr, "gx: internal error: unhandled flag char %c\n", *f);
    }
  return ctp != NULL;
}

void
alt_wrapup(int clean)
{
  if (clean)
    clean_alt_paren(); 
  (void)npool_copy((ucp)unspace(b_alt_tok),curr_cbd->pool);
  free(b_alt_tok); 
  b_alt_tok = NULL;
}

static void
basesc(const char *type)
{
  yylloc.file=efile;
  yylloc.first_line=yylineno; 
  vyyerror(yylloc,"misplaced %s semi-colon\n", type, NULL);
}

static void
baseco(const char *type)
{
  yylloc.file=efile;
  yylloc.first_line=yylineno; 
  vyyerror(yylloc,"misplaced %s comma\n", type, NULL);
}

static void
clean_alt_paren(void)
{
  b_alt_tok[strlen(b_alt_tok)-1] = '\0';
}

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
gx_user_action(void)
{
  if (flextrace)
    {
      if (cmt_queue)
	fprintf(stderr, "%s:%d: cmt_queue has %d items\n", efile, yylineno, (int)list_len(cmt_queue));
      fprintf(stderr, "%s:%d: flextrace: state=%s tok=::%s::\n", efile, yylineno, ssnames[mystate], yytext);
    }
  yylloc.file = efile;
  yylloc.first_line = yylloc.last_line = yylineno;
  yylloc.first_column = yylloc.last_column;
  for(int i = 0; yytext[i] != '\0'; i++)
    {
      if (yytext[i] == '\n')
        {
          yylloc.last_line++;
          yylloc.last_column = 0;
        }
      else 
        {
          yylloc.last_column++;
        }
    }
}

static int
tg_validate(int retval)
{
  extern int input_validation;
  extern char *rnvtgi_yytext;
  if (input_validation && yytext && *yytext)
    {
      if (retval == DCF)
        { rnvtgi_yytext = yytext , rnvtgi_token(efile, yylineno, mystate, "@dcf"); }
      else
        rnvtgi_token(efile, yylineno, mystate, yytext);
    }
  return retval;
}

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

  efile = include_stack[include_stack_ptr].file = (char*)npool_copy((ucp)yytext,curr_cbd->pool);
  include_stack[include_stack_ptr].yycb = YY_CURRENT_BUFFER;
  include_stack[include_stack_ptr].line = yylineno;
  include_stack[include_stack_ptr].fp = yyin = xfopen( yytext, "r" );
  yylineno = 1;

  if (flextrace) { fprintf(stderr, "%s:%d: include switching input to new file\n", efile, yylineno); }
  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
  BEGIN(ss(INITIAL));
}

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
      if (flextrace)
        {
          fprintf(stderr, "%s:%d: include of %s complete\n", efile, yylineno,
        	  	           		             include_stack[include_stack_ptr+1].file);
        }
    }
}

void
tgi_l_init(struct iom_io *ip)
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
tgi_l_term(void)
{
  
}


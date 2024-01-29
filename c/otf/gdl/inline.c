#include <stdio.h>
#include <ctype128.h>
#include <stdlib.h>
#include <string.h>
#include "warning.h"
#include "globals.h"
#include <cdf.h>
#include "tree.h"
#include "tokenizer.h"
#include "graphemes.h"
#include "pool.h"
#include "gdlopts.h"
#include "gdl.h"
#include "inline.h"
#include "lang.h"
#include "splitwords.h"
#include "block.h"
#include "note.h"
#include "nonx.h"

#define CHECKFORMS 0

#ifndef strdup
char *strdup(const char*);
#endif

#undef curr_lang
#define curr_lang curr_lang_ctxt

extern char *new_note_id(int);

int ods_cols = 0;
int ods_mode = 0;

int suppress_lem = 0;

static int grouped_det = 0; /* a bandaid to separate 
			    KI.{d}NANNA 
			  from 
			    {ama:gan}
		       */
static int np_already_set = 0;

struct node *surro_wp = NULL, *surro_atpt = NULL;

extern FILE*fp_forms;
extern int check_only;
extern int need_lemm, do_show_insts;
extern int verbose;

extern void set_or_append_attr(struct node *n, enum a_type atype, const char *aname, unsigned char *text);

static void (*lemm_reset_form_p)(const char *ref, const char *form) = NULL;
static void (*lemm_save_form_p)(const char *ref, const char *lang, 
				const char *formstr,struct lang_context *,
				const char *field) = NULL;
static void (*lemm_unform_p)(void) = NULL;

static int logo_word_lang(struct node *wp, struct token *tp);
static const char *grapheme_id(void);

const unsigned char *breakStart = NULL;
const unsigned char *surroStart = NULL;
const unsigned char *statusStart = NULL;

int in_g_surro = 0;
int in_long_surro = 0;
int surro_mark_grapheme = 0;
static int split_word_unfinished;
int in_split_word = 0;
int max_cells = 1, curr_cell = 0;
int punct_word = 0;
int w_sparse_lem = 0;

struct lang_context *word_lang = NULL;

int nline_words = 0, line_words_alloced = 0;
struct node *atpt = NULL; /* attach point for grapheme nodes */
/*List *group_stack = NULL;*/
/*struct node *group_node = NULL;*/
struct node **line_words = NULL;

/* Mutually exclusive flag tokens for implied, supplied, excised and
   maybe bracketing. */
enum t_type mutex_state = notoken;

enum t_type square_bracket = notoken;

struct node *watch = NULL;

int suppress_next_hyphen = 0, suppress_hyphen_delay = 0;

enum a_type fixed_attr_n[type_top];
const unsigned char * fixed_attr_v[type_top];
const unsigned char *default_ftype = ucc("default");

const char *default_dialect = NULL /*"SB"*/;
const char *curr_dialect = NULL /*"SB"*/;

static struct node *surro_node;
static struct node *prev_g = NULL; /* cleared after field/cell/line */
static struct node *last_word = NULL;
static const char *curr_field = NULL;
static struct token *pending_ub = NULL;
static struct token *word_tokp = NULL;
static const unsigned char *curr_logolang;
static unsigned char *formsbuf;
static unsigned char *forms_insertp;
static ssize_t formsbuf_size;
static enum t_type word_init_mutex = notoken;

/*static int g_surro(int tindex);*/
static struct token *surro_norm(int tindex);
static int nfields = 0;
static int long_surro(int tindex);
static int has_cells(void);
static int has_fields(void);
static void langhand(struct node *wp, struct token *tp);
static int nowt_to_render(struct node *n);
static void process_cells(struct node *parent, unsigned char *line_id);
static void process_fields(struct node *parent, int start, int end, unsigned char *line_id);
static void process_words(struct node *parent, int start, int end, int with_word_list);
static void wrapup_word(struct node *wp, enum t_type trigger);

static int logoline;
static int hacked_word_lang = 0;
static int word_id;
static char word_id_buf[32];
static char *word_id_insertp;

static int in_hash = 0;
static int split_flag = 0;

#define set_ub(wdp,ubtype,ubp) appendAttr((wdp),attr((ubtype),(ubp)->data))

#define no_norm(err) \
  if (tp->lang && tp->lang->mode == m_normalized) \
    { \
      vwarning("'%s' not allowed in normalized text",err); \
    }

static char grapheme_id_buf[64], *grapheme_id_insertp;
static int grapheme_id_index = 0;

extern int use_unicode;

static int nonw_id = 1;

/* From https://www.geeksforgeeks.org/convert-base-decimal-vice-versa/
   Function to convert a given decimal number to a base 'base'
*/ 
/* To return char for a value. For example '2' 
    is returned for 2. 'A' is returned for 10. 'B' 
    for 11 
*/
static char
reVal(int num) 
{ 
  return (char)((num-1) + 'a'); 
} 
  
/* Utility function to reverse a string */
static void
strev(char *str) 
{ 
    int len = strlen(str); 
    int i; 
    for (i = 0; i < len/2; i++) 
    { 
        char temp = str[i]; 
        str[i] = str[len-i-1]; 
        str[len-i-1] = temp;
    } 
}
static char*
fromDeci(char *res, int base, int inputNum) 
{ 
    int index = 0;  // Initialize index of result 
  
    // Convert input number is given base by repeatedly 
    // dividing it by base and taking remainder 
    while (inputNum > 0) 
    { 
        res[index++] = reVal(inputNum % base); 
        inputNum /= base; 
    } 
    res[index] = '\0'; 
  
    // Reverse the result 
    strev(res); 
  
    return res; 
}

static void
set_nonw_rewrite_gid(struct node *n)
{
  int i;
  for (i = 0; i < n->children.lastused; ++i)
    {
      struct node *c = n->children.nodes[i];
      if (c)
	{
	  /* In a surro this can be a nested word, in which case quit rewriting the IDs */
	  if (c->names && !strcmp(c->names->pname,"g:w"))
	    return;
	  else
	    {
	      const unsigned char *a = getAttr(c,"xml:id");
	      if (a && *a)
		setAttr(c,a_xml_id,(const unsigned char *)grapheme_id());
	      if (c->children.lastused)
		set_nonw_rewrite_gid(c);
	    }
	}
    }
}

static void
set_nonw_id(struct node *wp)
{
  const unsigned char *p = NULL; 
  char *p2 = NULL;
  static char buf[10];

  if (wp)
    {
      p = getAttr(wp,"xml:id");
      if (p && strlen((char*)p))
	{
	  const unsigned char *ptmp = p + strlen((char*)p);
	  while (ptmp > p)
	    if (ptmp[-1] == '.')
	      break;
	    else
	      --ptmp;
	  --word_id; --word_id;
	  /* This cast to char * from const is safe because the xml id is in a string pool */
	  sprintf((char*)ptmp, "%d", word_id++);
	  p2 = malloc(strlen((char*)p)+strlen(".aa")+1);
	  sprintf(p2, "%s.%s", p, fromDeci(buf, 26, nonw_id++));
	  setAttr(wp,a_xml_id,(const unsigned char*)p2);
	  xstrcpy(word_id_buf,p2);
	  grapheme_id_reset();
	  set_nonw_rewrite_gid(wp);
	  free(p2);
	}
      else
	{
	  const char *t = (const char *)getAttr(wp,"type");
	  if (strcmp(prog,"gdlme2")
	      && (*t
		  && strcmp(t, "notelink")
		  && strcmp(t, "vari"))
	      )
	    vwarning("internal error: attempt to reset never-set nonw_id");
	}
    }
}

static void
vck(struct node *a, struct node *w)
{
  if ((a && !strcmp(a->names->pname,"g:v"))
      || (w && !strcmp(w->names->pname,"g:v")))
    {
      vwarning("internal error: suspicious attach point %s",
	       a ? a->names->pname : w->names->pname);
    }
}

static struct node *
lastGrapheme(struct node *np)
{
  struct node *gp = np;
  struct node *deepest = NULL;
  while (1)
    {
      gp = lastChild(gp);
      if (gp)
	{
	  const char *n = gp->names ? gp->names->pname : "";
	  deepest = gp;
	  if (strlen(n) == 3
	      && (strchr("cnpsvxq",n[2])
		  || (!grouped_det && strchr("d",n[2]))))
	    break;
	}
      else
	break;
    }
  return deepest ? deepest : np;
}

static void
ascii_check(const unsigned char *s)
{
  while (*s)
    if (*s > 127)
      {
	warning("line contains non-ASCII characters");
	return;
      }
    else
      ++s;
}

void
grapheme_id_reset(void)
{
  extern const char *curr_word_id(void);
  grapheme_id_index = 0;
  strcpy(grapheme_id_buf, curr_word_id());
  grapheme_id_insertp = grapheme_id_buf + strlen(grapheme_id_buf);
}
static const char *
grapheme_id(void)
{
  sprintf(grapheme_id_insertp, ".%d", grapheme_id_index++);
  return grapheme_id_buf;
}

static int
has_cells()
{
  struct token **tp = tokens;
  while (*tp)
    if ((*tp)->type == cell)
      return 1;
    else
      ++tp;
  return 0;
}

static int
has_fields()
{
  struct token **tp = tokens;
  while (*tp)
    if ((*tp)->type == field)
      return 1;
    else
      ++tp;
  return 0;
}

void
forms_init()
{
  /*FIXME: formsbuf needs to be resized dynamically with tests against
    overflow */
  formsbuf_size = 4096000;
  formsbuf = malloc(formsbuf_size);
  forms_insertp = formsbuf;
}

void
forms_term()
{
  free(formsbuf);
  forms_insertp = formsbuf = NULL;
  formsbuf_size = 0;
}

void
inline_init()
{
  fixed_attr_n[implo]  = a_g_status;
  fixed_attr_n[supplo] = a_g_status;
  fixed_attr_n[exciso] = a_g_status;
  fixed_attr_n[eraso] = a_g_status;
  fixed_attr_n[maybeo] = a_g_status;
  fixed_attr_n[someo] = a_g_status;
  fixed_attr_n[glosso] = a_g_type;
  fixed_attr_n[smetao] = a_g_type;
  fixed_attr_n[damago] = a_g_break;
  fixed_attr_n[sforce] = a_g_role;
  fixed_attr_n[lforce] = a_g_role;
  fixed_attr_v[implo]  = ucc("implied");
  fixed_attr_v[supplo] = ucc("supplied");
  fixed_attr_v[exciso] = ucc("excised");
  fixed_attr_v[eraso] = ucc("erased");
  fixed_attr_v[maybeo] = ucc("maybe");
  fixed_attr_v[someo] = ucc("some");
  fixed_attr_v[glosso] = ucc("lang");
  fixed_attr_v[smetao] = ucc("text");
  fixed_attr_v[damago] = ucc("missing");
  fixed_attr_v[sforce] = ucc("sign");
  fixed_attr_v[lforce] = ucc("logo");
}

void
inline_term()
{
  if (line_words)
    {
      free(line_words);
      line_words_alloced = 0;
      line_words = NULL;
    }
}

void
inline_functions(void (*lemm_save_form_arg)(const char *,const char*,
					    const char*,struct lang_context*,
					    const char *field),
		 void (*lemm_unform_arg)(void),
		 void (*lemm_reset_form_arg)(const char *,const char *))
{
  lemm_save_form_p = lemm_save_form_arg;
  lemm_unform_p = lemm_unform_arg;
  lemm_reset_form_p = lemm_reset_form_arg;
}

static void
langhand(struct node *wp, struct token *tp)
{
  appendAttr(wp,attr(a_xml_lang,ucc(tp->lang->fulltag)));
  if (tp->fwhost)
    setAttr(wp,a_fwhost,(unsigned const char *)strdup(tp->fwhost));
}

extern int tokcheck(void);

void
tlit_reinit_inline(int with_word_list)
{
  mutex_state = square_bracket = notoken;
  if (with_word_list)
    nline_words = 0;
  g_reinit();
  in_g_surro = w_sparse_lem = 0;
}

struct node *
atpt_ancestor_gg(struct node *lastc)
{
  lastc = lastc->parent;
  while (lastc)
    {
      if (lastc->etype == e_g_gg)
	return lastc;
      lastc = lastc->parent;
  }
  return NULL;
}

struct node *
atpt_ancestor_or_self_gg(struct node *lastc)
{
  while (lastc)
    {
      if (lastc->etype == e_g_gg)
	return lastc;
      lastc = lastc->parent;
    }
  return NULL;
}

int
atpt_no_logo(struct node *np)
{
  while (np)
    {
      const char *a = (const char *)getAttr(np, "g:type");
      if (!xstrcmp(a, "logo"))
	return 0;
      np = np->parent;
    }
  return 1;
}

void
tlit_parse_inline(unsigned char *line, unsigned char *end, struct node*lnode, 
		  int word_id_base, int with_word_list, unsigned char *line_id)
{
  /*struct lang_context *line_lang = NULL;*/

  if (verbose > 1)
    fprintf(stderr, "%d\n", lnum);

  nonw_id = 1;
  
  tlit_reinit_inline(with_word_list);
  tokenize_reinit();
  nfields = 0;
  curr_field = NULL;

#if 0
  if (text_lang)
    line_lang = text_lang;
  else
    line_lang = global_lang;
#endif
  
  if (!use_unicode)
    ascii_check(line);

  tokenize(line,end);

  if (!curr_lang || curr_lang->mode != m_alphabetic) /* FIXME: tokcheck should be tuned for alpha vs graphemic */
    {
      if (tokcheck())
	{
	  status = 1;
	  return;
	}
    }
  word_id = 1 * word_id_base;
  xstrcpy(word_id_buf,line_id);
  xstrcat(word_id_buf,".");
  word_id_insertp = word_id_buf + xxstrlen(word_id_buf);
  mutex_state = notoken;

  /* (TOO HARSH)
  if (status)
    return;
  */

  if (fp_forms)
    {
      static int did_first_line = 0;
      if (!did_first_line)
	{
	  ++did_first_line;
	  list_locator(fp_forms);
	}
      else if (!in_split_word)
	{
	  fputc('\n',fp_forms);
	  list_locator(fp_forms);
	}
      /* fprintf(fp_forms,"%d ", lnum); */
    }

  curr_cell = 0;

  if (has_cells())
    process_cells(lnode,line_id);
  else if (has_fields())
    process_fields(lnode,0,last_token,line_id);
  else
    process_words(lnode,0,last_token, with_word_list);

 if (surro_node)
   {
     warning("unclosed surrogate");
     surro_node = NULL;
   }
   
  return;
}

void
max_cells_attr(struct node *tp)
{
  if (!*getAttr(tp,"cols"))
    {
      extern int ods_mode, ods_cols;
      char colsbuf[128];
      if (ods_mode)
	ods_cols = max_cells;
      (void)sprintf(colsbuf,"%d",max_cells);
      appendAttr(tp,attr(a_cols,pool_copy(ucc(colsbuf))));
      max_cells = 1;
    }
}

static int
nextcell(int start)
{
  while (start < last_token && cell != tokens[start]->type)
    ++start;
  return start;
}

static int
nextfield(int start)
{
  while (start < last_token && cell != tokens[start]->type && field != tokens[start]->type)
    ++start;
  return start;
}

static void
process_cells(struct node *parent, unsigned char *line_id)
{
  int start = 0;
  int end = last_token;
  int ncells = 0;
  if (tokens[start]->type == cell)
    ++start;
  while (start < end)
    {
      struct node *cp = elem(e_c,NULL,lnum,CELL);
      int next_cell = nextcell(start);
      curr_cell = ncells + 2; /* +1 because we didn't
				 increment yet, and +1
				 for the line number col */
      appendChild(parent,cp);
      if (tokens[start]->type == cellspan)
	{
	  appendAttr(cp,attr(a_span,tokens[start]->data));
	  ncells += atoi(tokens[start]->data);
	  ++start;
	}
      else
	{
	  appendAttr(cp,attr(a_span,(const unsigned char *)"1"));
	  ++ncells;
	}
      if (has_fields())
	{
	  process_fields(cp,start,next_cell,line_id);
	  start = next_cell + 1;
	}
      else
	{
	  process_words(cp,start,next_cell, 1);
	  start = next_cell + 1;
	}
    }
  if (ncells > max_cells)
    max_cells = ncells;
}

static void
process_fields(struct node *parent, int start, int end, unsigned char *line_id)
{
  while (start < end)
    {
      struct node *cp = elem(e_f,NULL,lnum,FIELD);
      int next_field;
      unsigned char *f_id = malloc(strlen((const char *)line_id) + strlen(".f1000") + 1);
      if (++nfields > 9999)
	{
	  warning("too many fields in line; remainder ignored");
	  free(f_id);
	  return;
	}
      sprintf((char*)f_id,"%s.f%d",line_id,nfields);
      if (tokens[start] && tokens[start]->type == field)
	++start;
      if (tokens[start] && tokens[start]->type == ftype)
	{
	  extern int xcl_is_sparse_field(const char *f);
	  if (sparse_lem && xcl_is_sparse_field(tokens[start]->data))
	    w_sparse_lem = 1;
	  else
	    w_sparse_lem = 0;
	  curr_field = (const char *)tokens[start]->data;
	  cp->data = (unsigned char*)strdup(tokens[start]->data);
	  appendAttr(cp,attr(a_type,ucc(tokens[start++]->data)));
	}
      else
	{
	  appendAttr(cp,attr(a_type,default_ftype));
	  cp->data = default_ftype;
	  curr_field = NULL;
	}
      next_field = nextfield(start);
      if (!next_field)
	next_field = nextfield(start+1);
      appendChild(parent,cp);
      process_words(cp,start,next_field, 1);
      appendAttr(cp,attr(a_xml_id,f_id));
      free(f_id);
      start = next_field + 1;
    }
}

/* return the location of the agroupc that matched agroupo */
static int
process_agroup(struct node *parent, int start, int end)
{
  struct node *cp = elem(e_ag,NULL,lnum,AGROUP);
  static unsigned char formbuf[1024];
  int agroup_end = start+1;
  int i;
  appendAttr(cp,attr(a_ref,tokens[start]->data));
  appendChild(parent,cp);
  while (agroup_end < end 
	 && (tokens[agroup_end]->type != agroupc
	     || xstrcmp(tokens[start]->data,tokens[agroup_end]->data)))
    ++agroup_end;
  process_words(cp,start+1,agroup_end, 1);
  *formbuf = '\0';
  for (i = 0; i < cp->children.lastused; ++i)
    {
      const unsigned char *f = getAttr(cp->children.nodes[i],"form");
      if (strlen(cc(formbuf))+strlen(cc(f))+1 < 1024)
	{
	  if (*formbuf)
	    strcat((char*)formbuf,"_");
	  strcat((char*)formbuf,cc(f));
	}
      else
	vwarning("alignment group form too long: %s",formbuf);
    }
  appendAttr(cp,attr(a_form,formbuf));
  if (fp_forms)
    fprintf(fp_forms,"%%%s:%s ",getAttr(cp->children.nodes[0],"xml:lang"),formbuf);
  return agroup_end+1;
}

static enum t_type
prev_b_or_g(ssize_t tindex)
{
  while (tindex >= 0)
    {
      if (tokens[tindex]->type == detc)
	{
	  while (tindex > 0 && tokens[--tindex]->type != deto)
	    ;
	  --tindex; /* skip deto */
	}
      else if (tokens[tindex]->type == excisc)
	{
	  while (tindex > 0 && tokens[--tindex]->type != exciso)
	    ;
	  --tindex; /* skip exciso */
	  /* if this is [DELIM]<<a>> ignore the preceding boundary also */
	  if (tindex >= 0 && tokens[tindex]->class == bound)
	    --tindex;
	}
      else if (tokens[tindex]->class == text)
	return noop;
      else if (tokens[tindex]->class == bound)
	{
	  if ((tokens[tindex]->type == plus || tokens[tindex]->type == ilig)
	      && tindex > 0
	      && tokens[tindex-1]->class != bound)
	    {
	      return noop; /*FIXME: this is probably not well enough thought out*/
	    }
	  else
	    {
	      split_flag = !tindex;
	      return tokens[tindex]->type;
	    }
	}
      else
	{
	  switch (tokens[tindex]->type)
	    {
	    case ellipsis:
	    case icmt:
	      return noop;
	      break;
	    case detc:
	    case normc:
	    case field:
	    case cell:
	    case glosso:
	    case surro:
	      return space;
	      break;
	    default:
	      --tindex;
	      break;
	    }
	}
    }
  return space;
}

static struct token *
next_text(ssize_t tindex)
{
  while (tindex < last_token)
    {
      if (tokens[tindex]->class == text)
	return tokens[tindex];
      else
	++tindex;
    }
  return NULL;
}

static enum t_type
next_b_or_g(ssize_t tindex)
{
  while (tindex < last_token)
    {
      if (tokens[tindex]->type == deto)
	{
	  while (tindex < last_token 
		 && tokens[++tindex] 
		 && tokens[tindex]->type != detc)
	    ;
	  ++tindex; /* skip detc */
	}
      else if (tokens[tindex]->type == smetao)
	{
	  while (tindex < last_token 
		 && tokens[++tindex] 
		 && tokens[tindex]->type != smetac)
	    ;
	  ++tindex; /* skip detc */
	}
      else if (tokens[tindex]->type == glosso)
	{
	  while (tindex < last_token 
		 && tokens[++tindex] 
		 && tokens[tindex]->type != glossc)
	    ;
	  ++tindex; /* skip detc */
	}
      else if (tokens[tindex]->type == exciso)
	{
	  /* ignore all the excised material */
	  while (tindex < last_token 
		 && tokens[++tindex]
		 && tokens[tindex]->type != excisc)
	    ;
	  /* now also ignore any following boundaries because they
	     belong to the excised stuff */
	  while (tindex < last_token
		 && tokens[++tindex]
		 && tokens[tindex]->class == bound)
	    ;
	}
      else if (tokens[tindex]->class == text 
	       || tokens[tindex]->type == ellipsis
	       || tokens[tindex]->type == icmt)
	return noop;
      else if (tokens[tindex]->class == bound)
	{
	  if (tokens[tindex]->type == hyphen 
	      && (tindex+1) < last_token
	      && tokens[tindex+1]->type == newline
	      && !tokens[tindex+2])
	    {
	      split_flag = 1;
	      return noop;
	    }
	  else if ((tokens[tindex]->type == plus || tokens[tindex]->type == ilig)
		   && tindex < last_token
		   && tokens[tindex+1]->class != bound)
	    return noop; /*FIXME: this is probably not well enough thought out*/
	  else
	    return tokens[tindex]->type;
	}
      else if (tokens[tindex]->type == deto
	       || tokens[tindex]->type == normo
	       || tokens[tindex]->type == surro
	       || tokens[tindex]->type == surrc
	       || tokens[tindex]->type == cell
	       || tokens[tindex]->type == field)
	return space;
      else
	++tindex;
    }
  return space;
}

#define emit_vari() \
  if (pending_varo) \
    { \
      if (pending_varo == 2) \
	{ \
	  struct node *np = elem(e_g_nonw,NULL,lnum,WORD); \
	  set_nonw_id(np); \
	  appendAttr(np,attr(a_type, ucc("vari"))); \
	  appendChild(np,textNode((unsigned char*)(tokens[varo_tok]->data))); \
	  appendChild(parent,np); \
	} \
      pending_varo = 0; \
    }

static void
maybeAppendXid(struct node *np, const char *id)
{
  if (strncmp(id,"gdl",3) || !no_gdl_xids)
    appendAttr(np,attr(a_xml_id,ucc(id)));
}	      

static struct node *
init_word_func(struct node *parent, struct token *tp, 
	       int *pending_varo_p, int varo_tok,
	       int *logo_word, int with_word_list)
{
  struct node *wp = NULL;
  int pending_varo = 0;
  if (pending_varo_p)
    {
      pending_varo = *pending_varo_p;
      emit_vari();
      *pending_varo_p = pending_varo;
    }

  word_lang = tp->lang;
  hacked_word_lang = 0;
  if (logo_word)
    *logo_word = 0;
  wp = elem(tp->lang->mode==m_graphemic ? e_g_w : e_n_w,NULL,lnum,WORD);
  sprintf(word_id_insertp, "%d", word_id++);
  grapheme_id_reset();
  maybeAppendXid(wp,word_id_buf);
  langhand(wp,tp);
  appendChild(parent,wp);
  word_tokp = tp;
  word_init_mutex = mutex_state;
  if (with_word_list && (!sparse_lem || w_sparse_lem))
    {
      if (nline_words == line_words_alloced)
	{
	  line_words_alloced += 128;
	  line_words = realloc(line_words, 
			       line_words_alloced * sizeof(struct node*));
	}
      line_words[nline_words++] = wp;
    }

  return wp;
}

#if 0
static int
validate_norm(const unsigned char *n)
{
  const unsigned char *orig = n;
  while (*n)
    {
      /* this n-orig hack is to avoid having to test UTF8 for uppercase;
	 it means that in SZU the U errors b/c it is not in initial position */
      if (*n < 127 && isupper(*n) && (n - orig || isupper(n[1])))
	{
	  vwarning("%s: incorrect capitalization in normalized text",orig);
	  return 1;
	}
      ++n;
    }
  return 0;
}
#endif

static void
normnum(struct node *seg, const char *data)
{
  while (isdigit(*data))
    ++data;
  if (!*data)
    setAttr(seg, a_n_num, (const unsigned char*)"yes");
}

static void
process_words(struct node *parent, int start, int end, int with_word_list)
{
  struct node *wp = NULL;
  struct node *last_g = NULL; /* cleared after word */
  struct node *last_wp = NULL;
  int sforce_flag = 0;
  int lforce_flag = 0;
  int long_logo = 0;
  int logo_word = 0;
  int pending_varo = 0, varo_tok = -1;
  enum t_type group_flag = notoken;
  char *http = NULL;
  
  atpt = NULL;
  in_hash = logoline = 0;
  prev_g = last_word = NULL;
  while (start < end)
    {
      struct node *np = NULL;
      struct token *tp = tokens[start];
      enum t_type b_or_g = notoken;
      register const void *datap = NULL;

      vck(atpt,wp);

      if (tokens[start]->type == agroupo)
	{
	  start = process_agroup(parent,start,end);
	  ++start;
	  continue;
	}

      switch (tp->class)
	{
	case text:
	  if (!wp)
	    wp = init_word_func(parent, tp, &pending_varo, varo_tok, 
				&logo_word, with_word_list);
	  if (!tp->data)
	    {
	      if (tp->type != wm_absent && tp->type != wm_broken)
		;
	    }
	  datap = tp->data;
	  switch (tp->type)
	    {
	    case norm:
#if 0
	      if (validate_norm(datap))
		{
		  ++start;
		  continue;
		}
#endif
	      if (wp
		  && xstrcmp(wp->names->pname,"n:w")
		  && xstrcmp(wp->parent->names->pname,"g:surro"))
		{
		  if (wp->parent->names
		      && !xstrcmp(wp->parent->names->pname,"g:w")
		      && !xstrcmp(wp->names->pname,"g:d"))
		    {
		      setName(wp,e_n_w);
		      appendChild(wp,textNode(datap));
		    }
		  else
		    {
		      struct node *topw = wp->parent;
		      while (topw)
			{
			  if (!strcmp(topw->names->pname,"g:w"))
			    break;
			  topw = topw->parent;
			}
		      if (!surro_node && topw && topw->children.lastused)
			{
			  vwarning("%s: normalization not allowed inside word",datap);
			  ++start;
			  continue;
			}
		      else
			{
			  struct node *nw = elem(e_n_w,NULL,lnum,WORD);
			  struct node *seg = elem(e_n_s,NULL,lnum,GRAPHEME);
			  sprintf(word_id_insertp, "%d", word_id++);
			  maybeAppendXid(nw,word_id_buf);
			  langhand(nw,tp);
			  normnum(seg, datap);
			  appendChild(seg,textNode(datap));
			  appendChild(nw,seg);
			  appendChild(wp,nw);
			  last_g = seg;
			}
		    }
		}
	      else
		{
		  struct node *lastC  = (wp ? lastChild(wp) : wp);
		  struct node *seg = elem(e_n_s,NULL,lnum,GRAPHEME);
		  if (!lastC)
		    lastC = wp;
		  if (lastC && lastC->etype == e_n_word_group && group_flag != notoken)
		    {
		      struct node *gw = elem(e_n_grouped_word,NULL,lastC->lnum,WORD);
		      appendChild(seg,textNode(datap));
		      appendChild(gw,seg);
		      appendChild(lastC, gw);
		    }
		  else
		    {
		      seg = elem(e_n_s,NULL,lnum,GRAPHEME);
		      appendChild(seg,textNode(datap));
		      appendChild(wp,seg);
		    }
		  normnum(seg, datap);
		  last_g = seg;
		}

	      maybeAppendXid(last_g,grapheme_id());

	      if (breakStart && *breakStart == 0x01)
		{
		  breakStart = getAttr(last_g,"xml:id");
		  setAttr(last_g,a_g_breakStart,(unsigned char *)"1");
		}
	      if (statusStart && *statusStart == 0x01)
		{
		  statusStart = getAttr(last_g,"xml:id");
		  setAttr(last_g,a_g_statusStart,(unsigned char *)"1");
		}

	      if ((end-start>1) && tokens[start+1]->type == flag)
		{
		  struct flags *fp = (struct flags *)tokens[++start]->data;
		  set_flags(last_g,fp);
		  if (in_hash && !fp->h)
		    {
		      appendAttr(prev_g?prev_g:np,attr(a_g_hc,ucc("1")));
		      in_hash = 0;
		    }
		  /* no g_corr processing in normalized mode */
		}
	      else
		{
		  if (in_hash)
		    {
		      appendAttr(prev_g,attr(a_g_hc,ucc("1")));
		      in_hash = 0;
		    }
		}
	      if (*cued_opener)
		{
		  appendAttr(last_g,attr(a_g_o,ucc(cued_opener)));
		  *cued_opener = '\0';
		}
	      prev_g = last_g;
	      break;
#if 0
	    case morph:
	      break;
#endif
	    default:
	      if (datap)
		{
		  if (((struct grapheme*)datap)->xml)
		    {
		      tokens[start]->grapheme = ((struct grapheme*)datap)->xml;
		    }
		  else
		    {
		      ++start;
		      if (tokens[start]->type == flag
			  || tokens[start]->type == prox)
			{
			  ++start;
			  continue;
			}
		    }
		}
	      punct_word = tp->type == g_p;
	      if (datap && ((struct grapheme*)datap)->xml)
		np = cloneNode(((struct grapheme*)datap)->xml);
	      else
		{
		  np = elem(e_g_x,NULL,lnum,GRAPHEME);
		  if (tp->type == wm_absent)
		    setAttr(np,a_g_type,(unsigned char*)"word-absent");
		  else if (tp->type == wm_broken)
		    setAttr(np,a_g_type,(unsigned char*)"word-broken");
		  else if (tp->type == wm_linecont)
		    setAttr(np,a_g_type,(unsigned char *)"word-linecont");
		  else if (tp->type == wm_linefrom)
		    setAttr(np,a_g_type,(unsigned char *)"word-linefrom");
		}
	      maybeAppendXid(np,grapheme_id());
	      if (breakStart && *breakStart == 0x01)
		{
		  breakStart = getAttr(np,"xml:id");
		  setAttr(np,a_g_breakStart,(unsigned char *)"1");
		}
	      if (statusStart && *statusStart == 0x01)
		{
		  statusStart = getAttr(np,"xml:id");
		  setAttr(np,a_g_statusStart,(unsigned char *)"1");
		}
	      if (in_g_surro && !surroStart)
		{
		  surroStart = getAttr(np,"xml:id");
		  /* setAttr(surro_node,a_g_surroStart,(unsigned char *)"1"); */
		}
	      np->lnum = lnum;
	      if (!watch)
		watch = np;
	      if (!np)
		{
		  ++start;
		  continue;
		}
	      if (pending_varo)
		{
		  appendAttr(np, attr(pending_varo == 1 ? a_g_varo : a_g_vari,
				      (unsigned char *)(tokens[varo_tok]->data)));
		  pending_varo = 0;
		}
	      if (*cued_opener)
		{
		  appendAttr(np,attr(a_g_o,ucc(cued_opener)));
		  *cued_opener = '\0';
		}
	      if ((end-start>1) && tokens[start+1]->type == flag)
		{
		  struct flags *fp = (struct flags *)tokens[++start]->data;
		  set_flags(np,fp);
		  if (in_hash && !fp->h)
		    {
		      appendAttr(prev_g?prev_g:np,attr(a_g_hc,ucc("1")));
		      in_hash = 0;
		    }
		  while ((end-start>1) && tokens[start+1]->type == g_corr)
		    {
		      struct node *gp = elem(e_g_gg,NULL,lnum,GRAPHEME);
		      struct node *cp;
		      appendAttr(gp,attr(a_g_type,ucc("correction")));
		      appendChild(gp,np);
		      datap = tokens[++start]->data;
		      if (!datap || !((struct grapheme*)datap)->xml)
			{
			  ++start;
			  continue;
			}
		      cp = cloneNode(((struct grapheme*)datap)->xml);
		      datap = tokens[start-2]->data;
		      cp->lnum = lnum;
		      if ((end-start>1) && tokens[start+1]->type == flag)
			{
			  /* This can only be a '?' flag */
			  struct flags *fp = (struct flags *)tokens[++start]->data;
			  set_flags(cp,fp);
			}
		      appendChild(gp,cp);
		      if (tokens[start+1] && tokens[start+1]->type == hyphen)
			{
			  np = gp;
			  /*++start;*/
			}
		      else
			{
			  np = gp;
			  break;
			}
		    }
		  if ((end-start>1) && tokens[start+1]->type == uflag)
		    {
		      struct uflags *ufp = (struct uflags *)tokens[++start]->data;
#if 0
		      if (in_hash)
			{
			  appendAttr(prev_g ? prev_g : np,attr(a_g_hc,ucc("1")));
			  in_hash = 0;			  
			}
#endif		      
		      set_uflags(np,ufp);
		      
		    }
		}
	      else if ((end-start>1) && tokens[start+1]->type == uflag)
		{
		  struct uflags *ufp = (struct uflags *)tokens[++start]->data;
		  if (in_hash)
		    {
		      appendAttr(prev_g ? prev_g : np,attr(a_g_hc,ucc("1")));
		      in_hash = 0;
		    }
		  set_uflags(np,ufp);
		}
	      else
		{
		  if (in_hash)
		    {
		      appendAttr(prev_g ? prev_g : np,attr(a_g_hc,ucc("1")));
		      in_hash = 0;
		    }
		}
	      
	      if ((end-start)>1 && tokens[start+1]->type == prox)
		appendAttr(np,attr(a_g_prox,ucc(tokens[++start]->data)));
	      if (mutex_state != notoken)
		appendAttr(np,attr(fixed_attr_n[mutex_state],
				   fixed_attr_v[mutex_state]));
	      /*#if 0*/
	      else /* use implo just to get name structure of g:status easily */
		appendAttr(np,attr(fixed_attr_n[implo],ucc("ok")));
	      /*#endif*/
	      if (square_bracket == damago)
		appendAttr(np,attr(fixed_attr_n[damago],fixed_attr_v[damago]));
	      if (datap)
		{
		  struct node *target = np;
		  if (np->etype == e_g_gg && np->children.lastused > 0)
		    target = firstChild(np);
		  if (surro_mark_grapheme)
		    {
		      surro_mark_grapheme = 0; /* appendChild(surro_node, removeLastChild(wp)); */
		    }
		  else if (sforce_flag)
		    {
		      enum t_type gt = ((struct grapheme*)datap)->type;
		      if (gt == g_s || gt == g_c)
			appendAttr(target,attr(fixed_attr_n[sforce],fixed_attr_v[sforce]));
		      else
			warning("$ only allowed before uppercased sign");
		      sforce_flag = 0;
		    }
		  else if (lforce_flag || tp->lang->core->uppercase == m_logo)
		    {
		      enum t_type gt = ((struct grapheme*)datap)->type;
		      if (long_logo || (gt == g_s || gt == g_c))
			{
			  struct node *lastc = lastChild(wp);
			  struct node *anc_gg = NULL;
			  if (lastc == NULL && wp->etype != e_g_d)
			    lastc = wp;
			  appendAttr(target,attr(fixed_attr_n[lforce],fixed_attr_v[lforce]));
			  appendAttr(target,attr(a_g_logolang,
						 ucc(tp->altlang)
						 ? ucc(tp->altlang)
						 : ucc(tp->lang->core->altlang)));
			  if (word_tokp && word_tokp->lang 
			      && (word_tokp->lang->core->features & LF_LOGO)
			      && !hacked_word_lang)
			    {
			      if (logo_word_lang(wp,tp))
				++hacked_word_lang;
			    }
			  ++logo_word;
			  if (atpt || lastc)
			    anc_gg = atpt_ancestor_or_self_gg(atpt ? atpt : lastc);
			  else if (lastc)
			    anc_gg = atpt_ancestor_gg(lastc);
			  if (anc_gg)
			    {
			      group_flag = anc_gg->ttype;
			      if (atpt_no_logo(anc_gg))
				{
				  const char *atype = (const char *)getAttr(anc_gg,"g:type");
				  if (!xstrcmp(atype,"reordering")
				      || !xstrcmp(atype,"alternation")
				      || !xstrcmp(atype,"ligature")
				      || !xstrcmp(atype,"correction")
				      )
				    {
				      struct node *n = elem(e_g_gg,NULL,lnum,GRAPHEME);
				      /* struct node *atpt_parent =
					 (atpt ? atpt->parent :
					 lastc); */
				      setAttr(n,a_g_type,ucc("logo"));
#if 1
				      /* np is the new grapheme which has triggered the
					 logo, but we aren't under a logo group. */
				      appendChild(n, removeLastChild(wp));
				      if (grouped_det)
					appendChild(n, np);
				      else
					if (atpt)
					  appendChild(atpt, np);
					else
					  appendChild(n, np);
				      appendChild(wp,n);
				      np_already_set = 1;
#else
				      appendChild(n, removeLastChild(atpt->parent));
				      appendChild(wp,n);
#endif
				      atpt = n;
				      group_flag = atpt->ttype = period;
			            }
				  else if (!xstrcmp(atype,"group"))
				    {
				      struct node *n = elem(e_g_gg,NULL,lnum,GRAPHEME);
				      setAttr(n,a_g_type,ucc("logo"));
				      appendChild(n, removeLastChild(atpt ? atpt : wp));
				      appendChild(atpt, n);
				      atpt = n;
				      group_flag = period;
				    }
				  else
				    {
				      fprintf(stderr, "unhandled group type in logo processing: %s\n", atype);
				    }
				}
			      else
				{
#if 0
				  /* There is a logo ancestor, just attach graphemes */
				  if (!atpt && anc_gg)
				    {
				      atpt = anc_gg;
				      group_flag = atpt->ttype = (tp->type == g_s ? period : tp->type);
				    }
#else
				  group_flag = period;
#endif
				}
			    }
			  else
			    {
			      struct node *n = elem(e_g_gg,NULL,lnum,GRAPHEME);
			      setAttr(n,a_g_type,ucc("logo"));
			      if (lastc && lastc->etype == e_g_d
				  && !xstrcmp(getAttr(lastc,"g:delim"),""))
				{
				  struct node *lastc2 = NULL;
				  removeLastChild(wp);
				  lastc2 = lastChild(wp);
				  /* check for {m}{d} */
				  if (lastc2 && lastc2->etype == e_g_d
				      && !xstrcmp(getAttr(lastc,"g:delim"),""))
				    {
				      removeLastChild(wp);
				      appendChild(n,lastc2);
				    }
				  appendChild(n,lastc);				  
				}
			      appendChild(wp,n);
			      /*group_node = n;*/
			      /*list_push(group_stack, n);*/
			      /* MORE WORK HERE? */
			      atpt = n;
			      group_flag = atpt->ttype = np->ttype = period;
			    }
			}
		      else if (lforce_flag)
			warning("~ only allowed before uppercased sign");
		      if (!long_logo)
			lforce_flag = 0;
		    }
		}
	      if (!np_already_set)
		{
		  if (surro_node && surro_node->parent == wp)
		    appendChild(surro_node, np); /* {MIN<(...)>} */
		  else if (grouped_det && !strcmp(wp->names->pname,"g:d"))
		    appendChild(wp, np); /* we have reset wp to e_g_d node when grouped_det */
		  else if (group_flag == notoken)
		    appendChild(atpt ? atpt : wp, np);
		  else
		    {
		      if (!atpt)
			warning("ox: internal error: attach point not set in group context");
		      appendChild(atpt ? atpt : lastChild(wp), np);		  
		    }
		}
	      else
		np_already_set = 0;
	      prev_g = last_g = np;
	      break;
	    }
	  break;
	case bound:
	  if (tp->type != space)
	    {
	    }
	  switch (tp->type)
	    {
	    case space:
	      if (wp)
		{
		  if (in_g_surro && surro_node)
		    {
		      wp = surro_wp;
			/*surro_node->parent;*/
		      surro_atpt = surro_wp = surro_node = NULL;
		    }
		  if (wp->etype == e_g_gg)
		    {
		      while (wp->etype == e_g_gg)
			wp = wp->parent;
		    }
		  wrapup_word(wp, space);
		  setAttr(wp,a_g_delim,tp->data);
		}
	      else if (last_wp)
		{
		  struct token *prev = tokens[start-1];
		  if (prev->class != meta 
		      || (prev->type != ub_plus && prev->type != ub_minus))
		    setAttr(last_wp,a_g_delim,tp->data);
		  last_wp = NULL;
		}
	      last_g = wp = NULL;
	      group_flag = notoken;
	      /*group_node = NULL;*/
	      atpt = NULL;
#if 0
	      if (((char*)tp->data)[1])
		setAttr(lastChild(wp),a_g_em,ucc("1"));
#endif
	      break;
	    case hyphen:
	      if (atpt)
		{
		  if (tp->data)
		    {
		      struct node *hyphme = lastGrapheme(atpt)/*lastChild(atpt)*/;
		      if (!hyphme)
			hyphme = atpt;
		      setAttr(hyphme,a_g_delim,tp->data);
		      if (((char*)tp->data)[1])
			setAttr(hyphme,a_g_em,ucc("1"));
		    }
		}
	      else if (wp)
		{
		  struct node *hyphme = lastGrapheme(wp)/*lastChild(wp)*/;
		  if (!hyphme)
		    hyphme = wp;
		  setAttr(hyphme,a_g_delim,tp->data);
		  if (((char*)tp->data)[1])
		    setAttr(hyphme,a_g_em,ucc("1"));
		}
	      else if (last_wp)
		{
		  struct node *hyphme = lastGrapheme(last_wp)/*lastChild(last_wp)*/;
		  if (!hyphme)
		    hyphme = last_wp;
		  setAttr(hyphme,a_g_delim,tp->data);
		  if (((char*)tp->data)[1])
		    setAttr(hyphme,a_g_em,ucc("1"));
		  last_wp = NULL;
		}
	      else if (gdl_fragment_ok)
		{
		  wp = init_word_func(parent, tp, &pending_varo, varo_tok, 
				      &logo_word, with_word_list);
		  np = elem(e_g_x,NULL,lnum,GRAPHEME);
		  appendAttr(np,attr(a_g_type,ucc("empty")));
		  appendChild(wp ? wp : parent,np);
		  setAttr(lastChild(wp),a_g_delim,tp->data);
		  if (((char*)tp->data)[1])
		    setAttr(lastChild(wp),a_g_em,ucc("1"));
		}
	      if (atpt && !grouped_det)
		{
		  if (atpt->parent && atpt->parent->etype == e_g_gg)
		    {
		      atpt = atpt->parent;
		      group_flag = atpt->ttype;
		    }
		  else
		    {
		      struct token *next_g = next_text(start);
		      if (!tokens[start+1]
			  || group_flag == plus
			  || group_flag == slash
			  || (group_flag == period
			      && next_g
			      && next_g->type != g_s
			      && next_g->type != g_c))
			{
			  atpt = NULL;
			  group_flag = notoken;
			}
		    }
		}
	      break;
	    case ilig:
	    case colon:
	    case period:
	    case plus:
	      no_norm(type_data[tp->type]);
	    case slash:
	      if (group_flag == notoken)
		{
		  struct node *hyphme = lastChild(wp);
		  if (!hyphme)
		    {
		      hyphme = wp;
		      setAttr(hyphme,a_g_delim,tp->data);
		    }
		  else if (hyphme->etype != e_n_s)
		    setAttr(hyphme,a_g_delim,tp->data);
		}
	      else
		{
		  if (tp->data)
		    setAttr(last_g ? last_g
			    : lastChild(atpt?atpt:lastChild(wp)),a_g_delim,tp->data);
		  else
		    setAttr(last_g ? last_g
			    : lastChild(atpt?atpt:lastChild(wp)),a_g_delim,(unsigned char *)"");
		}
	      if (wp)
		{
		  if (group_flag != tp->type)
		    {
		      struct node *g = elem(e_g_gg,NULL,lnum,GRAPHEME);
		      unsigned const char *tstr = NULL;
		      switch (tp->type)
			{
			case colon:
			  tstr = ucc("reordering");
			  break;
			case period:
			  if (lforce_flag || tp->lang->core->uppercase == m_logo)
			    tstr = ucc("logo");
			  else
			    tstr = ucc("group");
			  break;
			case ilig:
			  tstr = ucc("implicit-ligature");
			  break;
			case plus:
			  tstr = ucc("ligature");
			  break;
			case slash:
			  if (wp && wp->etype == e_n_w)
			    setName(g, e_n_word_group);
			  tstr = ucc("alternation");
			  break;
			default:
			  break;
			}
		      if (g)
			{
			  appendAttr(g,attr(a_g_type,tstr));
			  if (g->etype == e_g_gg)
			    {
#if 1
			      if (atpt)
				{
				  appendChild(g,removeLastChild(atpt));
				  appendChild(atpt,g);
				  atpt = g;
				}
#else
			      if (group_node)
				{
				  appendChild(g,removeLastChild(group_node));
				  appendChild(group_node,g);
				  group_node = g;
				}
#endif
			      else
				{
				  appendChild(g,removeLastChild(wp));
				  appendChild(wp,g);
				  /*group_node = g;*/
				  atpt = g;
				}
			    }
			  else
			    {
			      struct node *last = removeLastChild(wp);
			      struct node *gw = elem(e_n_grouped_word,NULL,last->lnum,WORD);
			      appendChild(gw,last);
			      appendChild(g,gw);
			      appendChild(wp,g);
			      /*group_node = g;*/
			      /*atpt = g;*//* need to figure out whether to set atpt in norm context */
			    }
			}
		      group_flag = tp->type;
		      if (atpt)
			atpt->ttype = (tp->type == g_s ? period : tp->type);
		      /*group_flag = tp->type;*/
		    }
		}
	      else
		{
		  warning("misplaced '.', '/', '+' or ':'");
		}
	      break;
	    default:
	      break;
	    }
	  break;
	case meta:
	  switch (tp->type)
	    {
	    case deto:
	      /*no_norm(type_data[tp->type]);*/
	      if (!wp)
		wp = init_word_func(parent, tp, &pending_varo, varo_tok, 
				    &logo_word, with_word_list);
	      np = elem(e_g_d,NULL,lnum,GRAPHEME);
	      if (start && tokens[start-1]->type == deto)
		b_or_g = hyphen;
	      else
		b_or_g = prev_b_or_g(start);
	      if (start+1 < end && tokens[start+1]->type == plus)
		{
		  ++start;
		  appendAttr(np,attr(a_g_role,ucc("phonetic")));
		}
	      else
		appendAttr(np,attr(a_g_role,ucc("semantic")));
#if 0
	      /* Leave the cued_opener for the grapheme-child of determinatives.*/
	      if (*cued_opener && tokens[start]->type != deto)
		{
		  appendAttr(np,attr(a_g_o,ucc(cued_opener)));
		  *cued_opener = '\0';
		}
#endif
	      /* ->user is utilized for label storage as well as b_or_g 
		 tracking; need to make sure that this doesn't conflict 
		 (which it won't as long as np is not a block element) */
	      if (np->user > (void*)1)
		{
		  fprintf(stderr,"np->user already set\n");
		  exit(1);
		}
	      np->user = (void*)b_or_g;
	      if (split_flag)
		{
		  long int tmp = (uintptr_t)np->user;
		  tmp |= SPLIT_BIT;
		  np->user = (void*)tmp;
		}
	      if (group_flag == notoken)
		{
		  wp = appendChild(wp,np);
		  grouped_det = 0;
		}
	      else
		{
		  wp = appendChild(atpt ? atpt : lastChild(wp),np);
		  grouped_det = 1;
		}
	      break;
	    case detc:
	      /*no_norm(type_data[tp->type]);*/
	      if (wp && !strcmp(wp->names->pname,"g:d"))
		{
		  split_flag = 0;
		  b_or_g = next_b_or_g(start);
		  if (b_or_g == noop)
		    {
		      if (((enum t_class)wp->user&(~SPLIT_BIT)) == noop)
			{
			  if (split_flag)
			    {
			      appendAttr(wp,attr(a_g_pos,ucc("post")));
			      split_flag = 0;
			    }
			  else
			    warning("determinative beside text at both ends");
			}
		      else
			appendAttr(wp,attr(a_g_pos,ucc("pre")));
		    }
		  else
		    {
		      if (((enum t_class)wp->user&(~SPLIT_BIT)) != noop)
			{
			  if (split_flag)
			    {
			      appendAttr(wp,attr(a_g_pos,ucc("pre")));
			      split_flag = 0;			      
			    }
			  else if (((uintptr_t)wp->user)&SPLIT_BIT)
			    appendAttr(wp,attr(a_g_pos,ucc("post")));
			  else if (colon == next_b_or_g(start))
			    appendAttr(wp,attr(a_g_pos,ucc("pre")));
			  else if (colon == prev_b_or_g(start))
			    appendAttr(wp,attr(a_g_pos,ucc("post")));
			  else
			    warning("determinative beside text at neither end");
			}
		      else
			appendAttr(wp,attr(a_g_pos,ucc("post")));
		    }
		  wp->user = NULL;
		  wp = wp->parent;
#if 1
		  if (grouped_det)
		    {
		      wp = wp->parent;
		      grouped_det = 0;
		      /* atpt = NULL; ??? */
		    }
		  else if (group_flag != notoken)
		    {
		      /*wp = wp->parent;*/
		      group_flag = notoken;
		      /*group_node = NULL;*/
		      /*atpt = atpt->parent;*/
		      atpt = NULL;
		    }
#else
		  if (group_flag != notoken)
		    wp = wp->parent;
#endif
		}
	      else
		warning("mismatched {...}");
	      break;
	    case glosso:
	    case smetao:
	      no_norm(type_data[tp->type]);
	      np = elem(e_g_gloss,NULL,lnum,GRAPHEME);
	      appendAttr(np,attr(a_g_type, ucc(((tp->type == glosso)
					      ? "lang" : "text"))));

#if 0	      
	      if (tp->type == glosso)
		appendAttr(np,attr(a_g_pos,ucc("post"))); /* FIXME!!*/
	      else
		appendAttr(np,attr(a_g_pos,ucc("free"))); /* FIXME!!*/
#endif
	      if (atpt || group_flag)
		{
		  atpt = NULL;
		  group_flag = notoken;
		}
	      if (wp)
		{
		  const char *pos = "post";
		  if (start)
		    {
		      struct token *tp = tokens[start-1];
		      if (tp->class == bound)
			{
			  pos = "pre";
			}
		      else
			{
			  struct node *cp = (struct node*)(lastChild(wp));
			  if (cp && *cp->type == 'e' && !strcmp(cp->names->pname, "g:d"))
			    pos = (const char *)getAttr(cp,"g:pos");
			}
		    }
#if 0
		  for (i = wp->children.lastused-1; i >= 0; --i)
		    {
		      struct node *cp = (struct node*)(wp->children.nodes[i]);
		      const char *pname = cp->names->pname;
		      if (*np->type == 'e' && !strcmp(pname, "g:d"))
			break;
		      else if (*np->type == 'e' && (!strcmp(pname, "g:x")))
			{
			  pos = "post";
			  break;
			}
		    }
#endif
		  appendAttr(np,attr(a_g_pos,ucc(pos)));
		  parent = appendChild(wp,np);
		}
	      else
		{
		  appendAttr(np,attr(a_g_pos,ucc("pre")));
		  parent = appendChild(parent,np);
		}
	      wp = NULL;
	      break;
	    case glossc:
	    case smetac:
	      no_norm(type_data[tp->type]);
	      if (wp)
		{
		  wrapup_word(wp, tp->type);
		  if (!xstrcmp(wp->parent->names->pname,"g:gloss"))
		    {
		      const unsigned char *pos = getAttr(wp->parent, "g:pos");
		      if (!strcmp((const char *)pos,"post"))
			{
			  int i = start+1;
			  struct token *nexttp = tokens[i];
			  while (nexttp && (nexttp->type == damago
					   || nexttp->type == damagc
					   ))
			    nexttp = tokens[++i];
			  if (nexttp && nexttp->class != bound
			      && nexttp->type != deto
			      && nexttp->type != cell
			      && nexttp->type != field
			      && nexttp->type != surrc)
			    warning("{{...}} beside text at both ends");
			}
		      if (!xstrcmp(getAttr(wp->parent,"g:type"),
				   ucc(((tp->type == glossc) 
					? "lang" : "text"))))
			{
			  wp = wp->parent->parent;
			  if (xstrcmp(wp->names->pname,"g:w"))
			    {
			      parent = wp;
			      wp = NULL;
			    }
			  else
			    parent = wp->parent;
			}
		      else
			warning("{{ not closed by }}");
		    }
		  else
		    warning("unexpected }}");
		}
	      else
		{
		  if (parent)
		    {
		      parent = parent->parent;
		      if (!xstrcmp(parent->names->pname,"g:w"))
			{
			  wp = parent;
			  parent = wp->parent;
			}
		    }
		}
	      break;
	    case surro_mark:
	      if (tokens[start+1]
		  && (tokens[start+1]->type == g_v
		      || tokens[start+1]->type == g_q))
		{
		  warning("bad surrogate grapheme before <(...)>");
		}
#if 0
	      if (long_surro(start))
		{
		  group_flag = notoken;
		  np = elem(e_surro,NULL,lnum,GRAPHEME);
		  in_long_surro = 1;
		  in_g_surro = 0;
		  /* if we're in the middle of a word make the word the 
		     initial child of surro */
		  if (wp)
		    appendChild(np, removeLastChild(parent));
		  appendChild(parent, np); /* attach to line node */
		  surro_node = parent = np;
		}
	      else
#endif		
		{
		  if (!wp)
		    {
		      struct token *maybe_norm = surro_norm(start);
		      wp = init_word_func(parent, maybe_norm ? maybe_norm : tp, &pending_varo, varo_tok, 
					  &logo_word, with_word_list);
		    }
		  surroStart = NULL;
		  in_long_surro = long_surro(start);
		  in_g_surro = 1;
		  /* in_long_surro = 0; */
		  np = elem(e_g_surro,NULL,lnum,GRAPHEME);
		  surro_atpt = atpt;
		  if (grouped_det)
		    atpt = surro_node = appendChild(wp, np);
		  else
		    atpt = surro_node = appendChild(atpt ? atpt : wp, np);
		}
	      surro_mark_grapheme = 1;
	      break;	      
	    case surro:
	      if (in_g_surro)
		{
		  struct node *gg = elem(e_g_gg,NULL,lnum,GRAPHEME);
		  group_flag = gg->ttype = surro;
		  surro_wp = wp;
		  appendAttr(gg,attr(a_g_type,ucc("group")));
		  atpt = wp = appendChild(surro_node,gg);
		}
	      else
		{
		  if (surro_node->children.lastused)
		    {
		      struct node *f = firstChild(surro_node);
		      if (f->children.lastused == 1)
			{
			  setName(f,e_g_nonw);
			  set_nonw_id(f);
			  appendAttr(f,attr(a_type,ucc("surro")));
			}
		      else
			{
			  
			}
		    }
                  if (need_lemm || do_show_insts)
                    (*lemm_unform_p)();
                  removeAttr(wp,"form");
                  /* back up the word list also b/c we consider the surro
                     host to be a non-word */
                  if (nline_words > 0 && (!sparse_lem || w_sparse_lem))
                    --nline_words;
                  wp = NULL;
		  /*		  wp = init_word_func(parent, tp, &pending_varo, varo_tok, 
				  &logo_word, with_word_list); */
		}
	      break;
	    case surrc:
	      if (NULL == surro_node)
		{
		  if (surroStart)
		    {
		      setAttr(lastGrapheme(wp),a_g_surroEnd,surroStart);
		      surroStart = NULL;
		    }
		  else
		    {
		      fprintf(stderr,"internal error: found surrc when surroStart==NULL\n");
		    }
		}
	      else
		{
		  if (in_g_surro)
		    {
		      if (surro_node)
			{
			  if (atpt)
			    setAttr(lastChild(atpt),a_g_surroEnd,surroStart);
			  else
			    setAttr(lastChild(lastChild(surro_node)),a_g_surroEnd,surroStart);
			  wp = surro_wp;
			  /*surro_node->parent;*/
			  atpt = surro_atpt;
			  surro_atpt = surro_wp = NULL;
			}
		      in_g_surro = 0;
		    }
		  else
		    {
		      if (start+1 < end && tokens[start+1]->type == hyphen)
			in_split_word = 1;
		      parent = surro_node->parent;
		      wrapup_word(wp,tp->type);
		      atpt = surro_atpt;
		      if (!xstrcmp(parent->names->pname,"g:w")
			  || !xstrcmp(parent->names->pname,"n:w"))
			{
			  wp = parent;
			  parent = wp->parent;
			  /* atpt = NULL; */
			}
		      else
			{
			  last_wp = wp;
			  wp = NULL;
			}
		    }
		}
	      if (atpt && atpt->etype == e_g_gg)
		group_flag = atpt->ttype;
	      else
		group_flag = notoken;
	      surro_node = NULL;
	      break;
	    case damago:
	      if (square_bracket == damago)
		warning("mismatched square brackets: no closer");
	      else
		{
		  square_bracket = damago;
		  cue_opener("[");
		  breakStart = (const unsigned char *)"\x1";
		}
	      break;
	    case damagc:
	      if (square_bracket == damago)
		{
		  square_bracket = notoken;
		  appendCloser(last_g,"]");
		  /* FIXME: I hacked this to make 1. [x ($ blank space$)] type stuff parse
		     but it probably doesn't behave right with [($blank space$)] so that 
		     needs checking */
		  if (last_g && breakStart && *breakStart && breakStart[0] != 0x01)
		    setAttr(last_g,a_g_breakEnd,breakStart);
		  breakStart = NULL;
		}
	      else
		warning("mismatched square brackets: no opener");
	      break;
#define set_mutex(tostate,expecting,trigger)\
	      if (mutex_state == expecting) \
	        mutex_state = tostate; \
	      else \
  	        vwarning("bracket mismatch: found %s while in %s",\
		         type_data[trigger],type_data[mutex_state])
	    case someo:
	      set_mutex(someo,notoken,someo);
	      cue_opener("((");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case somec:
	      set_mutex(notoken,someo,somec);
	      appendCloser(last_g,"))");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case implo:
	      set_mutex(implo,notoken,implo);
	      cue_opener("<(");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case implc:
	      set_mutex(notoken,implo,implc);
	      appendCloser(last_g,")>");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case supplo:
	      set_mutex(supplo,notoken,supplo);
	      cue_opener("<");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case supplc:
	      set_mutex(notoken,supplo,supplc);
	      appendCloser(last_g,">");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case exciso:
	      set_mutex(exciso,notoken,exciso);
	      cue_opener("<<");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case excisc:
	      set_mutex(notoken,exciso,excisc);
	      appendCloser(last_g,">>");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case eraso:
	      set_mutex(eraso,notoken,eraso);
	      cue_opener("<$");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case erasc:
	      set_mutex(notoken,eraso,erasc);
	      appendCloser(last_g,"$>");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case maybeo:
	      set_mutex(maybeo,notoken,maybeo);
	      cue_opener("(");
	      statusStart = (unsigned char *)"\x1";
	      break;
	    case maybec:
	      set_mutex(notoken,maybeo,maybec);
	      appendCloser(last_g,")");
	      setAttr(last_g,a_g_statusEnd,statusStart);
	      statusStart = NULL;
	      break;
	    case linebreak:
	      np = elem(e_g_x,NULL,lnum,GRAPHEME);
	      appendAttr(np,attr(a_g_type,ucc("linebreak")));
	      appendChild(wp ? wp : parent,np);
	      break;
	    case newline:
	      /* this is the least intrusive place to detect split words because
		 ; is rare anyway so this code will not be executed unnecesarily 
		 very often */
	      if (start && tokens[start-1]->type == hyphen
		  && (start+1 == end))/* this needs adjusting for ba-; & -ab */
		{
		  if (!in_split_word)
		    in_split_word = 1;
		  split_word_unfinished = 1;
		}
	      else
		{
		  last_g = np = elem(e_g_x,NULL,lnum,GRAPHEME);
		  appendAttr(np,attr(a_g_type,ucc("newline")));
		  if ((end-start>1) && tokens[start+1]->type == flag) /* add uflag */
		    {
		      struct flags *fp = (struct flags *)tokens[++start]->data;
		      set_flags(np,fp);
		      if (in_hash && !fp->h)
			{
			  appendAttr(prev_g?prev_g:np,attr(a_g_hc,ucc("1")));
			  in_hash = 0;
			}
		    }
		  if (atpt)
		    appendChild(atpt,np);
		  else
		    appendChild(wp ? wp : parent,np);
		}
	      break;
	    case ellipsis:
	      if (!wp)
		wp = init_word_func(parent, tp, &pending_varo, varo_tok, 
				    &logo_word, with_word_list);
	      if (in_hash)
		{
		  appendAttr(prev_g,attr(a_g_hc,ucc("1")));
		  in_hash = 0;
		}
	      np = elem(e_g_x,NULL,lnum,GRAPHEME);
	      appendAttr(np,attr(a_g_type,ucc("ellipsis")));
	      maybeAppendXid(np,grapheme_id());
	      if (breakStart && *breakStart == 0x01)
		{
		  breakStart = getAttr(np,"xml:id");
		  setAttr(np,a_g_breakStart,(unsigned char *)"1");
		}
	      if (statusStart && *statusStart == 0x01)
		{
		  statusStart = getAttr(np,"xml:id");
		  setAttr(np,a_g_statusStart,(unsigned char *)"1");
		}

	      if (mutex_state != notoken)
		appendAttr(np,attr(fixed_attr_n[mutex_state],
				   fixed_attr_v[mutex_state]));
	      else /* use implo just to get name structure of g:status easily */
		appendAttr(np,attr(fixed_attr_n[implo],ucc("ok")));
	      if (square_bracket == damago)
		appendAttr(np,attr(fixed_attr_n[damago],fixed_attr_v[damago]));
	      if (*cued_opener)
		{
		  appendAttr(np,attr(a_g_o,ucc(cued_opener)));
		  *cued_opener = '\0';
		}
	      prev_g = last_g = np;
	      appendChild(np,textNode(ucc("...")));
	      if (grouped_det)
		atpt = surro_node = appendChild(wp, np);
	      else
		appendChild(atpt ? atpt: wp,np);
	      break;
	    case sforce:
	      if (lforce_flag)
		warning("$ ignored after ~");
	      else
		sforce_flag = 1;
	      break;
	    case lforce:
	      if (sforce_flag)
		warning("~ ignored after $");
	      else
		logoline = lforce_flag = 1;
	      break;
	    case ftype:
	      break;
	    case flag:
	      /* in meta belonging to corr allow ? */
	      warning("misplaced flag");
	      break;
	    case uflag:
	      if (tokens[start-1]->type != flag)
		warning("misplaced uflag");
	      break;
	    case prox:
	      warning("misplaced prox");
	      break;
	    case icmt:	      
	      if (wp)
		{
		  char type = *(char*)tp->data;
		  last_g = np = elem(e_g_x,NULL,lnum,GRAPHEME);
		  appendAttr(np,attr(a_g_type,
				     ucc((type == '#' ? "comment" : "dollar"))));
		}
	      else
		{
		  char type = *(char*)tp->data;
		  last_g = np = elem(e_g_nonw,NULL,lnum,WORD);
		  appendAttr(np,attr(a_type,
				     ucc((type == '#' ? "comment" : "dollar"))));
		}
	      sprintf(word_id_insertp, "%d", word_id++);
	      maybeAppendXid(np,word_id_buf);
	      set_nonw_id(np);
	      if (*cued_opener)
		{
		  appendAttr(np,attr(a_g_o,ucc(cued_opener)));
		  *cued_opener = '\0';
		}
	      if ((http = strstr((char*)tp->data, "http")))
		nonx_link_data(nonx_link(((unsigned char*)(tp->data))+1, http), np);
	      else
		appendChild(np,textNode(((unsigned char*)(tp->data))+1));
	      if (!strcmp((((char*)(tp->data))+1), "DUMMY"))
		setAttr(parent,a_silent,(unsigned char *)"1");
	      if (atpt /*group_node*/)
		appendChild(atpt /*group_node*/,np);
	      else if (wp)
		appendChild(wp,np);
	      else
		appendChild(parent,np);
	      break;
	    case varo:
	      varo_tok = start;
	      if (start < end && tokens[start+1]->type == varc)
		{
		  pending_varo = 2;
		  ++start;
		}
	      else
		pending_varo = 1;
	      break;
	    case varc:
	      appendAttr(last_g, attr(a_g_varc,(unsigned char *)(tp->data)));
	      break;
	    case ub_plus:
	    case ub_minus:
	      if (last_word)
		set_ub(last_word,a_syn_ub_after,tp);
	      else
		pending_ub = tp;
	      break;
	    case normo:
	      /*FIXME: catch double normo on one word*/
	      if (logo_word)
		{
		  if (wp)
		    appendAttr(wp,attr(a_norm, tokens[start+1]->data));
		  else
		    warning("invalid word in normalization");
		}
	      else
		{
		  vwarning("no logogram in normalized word '%s' (current language = %c%s)",
			   tokens[start+1]->data,
			   '%', tp->lang->fulltag);
		}
	      start += 3;
	      continue;
	      break;
	    case notemark:
	      if (wp)
		{
		  struct node *np = elem(e_g_nonw,NULL,lnum,WORD);
		  appendAttr(np,attr(a_type, ucc("notelink")));
		  set_nonw_id(np);
		  appendChild(np, textNode(pool_copy(tokens[start]->data)));
		  note_register_tag(tokens[start]->data, np);
		  appendChild(wp->parent, np);
		}
	      else
		{
		  warning("misplaced note tag");
		}
	      break;
	    case noop:
	      ++start;
	      break;
	    case g_disamb:
	      {
		struct node *m = gelem(e_g_x,NULL,lnum,GRAPHEME);
		struct node *t = gtextNode(ucc(tp->data));
		appendAttr(m,attr(a_g_type,ucc("disamb")));
		appendChild(m,t);
		pending_disamb = m;
	      }
	      break;
	    default:
	      vwarning("unhandled token type %s", type_names[tp->type]);
	      /*exit(2);*/
	      break;
	    }
	  break;
	default:
	  break;
	}
      ++start;
    }
  if (in_hash)
    {
      appendAttr(prev_g,attr(a_g_hc,ucc("1")));
      in_hash = 0;
    }

  emit_vari();

  if (wp)
    {
      if (in_g_surro && surro_node)
	{
	  wp = surro_wp;
	  /*surro_node->parent;*/
	  surro_atpt = surro_wp = surro_node = NULL;
	}
      if (wp->etype == e_g_gg)
	{
	  while (wp->etype == e_g_gg)
	    wp = wp->parent;
	}
      wrapup_word(wp,eol);
    }
}

static struct node *
next_child_with_status(struct node *parent)
{
  if (parent->children.lastused)
    {
      if (*(getAttr(parent->children.nodes[0], "g:status")))
	return parent->children.nodes[0];
      else
	return next_child_with_status(parent->children.nodes[0]); 
    }
  return NULL;
}

/* word-rendering emits delimiters on the next grapheme, instead of using g:delim: this is probably a bug */
static struct node *
next_node_with_status(struct node *wp, int i)
{
  if (i < wp->children.lastused)
    {
      const unsigned char *s = getAttr(wp->children.nodes[i], "g:status");
      if (*s)
	return wp->children.nodes[i];
      else
	return next_child_with_status(wp->children.nodes[i]);
    }
  else
    return NULL;
}

static void
finish_word(struct node *wp)
{
  unsigned char *form = forms_insertp;
  int i;
  extern int rendering_word_form;

  if (!word_lang)
    word_lang = curr_lang;

  if (pending_disamb)
    {
      appendChild(wp,pending_disamb);
      pending_disamb = NULL;
    }  
  
  rendering_word_form = 1;
  for (suppress_next_hyphen = i = 0; 
       i < wp->children.lastused; 
       ++i)
    {	  
      struct node *cp = wp->children.nodes[i];
      unsigned const char *logolang = ucc("");
      if (cp->attr.lastused)
	{
	  if (nowt_to_render(cp))
	    continue;
	  if (logoline)
	    logolang = getAttr(cp,"g:logolang");
	}
      if (*logolang)
	{
	  if (!curr_logolang)
	    {
	      curr_logolang = logolang;
	      *forms_insertp++ = '%';
	      xstrcpy(forms_insertp,curr_logolang);
	      forms_insertp += xxstrlen(curr_logolang);
	      *forms_insertp++ = ':';
	    }
	}
      else if (curr_logolang)
	{
	  if (forms_insertp > formsbuf && forms_insertp[-1] == '-')
	    {
	      forms_insertp[-1] = forms_insertp[0] = '%';
	      *++forms_insertp = '-';
	      ++forms_insertp;
	    }
	  else
	    {
	      forms_insertp[0] = forms_insertp[1] = '%';
	      forms_insertp += 2;
	    }
	  curr_logolang = NULL;
	}
      if (i)
	{
	  if (suppress_next_hyphen)
	    {
	      struct node *next_np = next_node_with_status(wp, i);
	      if (!next_np || xstrcmp(getAttr(next_np, "g:status"),"excised"))
		suppress_next_hyphen = 0;
	    }
	  else if (word_lang->mode == m_graphemic)
	    {
	      if (forms_insertp[-1] != '-' && forms_insertp[-1] != '.' && forms_insertp[-1] != '{')
		*forms_insertp++ = '-'; /* the renderer needs rewriting
					   to emit g:delim after graph
					   not before next graph, but
					   this is what works for now */
	    }
	  else
	    {
	      const unsigned char *gdelim = getAttr(wp->children.nodes[i-1], "g:delim");
	      if (*gdelim)
		*forms_insertp++ = *gdelim;
	    }
	}
      if (*cp->type == 't')
	forms_insertp = render_g_text(cp, forms_insertp, form);
      else if (*cp->type == 'e' && !strcmp(cp->names->pname, "g:p"))
	{
	  if (forms_insertp > formsbuf && forms_insertp[-1] == '-')
	    *--forms_insertp = '\0';
	}
      else
	forms_insertp = render_g(cp, forms_insertp, form);
      
      if (!forms_insertp)
	{
	  rendering_word_form = 0;
	  return;
	}
    }
  
  rendering_word_form = 0;
  
  if (forms_insertp > formsbuf
      && ((forms_insertp[-1] == '-') || (forms_insertp[-1] == '.')))
    forms_insertp[-1] = '\0';
  else
    *forms_insertp++ = '\0';
  if (forms_insertp >= (formsbuf + formsbuf_size))
    {
      fprintf(stderr, "ox: internal error, formsbuf overflow (text too big).  Stop.\n");
      abort();
    }
  if (*form)
    {
      if (*form == '-' || *form == 0x1)
	++form;

      unsigned char *lemm_form = form;
      
      /* This can happen when a word finishes inside a g:surro */
      if (wp->etype != e_g_gg)
	{
	  extern int use_legacy;
	  unsigned char *h = NULL;
	  if (use_legacy)
	    {
	      if ((h = unheth(form)))
		lemm_form = h;
	    }
	  appendAttr(wp,attr(a_form, h ? h : form));
	}

      if (fp_forms && !in_split_word && strcmp(wp->parent->names->pname,"ag"))
	fprintf(fp_forms,"%%%s:%s ",getAttr(wp,"xml:lang"),form);
      
      /* need to do this unconditionally? */
      if ((need_lemm || do_show_insts) && !suppress_lem)
	(*lemm_save_form_p)((const char*)getAttr(wp,"xml:id"),
			    (const char *)getAttr(wp,"xml:lang"),
			    (in_split_word<=1)?(const char *)lemm_form:"-",
			    word_tokp ? word_tokp->lang : NULL,
			    curr_field);
    }
  else
    {
      wp->etype = e_g_nonw;
      wp->names = &enames[e_g_nonw];
      set_nonw_id(wp);
      if (punct_word)
	appendAttr(wp,attr(a_type,ucc("punct")));
      else if (word_init_mutex == exciso)
	appendAttr(wp,attr(a_type,ucc("excised")));
      else if (((struct node*)firstChild(wp))->etype == e_g_surro)
	appendAttr(wp,attr(a_type,ucc("surro")));
      /* FIXME: should warn about lack of nonw type */
      if (nline_words > 0 && (!sparse_lem || w_sparse_lem))
	--nline_words;
    }
  if (pending_ub)
    {
      set_ub(wp,a_syn_ub_before,pending_ub);
      pending_ub = NULL;
    }
  last_word = wp;
  word_tokp = NULL;
  word_lang = NULL;
}

static void
wrapup_word(struct node *wp, enum t_type trigger)
{
  if (wp && wp->children.lastused && !in_split_word)
    {
      curr_logolang = NULL;
      finish_word(wp);
    }
  else if (in_split_word)
    {
      static char idbuf[16];
      if (in_split_word == 1)
	{
	  finish_word(wp);
	  ++in_split_word;
	  strcpy(idbuf,(char*)getAttr(wp,"xml:id"));
	  sw_add(wp);
	}
      else
	{
	  finish_word(wp);
	  appendAttr(wp,attr(a_headref,(unsigned char*)idbuf));
	  wp->etype = e_g_swc;
	  wp->names = &enames[e_g_swc];
	  sw_add(wp);
	  if (!split_word_unfinished)
	    {
	      char *refs = sw_refs();
	      char *fullform = sw_form();
	      struct node *head = sw_get_head();
	      appendAttr(head,attr(a_headform,getAttr(head,"form")));
	      setAttr(head,a_form,(unsigned char *)fullform);
	      if (fp_forms)
		{
		  fprintf(fp_forms,"%%%s:%s\n",getAttr(head,"xml:lang"),fullform);
		  list_locator(fp_forms);
		}
	      setAttr(head,a_contrefs,(unsigned char *)refs);
	      if (lemm_reset_form_p)
		lemm_reset_form_p((char*)getAttr(head,"xml:id"),fullform);
	      free(refs);
	      free(fullform);
	      sw_free();
	      in_split_word = 0;
	      appendAttr(wp,attr(a_swc_final,(unsigned char *)"1"));
	    }
	  else
	    {
	      ++in_split_word;
	      appendAttr(wp,attr(a_swc_final,(unsigned char *)"0"));
	    }
	}
      split_word_unfinished = 0;
    }
}

void
reset_forms_insertp()
{
  forms_insertp = formsbuf;
}

void
set_flags(struct node *np, struct flags *fp)
{
  int i;
  if (NULL == np)
    {
      fprintf(stderr, "ox: internal error, attempt to set flag on NULL node. Ignoring flags\n");
      return;
    }
  for (i = 0; i < fp->nattr; ++i)
    if (setAttr(np,fp->a[i].a, ucc(fp->a[i].s)))
      vwarning("%c: superfluous flag", fp->atf);
  /* CHECKME: does this work in pipes? */
  if (fp->h && !in_hash)
    {
      setAttr(np,a_g_ho,ucc("1"));
      in_hash = 1;
    }
}

void
set_uflags(struct node *np, struct uflags *ufp)
{
  int i;
  if (NULL == np)
    {
      fprintf(stderr, "ox: internal error, attempt to set uflag on NULL node. Ignoring uflags\n");
      return;
    }
  for (i = 0; i < ufp->nattr; ++i)
    if (setAttr(np,ufp->a[i].a, ucc(ufp->a[i].s)))
      vwarning("%c: superfluous flag", ufp->atf);
}

static int
nowt_to_render(struct node *n)
{
  register int i = 0;
  int xg = 0;

  if (!n)
    {
      fprintf(stderr,"attempt to traverse NULL node\n");
      return 1;
    }
  if (!n->type)
    {
      fprintf(stderr,"attempt to traverse node with NULL type\n");
      return 1;
    }
  if (*n->type == 't')
    return 0;

  if (!xstrcmp(n->type,"cdata"))
    return 0;

  for (i = 0; i < n->attr.lastused; ++i)
    if (!xstrcmp(((struct attr*)(n->attr.nodes[i]))->renpair[0],"g:status"))
      {
	if (xstrcmp(((struct attr*)(n->attr.nodes[i]))->renpair[1],"excised"))
	  return 0;
	else
	  xg = 1;
      }

#if 0
  if (xg)
    return 0;
#endif

  if (!xg && n->children.lastused)
    for (i = 0; i < n->children.lastused; ++i)
      if (!nowt_to_render(n->children.nodes[i]))
	return 0;

  return 1;
}

static int
long_surro(int tindex)
{
  while (tokens[tindex])
    {
      if (tokens[tindex]->type == surrc)
	return 0;
      if (tokens[tindex]->type == space) /* norm */
	return 1;
      else if (tokens[tindex]->type == smetao)
	return 1;
      ++tindex;
    }
  /* This is an error condition but is reported elsewhere */
  return 0;
}

static struct token *
surro_norm(int tindex)
{
  while (tokens[tindex])
    {
      if (tokens[tindex]->type == surrc)
	return NULL;
      if (tokens[tindex]->type == norm)
	return tokens[tindex];
      ++tindex;
    }
  /* This is an error condition but is reported elsewhere */
  return NULL;
}

#if 0
static int
g_surro(int tindex)
{
  int init_index = tindex;

  while (tindex > 0 && tokens[tindex-1]->class != text)
    {
      if (tokens[tindex-1]->type == detc)
	{
	  --tindex;
	  while (tindex > 0 && tokens[tindex-1]->type != deto)
	    --tindex;
	}
      else
	--tindex;
    }

  while (tindex > 0 
	 && tokens[tindex-1]->type != deto
	 && tokens[tindex-1]->type != detc
	 && tokens[tindex-1]->class != bound
	 && tokens[tindex-1]->type != field
	 && tokens[tindex-1]->type != cell)
    --tindex;

  if (tindex 
      && tokens[tindex-1]->type != space
      && tokens[tindex-1]->type != field
      && tokens[tindex-1]->type != cell)
    return 1;

  tindex = init_index;

  while (tokens[tindex] && tokens[tindex]->type != surrc)
    {
      /* MIN<(A BA)>-ma is not a g_surro */
      if (tokens[tindex]->type == space)
	return 0;
      ++tindex;
    }

  if (tokens[tindex])
    {
      while (tokens[tindex+1] && tokens[tindex+1]->class != bound
	     && tokens[tindex+1]->type != cell)
	++tindex;
    }

  if (!tokens[tindex] || !tokens[tindex+1] || tokens[tindex+1]->type == space
      || tokens[tindex+1]->type == cell)
    return 0;

  return 1;
}
#endif

const char *
curr_word_id(void)
{
  return word_id_buf;
}

char *
note_id_string(int n)
{
  char notebuf[32];
  sprintf(notebuf,"note.%d",n);
  return (char*)pool_copy((unsigned char *)notebuf);
}

static int
logo_word_lang(struct node *wp, struct token *tp)
{
  const char *logo_script = NULL;
  const unsigned char *wp_lang = getAttr(wp, "xml:lang");
  char *wlang = NULL;
  const char *curr_logolang = tp->altlang;

  /* don't execute when g_s is a determinative or other non-lexical item */
  if (!wp->names)
    return 1;

  if (!xstrcmp(wp->names->pname,"g:d"))
    return 1;

  if (!curr_logolang)
    curr_logolang = tp->lang->core->altlang;

  switch (*curr_logolang)
    {
    case 'a':
      logo_script = "944";
      break;
    case 'l':
      logo_script = "945";
      break;
    case 's':
      logo_script = "946";
      break;
    default:
      fprintf(stderr, "ox tokenizer internal error: no logo script for lang %s\n",
	      logo_lang->core->name);
      break;
    }
  wlang = malloc(strlen((const char *)wp_lang)+5);
  strcpy(wlang, (const char *)wp_lang);
  if (isdigit(wlang[strlen((const char *)wp_lang)-1]))
    {
      char *insert = strrchr((const char *)wp_lang,'-');
      if (insert)
	sprintf(insert,"%s",logo_script);
      else
	fprintf(stderr, "ox tokenizer internal error: script code without hyphen in lang %s\n",
		wp_lang);
    }
  else
    {
      strcat(wlang,"-");
      strcat(wlang,logo_script);
    }
  setAttr(wp,a_xml_lang,(unsigned const char *)strdup((const char *)wlang));

  return 0;
}

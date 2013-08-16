#include <stdio.h>
#include <ctype128.h>
#include <stdlib.h>
#include <string.h>

#include <list.h>
#include "globals.h"
#include "cdf.h"
#include "run.h"
#include "protocols.h"
#include "block.h"
#include "text.h"
#include "blocktok.h"
#include "xmlnames.h"
#include "tree.h"
#include "warning.h"
/*#include "tokenizer.h"*/
#include "pool.h"
#include "nonx.h"
#include "translate.h"
#include "labtab.h"
#include "label.h"
#include "inline.h"
#include "lemline.h"
#include "note.h"

extern int lem_autolem, mylines;

struct lno
{
  int colno;
  int colprimes;
  int lineno;
  int lineprimes;
} lninfo;

void reset_lninfo(void);

#define MAX_ROMAN 40
const char * const roman[] = {
  "", 
  "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix", "x", 
  "xi", "xii", "xiii", "xiv", "xv", "xvi", "xvii", "xviii", "xix", "xx", 
  "xxi", "xxii", "xxiii", "xxiv", "xxv", "xxvi", "xxvii", "xxviii", "xxix", "xxx",
  "xxxi", "xxxii", "xxxiii", "xxxiv", "xxxv", "xxxvi", "xxxvii", "xxxviii", "xxxix", 
  "xl"
};

char block_tok_save = '\0';

struct node*current = NULL;

int div_level = 0;

/* offset used in calculation of word-ids.  In main text word-ids
   add 1..n; in exemplar 1 they add 1001 .. n+1000; in exemplar 2
   they add 2001 .. n+2000, etc. */
int exemplar_offset = 0;
int bil_offset = 0;

/* This variable gets reset when @translation is encountered
   so take care when using it */
struct node*text_element = NULL;

static int var_lines_in_lg = 0;
static int m_object_index = 0;

static struct node *current_trans = NULL;

enum block_levels current_level;
static enum e_tu_types transtype = 0;

static enum f_type flags = f_none;

unsigned const char *curr_discourse;

Hash_table *last_tlit_h_hash = NULL;
static struct node **last_tlit_h = NULL;
static int lth_alloced = 0;
static int lth_used = 0;
static int last_tlit_h_decay = 0;

extern const char *project;
extern int has_links;
extern int header_id;
extern int need_lemm;

extern void link_check(const char *line);
extern void set_flags(struct node *np,struct flags*fp);

static struct node *attach_point(enum block_levels b);
static void block(unsigned char *line,unsigned char *eol,struct block_token *bp);
static struct node *c_attach_point(void);
static void concat_continuations(unsigned char **lines);
static int comment(unsigned char **lines);
static int note(unsigned char **lines);
static void document(unsigned char *line,struct block_token *bp);
static void line_bil(unsigned char *lp);
static void line_gus(unsigned char *lp);
static void line_mts(unsigned char *line);
static void line_nts(unsigned char *lp);
static void line_lgs(unsigned char *lp);
static void line_var(unsigned char *lp);
/*static void line_las(unsigned char *lp);*//*deprecated, probably not coming back*/
static struct node *milestone(unsigned char *l,struct block_token*bp);
static struct node *end_discourse(unsigned char *em);
static unsigned const char *lnstr(int number,int primes);
static unsigned char *ntoken(unsigned char *s,unsigned char *eol,int multi,enum a_type a);
static struct node *scan_incref(unsigned char *s, enum e_type type);
static unsigned char **line_trans(unsigned char **lines, enum e_tu_types transtype);

static int
is_exemplar(const unsigned char *l)
{
  const unsigned char *entry = l;

  if (!isalnum(*l) && ':' != *l)
    return 0;
  while (*l && !isspace(*l))
    ++l;
  return l > entry && l[-1] == ':';
}

static unsigned char *
map_uscore(const unsigned char *vbar)
{
  unsigned char *tmp = pool_copy(vbar), *s;
  for (s = tmp; *s; ++s)
    if (*s == '_')
      *s = ' ';
  return tmp;
}

void
block_term()
{
  (void)scan_comment(NULL,NULL,0);
  (void)ntoken(NULL,NULL,0,0);
}

const unsigned char *
nth_tlit_hdr(const unsigned char *id, int nth)
{
  struct node **tmp_tlit_h = NULL;
  if (id)
    {
      tmp_tlit_h = hash_find(last_tlit_h_hash, id);
      if (tmp_tlit_h)
	{
	  int i;
	  for (i = 0; i < nth; ++nth)
	    if (NULL == tmp_tlit_h[i])
	      break;
	  if (tmp_tlit_h[i])
	    return getAttr(tmp_tlit_h[i],"xml:id");
	}
    }
  return NULL;
}

static int
needs_lg(unsigned char **ll)
{
#if 1
  while (ll[0] && (!strncmp((char*)ll[0],"#lem:",5) 
		   || !strncmp((char*)ll[0],"#etcsl:",7)
		   || ll[0][0] == '<'
		   || ll[0][0] == '>'
		   || ll[0][0] == '+'
		   ))
    ++ll;
  while (ll[0] && !ll[0][0])
    ++ll;
  return ll[0] && (is_exemplar(ll[0]) 
		   || ll[0][0] == ':' || ll[0][0] == '=' 
		   || ll[0][0] == '.' || ll[0][0] == '{');
#else
  unsigned char *l = *ll;
  if (l)
    {
      while (1)
	{
	  while (isspace(*l))
	    ++l;
	  if (*l && '#' != *l)
	    break;
	  else
	    while (*l)
	      ++l;
	  if (*l)
	    ++l;
	  else
	    break;
	}
      if (':' == *l || '=' == *l)
	return 1;
    }
#endif
  return 0;
}

static int
is_blank_line(const unsigned char *l)
{
  while (isspace(*l))
    ++l;
  return !*l;
}

#define skip_blank() \
  while (lines[1]) \
    {					\
      unsigned char *next = lines[1];	\
      while (isspace(*next))	     	\
	++next;				\
      if (*next)			\
	break; 				\
      if (lem_autolem)			\
	lem_save_line(next);		\
      ++lnum; 				\
      ++lines;				\
    }

unsigned char **
parse_block(struct run_context *run, struct node *text, unsigned char **lines)
{
  unsigned char *token;
  unsigned char *s, save;
  struct block_token *blocktokp;
  struct nonx *nonxp;
  struct node *tmp;
  int nflags;
  enum block_levels nonx_attach;
  unsigned char *eol = NULL;

  transtype = 0;
  curr_lang = global_lang;
  protocol_state = s_inter;
  current = text_element = text;
  reset_labels();
  if (last_tlit_h_hash)
    {
      hash_free(last_tlit_h_hash,hash_xfree);
      last_tlit_h_hash = NULL;
    }
  while (*lines != NULL)
    {
      ++lnum;
      
      if (is_blank_line(*lines))
	{
	  if (lem_autolem)
	    lem_save_line(*lines);
	  ++lines;
	  continue;
	}

      if (lines[1] == NULL)
	eol = *lines + xxstrlen(*lines);

      /* This means that the processor will produce partial output
	 on error; caller must take care to look at the processor status
	 return and report it to users */
      if (need_lemm && **lines != '&' 
	  && (!transtype || **lines == '@' || **lines == '$' || **lines == '#'))
	{
	  if (*lines && xstrncmp(*lines,"#lem:",4))
	    {
	      if (!lem_autolem)
		concat_continuations(lines);
	      lem_save_line(*lines);
	    }
	}

      switch (**lines)
	{
	case '&':
	  if (current_trans)
	    {
	      trans_cols_attr(current_trans);
	      current_trans = NULL;
	    }
	  return lines;
	  break;
	case '#':
	  {
	    register const unsigned char *s = *lines;

	    if (lines[0][1] == 'e' && !xstrncmp(*lines,"#eid:",5))
	      {
		++lines;
		continue;
	      }

	    while (*s && !isspace(*s))
	      if (':' == *s && (isspace(s[1]) 
				|| ('^' == s[1] 
				    && (s - *lines) == 5 
				    && !strncmp((const char *)*lines,"#note",5))))
		break;
	      else
		++s;
	    if (':' == *s)
	      {
		if (!xstrncmp(*lines,"#note:",6))
		  {
		    int lines_used = note_parse_tlit(current, current_level, lines) - 1;
		    lines += lines_used;
		    lnum += lines_used;
		  }
		else if (!xstrncmp(*lines,"#tr",3)
			 && (lines[0][3] == '.' || lines[0][3] == ':'))
		  {
		    lines = trans_inter(lines);
		    --lnum;
		    continue;
		  }
		else
		  protocol(run, protocol_state, LINE, current, *lines);
	      }
	    else
	      {
		int lines_used = comment(lines) - 1;
		lines += lines_used;
		lnum += lines_used;
	      }
	  }
	  break;
	case '@':
	  token = s = *lines+1;
	  if (*token == '(' && transtype)
	    {
	      if (lem_autolem)
		{
		  /* We already dumped this before the switch */
		  /* lem_save_line(lines[0]); */
		  ++lines;
		}
	      else
		{
		  lines = trans_block(lines,token,NULL,'\0');
		}
	      continue;
	    }
	  while (*s && isalnum(*s))
	    ++s;
	  /* handle flags attached to token name, e.g., @obverse? */
	  flags = parse_flags(s,&nflags);
	  block_tok_save = save = *s;
	  eol = s + strlen((char*)s);
	  *s = '\0';
	  if (nflags)
	    s += nflags;
	  else
	    ++s;
	  while (isspace(*s))
	    ++s;
	  /*ADDME: block_junk_check(s+nflags); */
	  blocktokp = blocktok((const char *)token,xxstrlen(token));
	  if (!blocktokp)
	    {
	      vwarning("unknown block token: %s", token);
	      ++lines;
	      continue;
	    }
	  else if (doctype == e_composite 
		   && (blocktokp->type == OBJECT
		       || blocktokp->type == SURFACE
		       || blocktokp->type == COLUMN))
	    {
	      vwarning("block token %s not allowed in composite",token);
	      ++lines;
	      continue;
	    }
	  switch (blocktokp->type)
	    {
	    case TEXT:
	      document(*lines,blocktokp);
	      break;
	    case OBJECT:
	    case SURFACE:
	    case DIVISION:
	      reset_lninfo();
	      block(*lines,eol ? eol : &(lines[1])[-1],blocktokp);
	      break;
	    case COLUMN:
	      ++lninfo.colno;
	      lninfo.lineno = lninfo.lineprimes = 0;
	      block(*lines,eol ? eol : &(lines[1])[-1],blocktokp);
	      break;
	    case MILESTONE:
	      if (xstrcmp("fragment",blocktokp->name) || xstrcmp("cfragment",blocktokp->name))
		{
		  if (doctype == e_composite)
		    current = c_attach_point();
		  else if (current->level != COLUMN)
		    current = attach_point(COLUMN);
		}
	      /* otherwise leave current wherever it is */
	      if (blocktokp->etype == e_include
		  || blocktokp->etype == e_referto)
		{
		  struct node *increfp;
		  /* *s = save; */ /* s already points to the first char to read */
		  increfp = scan_incref(s, blocktokp->etype);
		  if (increfp)
		    appendChild(current,increfp);
		  ++has_links;
		}
	      else
		{
		  appendChild(current,milestone(*lines,blocktokp));
		}
	      break;
	    case TR_TRANS:
	      /* if we just processed another translation add and clear
		 the cols attribute */
	      if (current_trans)
		trans_cols_attr(current_trans);
	      
	      max_cells_attr(text_element);

	      /* translation must return a new tree that will only 
		 be used for label generation; actual trans tree building
		 is carried out in translate.c */
	      current = text_element = translation(lines,text,&transtype);
	      current_trans = curr_trans_tree();
	      if (transtype == etu_parallel)
		line_id = exemplar_offset = 0;
	      if (!current)
		return skip_rest(lines);
	      /* parse_inline = trans_parse_inline */
	      break;
	    case TR_SWITCH:
	      token += strlen(cc(token));
	      ++token;
	      while (isspace(*token))
		++token;
	      if (!strncmp(cc(token),"parallel",8))
		{
		  token += 8;
		  while (isspace(*token))
		    ++token;
		  if (*token)
		    vwarning("%s: junk after @transtype parallel", token);
		  transtype = etu_parallel;
		}
	      else
		vwarning("%s: only 'parallel' allowed after @transtype",token);
	      break;
	    case TR_H1:
	    case TR_H2:
	    case TR_H3:
	      if (!transtype)
		{
		  static char hbuf[5];
		  strcpy(hbuf,blocktokp->name);
		  strcat(hbuf,"x");
		  blocktokp = blocktok((const char *)hbuf,3);
		  block(*lines,eol ? eol : &(lines[1])[-1],blocktokp);
		  ++lines;
		  continue;
		}
	    case TR_LABEL:
	    case TR_SPAN:
	    case TR_UNIT:
	      note_initialize_line();
	    case TR_NOTE:
	      if (transtype)
		{
		  if (lem_autolem)
		    {
		      /* We did this already on the '@' above */
		      /* lem_save_line(lines[0]); */
		      ++lines;
		    }
		  else
		    lines = trans_block(lines,token,blocktokp,save);
		  continue;
		}
	      else
		{
		  vwarning("block token %s only allowed in @translation",token);
		}
	      break;
	    default:
	      abort();
	    }
	  break;
	case '$':
	  if (transtype)
	    {
	      lines = trans_dollar(lines);
	    }
	  else
	    {
	      s = &lines[0][1];
	      while (isspace(*s))
		++s;
	      if (doctype == e_composite)
		current = c_attach_point();
	      else if (current->level < OBJECT || current->level > COLUMN)
		current = attach_point(COLUMN);
	      /* class nonx as MILESTONE because it is supposed to float */
	      tmp = elem(e_nonx,NULL,lnum,MILESTONE);
	      sprintf(line_id_insertp,"%d", ++line_id);
	      setAttr(tmp,a_xml_id,ucc(line_id_buf));
	      if (*s == '@' && s[1] == '(')
		{
		  unsigned char *lstart;
		  s += 2;
		  lstart = s;
		  while (*s && *s != ')')
		    ++s;
		  if (*s)
		    {
		      *s++ = '\0';
		      setAttr(tmp,a_label,lstart);
		      setAttr(tmp,a_silent,ucc("1"));
		      check_label(lstart,0,pool_copy(ucc(line_id_buf)));
#if 0
		      while (*s)
			++s;
#endif
		    }
		  else
		    warning("label on $-line lacks closing ')'");
		  while (isspace(*s))
		    ++s;
		}
	      else if (dollar_fifo)
		dollar_add((const char*)getAttr(tmp,"xml:id"));
	      nonxp = parse_nonx(s);
	      if (nonxp)
		{
		  appendAttr(tmp,attr(a_strict,
				      nonxp->strict ? ucc("1") : ucc("0")));
		  if (nonxp->strict || nonxp->scope)
		    {
		      if (nonxp->strict
			  && (!nonxp->extent || nonxp->extent->class != x_rule)
			  && (!nonxp->scope || ((nonxp->scope->class != x_impression
						 && nonxp->scope->type != x_dock)
						&& !nonxp->state))
			  && !nonxp->ref)
			{
			  warning("strict $-lines must have scope and state indicators");
			  ++lines;
			  continue;
			}
		      else
			switch (nonxp->scope->type)
			  {
			  case x_object:
			    /* WATCHME: does this need to attach
			       to <object> in the same way as surface
			       nonx's need to attach to <surface>? */
			    nonx_attach = TEXT;
			    break;
			  case x_surface:
			    /* if we have:
@reverse
$ start of reverse missing
@column 1
                               the structure breaks if the nonx is
			       attached to <object> -- it's simpler
			       just to attach it to <surface>
			    */
			    nonx_attach = SURFACE;
			    break;
			  case x_column:
			    nonx_attach = SURFACE;
			    if (nonxp->state)
			      switch (nonxp->state->type)
				{
				case x_broken:
				  /* don't do multiple primes on columns */
				  if (!lninfo.colprimes)
				    ++lninfo.colprimes;
				  lninfo.lineno = 0;
				  lninfo.lineprimes = 1;
				  break;
				case x_effaced:
				case x_illegible:
				case x_traces:
				  if (nonxp->number 
				      && isdigit(*nonxp->number)
				      && !strchr(cc(nonxp->number),'-'))
				    lninfo.colno += strtol(cc(nonxp->number), NULL, 10);
				  else
				    {
				      ++lninfo.colprimes;
				      lninfo.lineno = 0;
				      lninfo.lineprimes = 1;
				    }
				  break;
				default:
				  break;
				}
			    break;
			  case x_line:
			    if (nonxp->state)
			      {
				switch (nonxp->state->type)
				  {
				  case x_broken:
				    ++lninfo.lineprimes;
				    break;
				  case x_effaced:
				  case x_illegible:
				  case x_traces:
				    if (nonxp->number 
					&& isdigit(*nonxp->number)
					&& !strchr(cc(nonxp->number),'-'))
				      lninfo.lineno += strtol(cc(nonxp->number), NULL, 10);
				    else
				      ++lninfo.lineprimes;
				    break;
				  default:
				    break;
				  }
			      }
			    nonx_attach = COLUMN;
			    break;
			  default:
			    nonx_attach = COLUMN;
			    break;
			  }
		    }
		  else
		    {
		      nonx_attach = COLUMN;
		    }
		  if (nonxp->ref)
		    {
		      appendAttr(tmp,attr(a_ref,ucc(nonxp->ref)));
		      switch (nonxp->scope->type)
			{
			case x_image:
			  appendAttr(tmp,attr(a_type, ucc("image")));
			  appendAttr(tmp,attr(a_alt, nonxp->literal));
			  break;
			case x_empty:
			  appendAttr(tmp,attr(a_type, ucc("empty")));
			  break;
			default:
			  appendAttr(tmp,attr(a_scope,
					      nonxp->scope ? ucc(nonxp->scope->str) 
					      : ucc("impression")));
			  break;
			}
		    }
		  else
		    {
		      if (nonxp->extent)
			appendAttr(tmp,attr(a_extent,ucc(nonxp->extent->str)));
		      else if (nonxp->number)
			appendAttr(tmp,attr(a_extent,ucc(nonxp->number)));
		      else if (nonxp->strict)
			warning("extent not found in strict $-line");
		      if (nonxp->scope)
			appendAttr(tmp,attr(a_scope,ucc(nonxp->scope->str)));
		      else if (nonxp->strict)
			appendAttr(tmp,attr(a_scope,ucc("line")));
		      if (nonxp->state)
			appendAttr(tmp,attr(a_state,ucc(nonxp->state->str)));
		      else
			{
			  appendAttr(tmp,attr(a_state,ucc("other")));
			  nonx_attach = COLUMN;
			}
		    }
		  if (*nonxp->flags)
		    appendAttr(tmp,attr(a_flags,ucc(nonxp->flags)));
		  appendChild(tmp,textNode(*s == '(' ? (s+1) : s));
		  if (doctype == e_composite)
		    current = c_attach_point();
		  else if (current->level != nonx_attach)
		    current = attach_point(nonx_attach);
		  appendChild(current,tmp);
		}
	    }
	  break;
	case '|':
	case '<':
	case '>':
	case '+':
	  if (transtype)
	    {
	      if (lem_autolem)
		lem_save_line(lines[0]);
	      else
		lines = line_trans(lines,transtype);
	      continue;
	    }
	  if ((*lines)[0] != (*lines)[1])
	    warning("malformed link");
	  else
	    {
	      struct node *e = appendChild(current,elem(e_protocol,NULL,
							lnum,current_level));
	      unsigned char*endp = *lines+strlen(cc(*lines));
	      while (isspace(endp[-1]))
		--endp;
	      if (*endp)
		*endp = '\0';
	      setAttr(e, a_type,ucc("link"));
	      appendChild(e,cdata(*lines));
	      link_check((const char *)*lines);
	      ++has_links;
	    }
	  break;
	case '\0':
	  /* empty line */
	  if (need_lemm && transtype)
	    lem_save_line(lines[0]);
	  break;
	case ' ':
	case '\t':
	case '\n':
	  if (transtype && lem_autolem)
	    lem_save_line(lines[0]);
	  else
	    {
	      s = *lines;
	      while (isspace(*s))
		++s;
	      if (*s)
		warning("non-empty lines may not start with spaces");
	    }
	  break;
	default:
	  /* This is the processing for an @translation */
	  if (transtype)
	    {
	      if (lem_autolem)
		{
		  lem_save_line(lines[0]);
		  ++lines;
		}
	      else
		lines = line_trans(lines,transtype);
	      continue;
	    }

	  /* Now we are processing @transliteration or @composite */
	  
	  /* First concatenate continuation lines and replace them
	     with blank lines */
	  concat_continuations(lines);

	  if (doctype == e_composite)
	    current = c_attach_point();
	  else if (current->level != COLUMN)
	    current = attach_point(COLUMN);

	  /*	  else if (lines[1] && *lines[1] == '=') */

	  note_initialize_line();

	  if (needs_lg(lines+1))
	    {
	      struct node *ocurrent = current;
	      current = elem(e_lg,NULL,lnum,LINE);
	      line_mts(*lines);
	      exemplar_offset = 0;
	      skip_blank();
	      /* FIXME: weak support for #lem: */
	      while (lines[1] 
		     && (lines[1][0] == '#' 
			 || lines[1][0] == '<'
			 || lines[1][0] == '>'
			 || lines[1][0] == '+'))
		{
		  if (lines[1][0] == '<' || lines[1][0] == '>' || lines[1][0] == '+')
		    {
		      ++lnum;
		      ++lines;
		      if ((*lines)[0] != (*lines)[1])
			warning("malformed link");
		      else
			{
			  struct node *e = appendChild(current,elem(e_protocol,NULL,
								    lnum,current_level));
			  unsigned char*endp = *lines+strlen(cc(*lines));
			  while (isspace(endp[-1]))
			    --endp;
			  if (*endp)
			    *endp = '\0';
			  setAttr(e, a_type,ucc("link"));
			  appendChild(e,cdata(*lines));
			  link_check((const char *)*lines);
			  ++has_links;
			}
		    }
		  else
		    {
		      ++lnum;
		      ++lines;
		      if (!xstrncmp(*lines, "#lem:",5))
			concat_continuations(lines);
		      else
			lem_save_line(lines);
		      if (xstrncmp(*lines,"#eid:",5))
			protocol(run, protocol_state, LINE, current, *lines);
		    }
		  skip_blank();
		}
	      if (lines[1] && lines[1][0] == '=' && lines[1][1] == '{')
		{
		  ++lnum;
		  lem_save_line(lines[1]);
		  line_gus(*++lines);
		}
	      skip_blank();
	      if (lines[1] && lines[1][0] == '=' && lines[1][1] == '.')
		{
		  ++lnum;
		  lem_save_line(lines[1]);
		  line_nts(*++lines);
		}
	      skip_blank();
	      if (lines[1] && lines[1][0] == '=' && lines[1][1] == ':')
		{
		  ++lnum;
		  lem_save_line(lines[1]);
		  line_lgs(*++lines);
		}
	      skip_blank();
	      while (lines[1] && lines[1][0] == '=' && lines[1][1] == '=')
		{
		  ++lnum;
		  ++lines;
		  concat_continuations(lines);
		  lem_save_line(lines);
		  line_bil(*lines);
		  skip_blank();
		  /* FIXME: weak support for #lem: */
		  if (lines[1] && lines[1][0] == '#')
		    {
		      ++lnum;
		      ++lines;
		      if (!xstrncmp(*lines, "#lem:",5))
			concat_continuations(lines);
		      protocol(run, protocol_state, LINE, current, *lines);
		      skip_blank();
		    }		  
		}
	      skip_blank();
#if 0
	      if (lines[1] && !xstrncmp(lines[1],"#lem:",5))
		{
		  ++lnum;
		  line_las(*++lines);
		}
#endif
	      if (lines[1] && lines[1][0] == '=')
		{
		  vwarning("misplaced %c%c line (order is: unmarked,={,==,=:,=&)",
			   lines[1][0],lines[1][1]);
		  while (1)
		    {
		      ++lines;
		      ++lnum;
		      if (!lines[1] || lines[1][0] != '=')
			break;
		    }
		}
	      skip_blank();
	      var_lines_in_lg = 0;
	      while (lines[1] && is_exemplar(lines[1]))
		{
		  ++lnum;
		  ++lines;
		  if (need_lemm)
		    lem_save_line(*lines);
		  line_var(*lines);
		  skip_blank();
		  /* FIXME: weak support for #lem: */
		  if (lines[1] && lines[1][0] == '#')
		    {
		      ++lnum;
		      ++lines;
		      if (!xstrncmp(*lines, "#lem:",5))
			concat_continuations(lines);
		      protocol(run, protocol_state, LINE, current, *lines);
		      skip_blank();
		    }
		}
	      appendChild(ocurrent,current);
	      /* current is the <lg> node */
	      if (word_matrix)
		word_matrix_rewrite(current);
	      current = ocurrent;	      
	    }
	  else
	    line_mts(*lines);
	  break;
	}
      ++lines;
    }
  return lines;
}

static struct node *
attach_point(enum block_levels b)
{
  struct node *tmp = text_element;
  if (doctype == e_composite)
    abort(); /* my bad */
  while (tmp->level < b)
    {
      struct node *down = lastChild(tmp);
      if (down == NULL || down->names == NULL
	  || !xstrcmp(down->names->pname,"cmt") 
	  || !xstrcmp(down->names->pname,"nonx") 
	  || !xstrcmp(down->names->pname,"note:link") 
	  || !xstrcmp(down->names->pname,"note:text") 
	  || !xstrcmp(down->names->pname,"m") 
	  || !xstrcmp(down->names->pname,"protocol")
	  || !xstrcmp(down->names->pname,"protocols"))
	{
	  enum e_type tag = e_enum_top;
	  static const char *name = NULL;
	  enum block_levels level = tmp->level+1;
	  switch (level)
	    {
	    case OBJECT:
	      tag=e_object;
	      name="tablet";
	      break;
	    case SURFACE:
	      tag=e_surface;
	      name="obverse";
	      break;
	    case COLUMN:
	      tag = e_column;
	      name=NULL;
	      break;
	    default:
	      abort();
	    }

	  /* don't set down->user here; we don't render implicit structure */
	  down = appendChild(tmp,elem(tag,NULL,lnum,level));
	  if (name)
	    appendAttr(down,attr(a_type,ucc(name)));
	  appendAttr(down,attr(a_implicit,ucc("1")));
	  /* FIXME: set xml:id and label for implicits? */
	  if (!xstrcmp(down->names->pname,"column"))
	    {
	      setAttr(down,a_n,ucc("0"));
	      /* *line_label_buf = '\0'; */
	    }
	}
      tmp = down;
    }
  return tmp;
}

static void
block(unsigned char *line,unsigned char *eol,struct block_token *bp)
{
  enum e_type tag;
  register unsigned char *tok = NULL;
  unsigned char save, *htext, *block_tok_name = NULL;

  switch (bp->type)
    {
    case OBJECT:
      tag = e_object;
      break;
    case SURFACE:
#if 0
      if (!strcmp(line, "@surface"))
	{
	  if (block_tok_save)
	    {
	      line += strlen(line);
	      block_tok_name = ++line;
	      while (!isspace(*line))
		++line;
	      if (isspace(*line))
		*line++ = '\0';
	      while (isspace(*line))
		++line;
	    }
	  else
	    {
	      vwarning("please name the type of the @surface; assuming \"surface\" for now");
	    }
	}
#endif
      tag = e_surface;
      break;
    case COLUMN:
      tag = e_column;
      break;
    case LINE:
      tag = e_l;
      break;
    case DIVISION:
      tag = (*bp->name == 'e' ? e_enum_top : e_div);
      break;
    case H1:
    case H2:
    case H3:
      tag = bp->etype;
      break;      
    default:
      abort();
    }
  switch (tag)
    {
    case e_div:
      while (current->level != DIVISION && current->level != TEXT)
	current = current->parent;
      current = appendChild(current,elem(tag,NULL,lnum,bp->type));
      while (*line && !isspace(*line))
	++line;
      if (line < eol)
	++line;
      while (isspace(*line))
	++line;
      if (*line)
	{
	  int segflag = 0;
	  int tabflag = 0;
	  int verflag = 0;
	  int non_label_div = 0;
	  unsigned char *divtok = NULL;
	  ++div_level;
	  tok = line;
	  while (*line && !isspace(*line))
	    ++line;
	  save = *line;
	  *line = '\0';
	  appendAttr(current, attr(a_type,tok));
	  if (!xstrcmp(tok,"segment"))
	    segflag = 1;
	  else if (!xstrcmp(tok,"tablet"))
	    tabflag = 1;
	  else if (!xstrcmp(tok,"version"))
	    verflag = 1;
	  else if (!xstrcmp(tok,"kirugu")
		   || !xstrcmp(tok,"trailer")
		   || !xstrcmp(tok,"jicgijal"))
	    non_label_div = 1;
	  else
	    {
	      divtok = pool_copy(tok); /* FIXME: use a text-freeable 
					  pool here */
	    }
	  *line = save;
	  if (*line)
	    {
	      while (isspace(*line))
		++line;
	      tok = line;
	      line = line+xxstrlen(line);
	      while (isspace(line[-1]))
		--line;
	      if (*line)
		*line = '\0';
	      appendAttr(current,attr(a_n,tok));
	    }
	  else
	    tok = (unsigned char *)"";
	  if (verflag && xstrcmp(tok,"0"))
	    {
	      unsigned char *tokend = tok;
	      while (*tokend && !isspace(*tokend))
		++tokend;
	      *tokend = '\0';
	      label_segtab("Ver.",tok);
	    }
	  if (!non_label_div)
	    {
	      if (segflag && xstrcmp(tok,"0"))
		label_segtab("Seg.",tok);
	      else if (tabflag && xstrcmp(tok,"0"))
		label_segtab("Tab.",tok);
	      else if (divtok)
		label_segtab(cc(divtok),tok);
	    }
	}
      else if (!xstrcmp(bp->name,"variants")) 
	{
	  /* current = appendChild(current,elem(e_variants,NULL,lnum,bp->type)); */
	  setName(current,e_variants);
	  current = appendChild(current,elem(e_variant,NULL,lnum,bp->type));
	} 
      else if (!xstrcmp(bp->name,"variant"))
	{
	  struct node *parent = current->parent;
	  (void)removeLastChild(parent);
	  current = parent;
	  while (current && xstrcmp(current->names->pname,"variants"))
	    current = current->parent;
	  if (!current)
	    {
	      warning("orphan @variant (not preceded by '@variants')");
	      return;
	    }
	  else
	    current = appendChild(current,elem(e_variant,NULL,lnum,bp->type));	  
	}
      else
	warning("@div must give division type");
      break;
    case e_enum_top:
      while (*line && !isspace(*line))
	++line;	
      if (line < eol)
	++line;
      while (isspace(*line))
	++line;
      if (*line)
	{
	  if (tok && (!xstrcmp(tok,"body")
		      || !xstrcmp(tok,"catchline")
		      || !xstrcmp(tok,"colophon")
		      || !xstrcmp(tok,"date")
		      || !xstrcmp(tok,"linecount")
		      || !xstrcmp(tok,"sealings")
		      || !xstrcmp(tok,"signature")
		      || !xstrcmp(tok,"summary")
		      || !xstrcmp(tok,"witnesses")))
	    {
	      if (!xstrcmp(tok,curr_discourse))
		appendChild(current,end_discourse(tok));
	      else
		vwarning("mismatched milestones: found `@%s' expecting `@end %s'",
			 tok,curr_discourse);
		
	    }
	  else
	    {
	      while (current->level != DIVISION && current->level != TEXT)
		current = current->parent;
	      --div_level;
	      tok = line;
	      while (*line && !isspace(*line))
		++line;
	      save = *line;
	      *line = '\0';
	      if (!xstrcmp(tok,getAttr(current,"type")))
		{
		  *line = save;
		  if (*line)
		    {
		      while (isspace(*line))
			++line;
		      if (*line)
			{
			  register unsigned char *tok2 = line;
			  line = line + xxstrlen(line);
			  while (isspace(line[-1]))
			    --line;
			  if (!xstrcmp(tok2,getAttr(current,"n")))
			    current = current->parent;
			  else
			    {
			      static char type[16],n[16];
			      xstrcpy(type,getAttr(current,"type"));
			      xstrcpy(n,getAttr(current,"n"));
			      vwarning("mismatched strict @div %s %s ... @end %s %s",
				       type,n,
				       tok, tok2);
			    }
			}
		      else
			current = current->parent;
		    }
		  else
		    current = current->parent;
		}
	      else if (current_trans && !xstrcmp(tok,"translation"))
		{
		  /* @translation ... @end translation is OK */
		  if (current_trans)
		    trans_cols_attr(current_trans);
		}
	      else
		vwarning("mismatched @div %s ... @end %s",
			 getAttr(current,"type"),tok);
	    }
	}
      else if (!xstrcmp(bp->name,"endvariants"))
	{
	  while (current && xstrcmp(current->names->pname,"variants"))
	    current = current->parent;
	  if (!current)
	    {
	      warning("orphan @endvariants (not preceded by '@variants')");
	      return;
	    }
	  else
	    current = current->parent;
	}
      else
	warning("@end must give division type");
      break;
    case e_h:
      if (doctype == e_composite)
	current = c_attach_point();
      else
	current = attach_point(COLUMN);
      current = appendChild(current,elem(e_h,NULL,lnum,bp->type));
      if ((lth_used+1) >= lth_alloced)
	{
	  lth_alloced += 8;
	  last_tlit_h = realloc(last_tlit_h, lth_alloced * sizeof(struct node *));
	}
      last_tlit_h[lth_used++] = current;
      last_tlit_h_decay = 1;

      appendAttr(current,attr(a_level,
			      (unsigned char *)(bp->name[1]=='1'?"1":(bp->name[1]=='2'?"2":"3"))));
      sprintf((char *)idbuf,"%s.h%d",textid,header_id++);
      appendAttr(current,attr(a_xml_id,idbuf));
      line += strlen((const char *)line);
      if (line < eol)
	++line;
      while (isspace(*line))
	++line;
      htext = line;
      /* this really needs to be a parsed para */
      (void)trans_inline(current,htext,NULL,0);
      /*appendChild(current,textNode(htext));*/
      break;
    default:
      if (doctype == e_composite)
	current = c_attach_point();
      else
	current = attach_point(bp->type - 1);
      current = appendChild(current,elem(tag,NULL,lnum,bp->type));
      if (flags != f_none && bp->type != COLUMN)
	set_flags(current,&flag_info[flags]);
      switch (bp->type)
	{
	case OBJECT:
	  setAttr(current,a_type,ucc(bp->name));
	  if (!xstrcmp(enames[tag].pname,bp->name))
	    {
	      unsigned char *l = ntoken(line,eol,1,a_n);
	      if (l && *l)
		{
		  unsigned char *new_l = l;
		  if (!xstrncmp(l,"fragment",8))
		    {
		      static unsigned char nfrg[16] = { "frg." };
		      l+=8;
		      while (isspace(*l))
			++l;
		      xstrcpy(nfrg+4,l);
		      new_l = nfrg;
		    }
		  else
		    new_l = l;
		  current->user = newlabel('n',OBJECT,new_l);
		  update_labels(current,transtype);
		}
	    }
	  else
	    {
	      current->user = newlabel('b',OBJECT,bp);
	      update_labels(current,transtype);
	    }
	  break;
	case SURFACE:
	  setAttr(current,a_type,ucc(bp->name));
#if 0
	  if (block_tok_name)
	    {
	      setAttr(current,a_n,ucc(block_tok_name));
	      block_tok_name = NULL;
	    }
#endif
	  if (!xstrcmp(enames[tag].pname,bp->name)
	      || !xstrcmp(bp->name,"face"))
	    {
	      unsigned char *l = NULL;

	      if (block_tok_save)
		{
		  l = ntoken(line,eol,1,a_n);
		  if (l && *l)
		    {
		      if (!xstrcmp(bp->name,"face") 
			  && (l[1] || !islower(l[0])))
			vwarning("%s: prism's @face must be single lowercase letter; ('@surface face %s' is legal)",l,l);
		      current->user = newlabel('n',SURFACE,l);
		      update_labels(current,transtype);
		    }
		}
	      else
		{
		  vwarning("expected surface type after @surface");
		  
		}
	    }
	  else
	    {
	      unsigned char *l = ntoken(line,eol,1,a_n);
	      if (l && *l)
		{
		  if (!strcmp(bp->n,"edge"))
		    {
		      char lbuf[128], *lp = (char*)l;
		      while (*lp)
			{
			  if (!islower(*lp))
			    break;
			  else
			    ++lp;
			}		      
		      if (*lp)
			warning("designation of @edge must be lowercase letter");
		      else
			{
			  sprintf(lbuf,"%s%s",bp->nano,l);
			  current->user = newlabel('n',SURFACE,pool_copy((unsigned char *)lbuf));
			  update_labels(current,transtype);
			}
		    }
		  else if (!strcmp(bp->n,"seal") || !strcmp(bp->n,"docket"))
		    {
		      char lbuf[128], *lp;
		      lp = (char*)l + strlen(cc(l));
		      while (isspace((unsigned char)lp[-1]))
			--lp;
		      if (*lp)
			*lp = '\0';
		      sprintf(lbuf,"%s %s",bp->n,l);
		      current->user = newlabel('n',SURFACE,pool_copy((unsigned char *)lbuf));
		      update_labels(current,transtype);
		    }
		  else
		    vwarning("%s: block token %s does not take qualifiers",
			     l, bp->n);
		}
	      else
		{
		  if (!strcmp(bp->n,"seal"))
		    vwarning("@seal must be followed by a label (say `@seal 1')");
		  current->user = newlabel('b',SURFACE,bp);
		  update_labels(current,transtype);
		}
	    }
	  break;
	case COLUMN:
	  /* set o or n to user column number */
	  (void)ntoken(line, eol, 0, mylines ? a_n : a_o);
	  if (!mylines)
	    appendAttr(current,attr(a_n,lnstr(lninfo.colno,lninfo.colprimes)));
	  /* if ('1' != *(getAttr(current,"implicit"))) */
	  update_labels(current,transtype);
	  break;
	case LINE:
	default:
	  abort();
	}
    }
}

static struct node *
c_attach_point()
{
  while (current->parent 
	 && (current->level != DIVISION && current->level != TEXT))
    current = current->parent;
  return current;
}

static void
concat_continuations(unsigned char **lines)
{
  unsigned char *dst = *lines + strlen(cc(*lines));
  while (lines[1] && isspace(*lines[1]))
    {
      unsigned char *l = lines[1];
      if (need_lemm)
	lem_save_cont(l);
      while (isspace(*l))
	++l;
      if (l)
	{
	  unsigned char *src = lines[1];
	  *dst++ = ' ';
	  *src++ = '\0';
	  while (isspace(*src))
	    ++src;
	  while (*src)
	    *dst++ = *src++;
	  *dst = '\0';
	  lines[1] = (unsigned char*)""; /* WATCHME: should this be strdup("")? */
	  ++lines;
	}
      else
	break;
    }
}

static int
maybe_protocol(const unsigned char *l)
{
  while (*l && !isspace(*l))
    ++l;
  return l[-1] == ':';
}

unsigned char *
scan_comment_sub(unsigned char **lines, int *nlinesp, int badcolon)
{
  int nlines = 0, i;
  size_t cmt_len = 0;
  static unsigned char *buf = NULL;
  static int bufsize = 0;
  register int bufused = 0;

  if (!lines)
    {
      free(buf);
      buf = NULL;
      bufsize = 0;
      return NULL;
    }

  while (lines[nlines] && *lines[nlines] == '#')
    {
      if (nlines && need_lemm)
	lem_save_line(lines[nlines]);
      if (maybe_protocol(lines[nlines]))
	{
	  if (badcolon)
	    {
	      notice("protocol in comment? (say, e.g., '# link:' or move protocol before comment");
	    }
	  else if (!xstrncmp(cc(lines[nlines]),"#note:",6))
	    {
	      goto stop;
	    }
	}
      cmt_len += xxstrlen(lines[nlines]);
      ++nlines;
    }
 stop:
  while (1+bufused+cmt_len > bufsize)
    {
      if (!bufsize)
	bufsize = 8;
      else
	bufsize *= 2;
      buf = realloc(buf,bufsize);
    }
  
  /*   bufp = buf = malloc(cmt_len+1); */
  for (i = 0; i < nlines; ++i)
    {
      register unsigned char *l = lines[i];
      if (i)
	buf[bufused-1] = '\n';
      ++l;
      while ((buf[bufused++] = *l++))
	;
    }
  *nlinesp = nlines;
  return buf;
}

struct node *
scan_comment(unsigned char **lines, int *nlinesp, int badcolon)
{
  struct node *e = NULL;
  if (lines)
    {
      e = elem(e_cmt,NULL,lnum,current_level);
      appendChild(e,cdata(scan_comment_sub(lines,nlinesp,badcolon)));
    }
  else
    scan_comment_sub(NULL,NULL,0);
  return e;
}

static int
comment(unsigned char **lines)
{
  int nlines;
  appendChild(current,scan_comment(lines,&nlines,1));
  return nlines;
}


static void
document(unsigned char *line,struct block_token *bp)
{
  switch (protocol_state)
    {
    case s_file:
      warning("document type not allowed before text");
      break;
    case s_text:
      setName(text_element,bp->etype);
      /* setAttr(text_element,a_implicit,ucc("0")); */
      break;
    default:
      warning("misplaced document type");
      break;
    }
}

static unsigned char **
line_trans(unsigned char **lines, enum e_tu_types transtype)
{
  unsigned char *tok = *lines;
  /*  unsigned char *end = tok+xxstrlen(tok); */
  unsigned char *s = *lines;

  curr_lang = text_lang;
  protocol_state = s_intra;

  if (transtype == etu_parallel)
    {
      lem_save_line(s);
      while (*s && !isspace(*s))
	{
	  ++s;
	}
      if (s[-1] != '.')
	{
	  warning("malformed line number in translation");
	  return lines+1; /* FIXME: this is going to result in some multiple errors */
	}
      s[-1] = '\0';
      sprintf(line_id_insertp,"%d",++line_id);
      curr_line_label = line_label(tok,transtype,NULL);
      while (isspace(*s))
	++s;
    }
  
  lines = trans_para(lines,s,NULL,1,1,curr_line_label,1);
  --lnum;
  return lines;
}

static void
line_mts(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  struct attr *ap,*xid;
  unsigned char *tok = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *s = lp;

  ++lninfo.lineno;

  curr_lang = text_lang;
  protocol_state = s_intra;

  while (*s && !isspace(*s))
    {
      ++s;
    }
  if (s[-1] != '.')
    {
      warning("malformed line number");
      warning("(line numbers must be <non-space><period><space>, e.g., `1. ')");
      return;
    }
  else if (s == lp+1)
    {
      warning("empty line number");
    }
  s[-1] = '\0';
  sprintf(line_id_insertp,"%d", ++line_id);
  xid = attr(a_xml_id,ucc(line_id_buf));
  if (last_tlit_h_decay)
    {
      last_tlit_h_decay = 0;
      if (!last_tlit_h_decay && lth_used)
	{
	  struct node **tmp = calloc(lth_used+1,sizeof(struct node*));
	  memcpy(tmp,last_tlit_h,lth_used * sizeof(struct node *));
	  if (!last_tlit_h_hash)
	    last_tlit_h_hash = hash_create(3);
	  hash_add(last_tlit_h_hash,(const unsigned char *)xid->valpair[1],tmp);
	}
      lth_used = 0;
    }
  appendAttr(lnode,xid);
  if (atf_cbd_err)
    {
      extern int cbd_err_line;
      cbd_err_line = atoi((char*)tok);
    }
  if (!mylines)
    {
      appendAttr(lnode,attr(a_o,tok));
      tok = (unsigned char*)lnstr(lninfo.lineno,lninfo.lineprimes);
      appendAttr(lnode,attr(a_n,tok));
    }
  else
    appendAttr(lnode,attr(a_n,tok));

  curr_line_label = line_label(tok,0,(unsigned const char *)xid->valpair[1]);
  if (curr_line_label)
    {
      ap = attr(a_label,curr_line_label);
      appendAttr(lnode,ap);
      register_label(labtab,uc(xid->valpair[1]),uc(ap->valpair[1]));
      free((char*)curr_line_label);
      curr_line_label = ucc(ap->valpair[1]);
    }
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      /* a leading |<SPACE> means span all cells */
      if (*s == '|' && isspace(s[1]))
	{
	  s += 2;
	  appendAttr(lnode,attr(a_spanall,(unsigned char *)"1"));
	}
      
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,1, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
  else
    tlit_reinit_inline(WITH_WORD_LIST);
}

static void
line_bil(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  appendAttr(lnode,attr(a_type,ucc("bil")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode, 
			    ++bil_offset * 1000, 
			    WITH_WORD_LIST, uc(line_id_buf));
	}
    }
}

/* gloss under stream */
static void
line_gus(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  appendAttr(lnode,attr(a_type,ucc("gus")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,1000, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
}

/* normalized transliteration stream */
static void
line_nts(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  
  appendAttr(lnode,attr(a_type,ucc("nts")));
  appendChild(current,lnode);
  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,2000, WITH_WORD_LIST,uc(line_id_buf));
	}
    }
}

static void
line_lgs(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  unsigned char *s = lp+2;
  unsigned char *end = lp+xxstrlen(lp);
  appendAttr(lnode,attr(a_type,ucc("lgs")));
  while (isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  int i;
	  *end = '\0';
	  tlit_parse_inline(s,end,lnode,3000, NO_WORD_LIST,uc(line_id_buf));
	  /* this gives us g:w wrappers; unwrap and check there were only
	     space boundaries */
	  for (i = 0; i < lnode->children.lastused; ++i)
	    {
	      struct node *cp = lnode->children.nodes[i];
	      if (cp->children.lastused > 1)
		warning("boundary not allowed in literal grapheme sequence");
	      else if (cp->children.lastused)
		{
		  struct node *tmp = cp->children.nodes[0];
		  free(cp->children.nodes);
		  cp->children.nodes = NULL;
		  cp->children.lastused = 0;
		  lnode->children.nodes[i] = tmp;
		}
	    }
	}
    }  
  appendChild(current,lnode);
}

static void
line_var(unsigned char *lp)
{
  struct node *lnode = elem(e_v,NULL,lnum,LINE);
  unsigned char *s = lp, *entry = lp;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *n, *n_vbar;
  unsigned char lab[128];

  /* FIXME: stabilize this so that the same siglum maps
     to the same exemplar_offset within each text and 
     output a map of the siglum::exemplar_offset pairs */
  ++exemplar_offset;
  
  /* appendAttr(lnode,attr(a_type,ucc("var"))); */
  appendChild(current,lnode);
  while (*s && !isspace(*s))
    ++s;

  n = s-1; /* now n points at final ':' */
  *n = '\0';
  n = entry;
  if ((n_vbar = (unsigned char*)strchr((char*)n,'|')))
    *n_vbar++ = '\0';
  appendAttr(lnode,attr(a_varnum,n));
  sprintf((char*)lab, "%s [%s]", curr_line_label, n);
  appendAttr(lnode,attr(a_label,lab));
  if (n_vbar)
    appendAttr(lnode,attr(a_ex_label,map_uscore(n_vbar)));

  s[-1] = ':';

  while (*s && isspace(*s))
    ++s;
  if (s < end)
    {
      while (isspace(end[-1]) && end > s)
	--end;
      if (end > s)
	{
	  *end = '\0';
	  tlit_parse_inline(s, end, lnode, 1000 * exemplar_offset,
			    WITH_WORD_LIST, uc(line_id_buf));
	}
    }
}

#if 0
static void
line_las(unsigned char *lp)
{
  struct node *lnode = elem(e_l,NULL,lnum,LINE);
  appendAttr(lnode,attr(a_type,ucc("las")));
  appendChild(current,lnode);
}
#endif

static struct node *
end_discourse(unsigned char *em)
{
  struct node *m = elem(e_m,NULL,lnum,MILESTONE);
  appendAttr(m,attr(a_type,uc("discourse")));
  appendAttr(m,attr(a_subtype,em));
  appendAttr(m,attr(a_endflag,uc("1")));
  curr_discourse = NULL;
  return m;
}

static struct node *
milestone(unsigned char *l,struct block_token*bp)
{
  unsigned char *type = l+1;
  unsigned char *subtype = NULL, *m_div_n = NULL;
  struct node *m = elem(e_m,NULL,lnum,MILESTONE);
  extern int m_label_col_index;
  if (type[0] == 'm' && type[1] == '\0')
    {
      struct block_token *blocktokp;
      unsigned char *sub_n = NULL;
      type += 2; /* skip 'm=' */
      l = type;
      while (*l && !isspace(*l))
	++l;
      if (*l)
	*l++ = '\0';
      while (isspace(*l))
	++l;
      if (*l)
	sub_n = l;
      blocktokp = blocktok(cc(type),xxstrlen(type));
      if (blocktokp)
	{
	  switch (blocktokp->etype)
	    {
	    case e_object:
	      subtype = type;
	      type = uc("locator");
	      m_label_col_index = 0;
	      if (strcmp((const char *)subtype,"tablet") || sub_n)
		update_mlabel(e_object, sub_n ? sub_n : type);
	      m_object_index = m_label_col_index;
	      break;
	    case e_surface:
	      subtype = type;
	      type = uc("locator");
	      if (!m_object_index)
		m_label_col_index = 0;
	      update_mlabel(e_surface,
			    blocktokp->nano ? ucc(blocktokp->nano) : type);
	      break;
	    case e_column:
	      subtype = type;
	      type = uc("locator");
	      update_mlabel(e_column,l);
	      break;
	    default:
	      break;
	    }
	}
      else if (!xstrcmp("locator",type))
	{
	  subtype = l;
	  type = uc("locator");
	  m_label_col_index = 0;
	  update_mlabel(e_surface,l);
	}
      else if (!xstrcmp("division",type))
	{
	  static unsigned char buf[32];
	  unsigned char *n = l;
	  type = uc("division");
	  m_label_col_index = 0;	      
	  while (*n && !isspace(*n))
	    ++n;
	  if (*n)
	    {
	      *n++ = '\0';
	      while (isspace(*n))
		++n;
	      if (!*n)
		n = NULL;
	    }
	  if (n)
	    {
	      if (!xstrcmp(l,"segment"))
		sprintf((char*)buf,"Seg.%s,",n);
	      else if (!xstrcmp(l,"paragraph"))
		sprintf((char*)buf,"Par.%s,",n);
	      else if (!xstrcmp(l,"exemplar"))
		sprintf((char*)buf,"Ex.%s,",n);
	      else
		sprintf((char*)buf,"%s,",(char*)l);
	      subtype = l;
	      m_div_n = n;
	      update_mlabel(e_surface,buf);
	    }
	  else
	    {
	      subtype = l;
	      update_mlabel(e_surface,l);
	    }
	}
    }
  else
    {
      l += xxstrlen(l);
      if (!xstrcmp(type,"body")
	  || !xstrcmp(type,"catchline")
	  || !xstrcmp(type,"colophon")
	  || !xstrcmp(type,"date")
	  || !xstrcmp(type,"linecount")
	  || !xstrcmp(type,"sealings")
	  || !xstrcmp(type,"signature")
	  || !xstrcmp(type,"summary")
	  || !xstrcmp(type,"witnesses"))
	{
	  subtype = type;
	  /* curr_discourse gets reset later to point to the
	     pool_copy'd version of the subtype */
	  curr_discourse = type = uc("discourse");
	  if (!xstrcmp(subtype,"body"))
	    {
	      extern const char *default_discourse_level;
	      default_discourse_level = "preamble";
	    }
	}
      else if (!xstrcmp("fragment",bp->name) || !xstrcmp("cfragment",bp->name))
	{
	  ++l;
	  while (*l && isspace(*l))
	    ++l;
	  if (l)
	    {
	      unsigned char *end = l + xxstrlen(l);
	      unsigned char buf[32];
	      while (end > l && isspace(end[-1]))
		--end;
	      *end = '\0';
	      type = uc("locator");
	      subtype = uc("fragment");
	      if (doctype != e_composite)
		{
		  label_frag(current,l);
		  update_labels(current,transtype);
		}
	      else
		{
		  if (xxstrlen(l)+5 > 32)
		    warning("fragment label too long (max 26 letters)");
		  else
		    {
		      xstrcpy(buf,"frg.");
		      xstrcat(buf,l);
		      if ('c' == *bp->name)
			update_mlabel(e_enum_top,buf);
		      else
			update_mlabel(e_surface,buf);
		    }
		}
	    }
	  else
	    warning("@fragment must have frag indicator");
	}
    }
  appendAttr(m,attr(a_type,type));
  if (subtype)
    {
      appendAttr(m,attr(a_subtype,subtype));
      if (curr_discourse)
	curr_discourse = getAttr(m,"subtype");
    }
  if (m_div_n)
    {
      appendAttr(m,attr(a_n,m_div_n));
      *m_div_n = '\0';
    }
    
  if (*l)
    appendChild(m,cdata(l));
  return m;
}

/* arg is ptr to first character of @-token name; @-token has been
   zero-terminated; multi flag is 1 if ntoken can be multiple "words"
 */
static unsigned char *
ntoken(unsigned char *s,unsigned char *eol,int multi,enum a_type a)
{
  static int retp_len = 0;
  static unsigned char *retp = NULL;
  unsigned char *tok, *etok;
  enum f_type flags = f_none;
  int primes = 0;
  int nflags = 0;

  if (!s)
    {
      free(retp);
      retp = NULL;
      retp_len = 0;
      return NULL;
    }
  
  while (*s++)
    ;

  if (s >= eol)
    {
      if (current->level == COLUMN)
	warning("column must have number");
      return NULL;
    }

  while (*s && isspace(*s))
    ++s;

  if (isalnum(*s))
    {
      tok = s++;
      while (isalnum(*s) || '+' == *s || (multi && isspace(*s)) || '/' == *s)
	++s;
    }
  else
    return (unsigned char *)"";
#if 0
  /* this breaks '@reverse?' : what is it supposed to do? */
  else
    {
      warning("bad name in block");
      return NULL;
    }
#endif

  while ('\'' == *s)
    {
      ++primes;
      *s++ = '\0';
    }
  flags = parse_flags(s,&nflags);
  if (nflags)
    {
      *s = '\0';
      s += nflags;
    }
  if (isspace(*s))
    *s++ = '\0';
  while (isspace(*s))
    ++s;
  if (*s)
    {
      vwarning("bad character in block line at: %s",s);
      return NULL;
    }
  etok = tok + strlen(tok);
  while (isspace(etok[-1]))
    *--etok = '\0';
  appendAttr(current,attr(a,tok));
  if (current->level == COLUMN)
    {
      /* tok must be set here because we've already returned if there's no colnum */
      if (isdigit(*tok))
	{
	  int r = atoi(cc(tok));
	  static char nbuf[128];
	  char pbuf[32];
	  *pbuf = '\0';
	  while (primes--)
	    strcat(pbuf,"'");
	  if (r > MAX_ROMAN)
	    {
	      vwarning("column number %s too big to romanize",tok);
	      strcpy(nbuf,cc(tok));
	      if (mylines && *pbuf)
		strcat(nbuf,pbuf);
	    }
	  else
	    {
	      strcpy(nbuf,roman[r]);
	      if (mylines && *pbuf)
		strcat(nbuf,pbuf);
	    }
	  current->user = newlabel('n',COLUMN,nbuf);
	}
      else
	warning("column number must begin with digit");
    }
  else if (current->level == SURFACE)
    {
      unsigned const char *type = getAttr(current,"type");
      if (!xstrcmp(type,"face"))
	{
	  /*	  current->user = newlabel('n',tok);*/
	  if (!islower(*tok))
	    vwarning("@face requires lowercase letter not '%s'",tok);
	}
    }
  else
    current->user = newlabel('n', current->level, tok);
  if (flags != f_none)
    set_flags(current,&flag_info[flags]);

  if (xxstrlen(tok) >= retp_len)
    {
      if (retp_len)
	retp_len *= 2;
      else
	retp_len = 128;
      retp = realloc(retp,retp_len);
    }
  xstrcpy(retp,tok);

  return pool_copy(retp);
}

void
reset_lninfo()
{
  memset(&lninfo,'\0',sizeof(struct lno));
  m_object_index = 0;
}

#include "linenums.c"

/*FIXME: test the array bounds and generate strings dynamically if 
  the line numbers are out of range */
static unsigned const char *
lnstr(int number,int primes)
{
  if (primes)
    {
      if (primes > 4)
	{
	  vwarning("%d is too many primes: restructure using @fragment\n", primes);
	  return ucc("");
	}
      else if (number > 500)
	{
	  vwarning("%d is too big!",number);
	  return ucc("");
	}
      else
	return ucc(lnstrsp[primes-1][number]);
    }
  else
    {
      if (number > 4999)
	{
	  /* vwarning("%d is too big!",number); */
	  char buf[10];
	  sprintf(buf,"%d",number);
	  return pool_copy((unsigned char*)buf);
	}
      else
	return ucc(lnstrs[number]);
    }
}

static struct node *
scan_incref(unsigned char *s, enum e_type type)
{
  unsigned char *ref,*n,*frag,
    *from = (unsigned char *)"",*to = (unsigned char *)"";
  struct node *increfp = elem(type,NULL,lnum,DIVISION);
  frag = (unsigned char *)strstr(cc(s)," # ");
  if (frag)
    *frag++ = '\0';
  while (isspace(*s))
    ++s;
  ref = s;
  while (*s && !isspace(*s))
    ++s;		  
  if (*s)
    {
      *s++ = '\0';
      while (isspace(*s))
	++s;
      if (*s == '=')
	{
	  ++s;
	  while (isspace(*s))
	    ++s;
	}
      else
	*s = '\0';
    }
  n = s;
  if (!*ref /* || !*n */)
    {
      warning("malformed @include or @referto");
      return NULL;
    }
  else
    {
      if (frag)
	{
	  s = frag + 1;
	  while (isspace(*s))
	    ++s;
	  from = s;
	  while (*s && '-' != *s)
	    ++s;
	  if ('-' == *s)
	    {
	      if (isspace(s[-1]) && isspace(s[1]))
		{
		  unsigned char *t = s;
		  while (t > from && isspace(t[-1]))
		    --t;
		  t[-1] = '\0';
		  ++s;
		  while (isspace(*s))
		    ++s;
		  if (*s)
		    {
		      to = s;
		      s = s + xxstrlen(s);
		      if (isspace(s[-1]))
			{
			  while (isspace(s[-1]))
			    --s;
			  s[-1] = '\0';
			}
		    }
		}
	      else
		warning("hyphen in #-part needs space on both sides");
	    }
	  else
	    {
	      if (isspace(s[-1]))
		{
		  while (isspace(s[-1]))
		    --s;
		  s[-1] = '\0';
		}
	    }
	}
      if (type == e_include && project)
	{
	  char buf[128];
	  sprintf(buf,"%s:%s",project,cc(ref));
	  appendAttr(increfp,attr(a_ref,ucc(buf)));
	}
      else
	appendAttr(increfp,attr(a_ref,ref));
      appendAttr(increfp,attr(a_n,n));
      if (*from)
	appendAttr(increfp,attr(a_from,from));
      if (*to)
	appendAttr(increfp,attr(a_to,to));
      return increfp;
    }
}

unsigned char **
skip_rest(unsigned char **lines)
{
  while (*lines)
    {
      if (lines[0][0] == '&')
	break;
      ++lines;
      ++lnum;
    }
  return lines;
}

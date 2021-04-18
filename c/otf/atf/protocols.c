#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include <psdtypes.h>
#include <hash.h>
#include "cdf.h"
#include "gdl.h"
#include "warning.h"
#include "run.h"
#include "protocols.h"
#include "text.h"
#include "lang.h"
#include "blocktok.h"
#include "block.h"
#include "translate.h"
#include "tree.h"
#include "nonx.h"
#include "xmlnames.h"
#include "hash.h"
#include "pool.h"
#include "lemline.h"
/*#include "ngram.h"*/
#include "xpd2.h"
#include "gdlopts.h"
#include "inline.h"
#include "proj_context.h"
#include "xcl.h"
#include "key.h"
#include "symbolattr.h"
#include "globals.h"


#define OLDLEM 0

#define C(x) #x,
const char *const scope_names[] = { T_SCOPES };
enum t_scope protocol_state = s_global;

extern const unsigned char *default_ftype;
extern int has_links;
extern int lem_extended;
extern int lem_props_strict;
const char *lem_props_strict_opt;
extern int lem_simplify;
const char *lem_simplify_opt;
extern int mixed_case_ok;
static int need_auto_syntax_line_is_unit = 0;
extern int use_legacy, use_unicode, use_ilem_conv;
extern const char *curr_dialect;

static void atf_handler(struct node *parent, enum t_scope scope, 
			enum block_levels level, unsigned char *l);
static void lem_handler(struct node *parent, enum t_scope scope, 
			enum block_levels level, unsigned char *l);
static int lemwords(int j, struct node *parent, unsigned char**lem);
static void lemmatizer_handler(struct node *parent, enum t_scope scope, 
			       enum block_levels level, unsigned char *l);
static void parse_score_doctype(struct node *np, unsigned char *l);
static void project_handler(struct run_context *run,
			    struct node *parent, enum t_scope scope, 
			    enum block_levels level, unsigned char *l);
static void version_handler(struct node *parent, enum t_scope scope, 
			    enum block_levels level, unsigned char *l);

extern void link_check_protocol(const char *line);

static int
is_blank_line(const unsigned char *l)
{
  while (isspace(*l))
    ++l;
  return !*l;
}

struct node *
protocols(struct run_context *run,
	  enum t_scope scope, enum block_levels level, 
	  unsigned char **lines, unsigned char ***end, struct node *np)
{
  struct node *p = elem(e_protocols,NULL,lnum,level);
  unsigned char savebuf[1024];

  appendAttr(p,attr(a_scope,ucc(strchr(scope_names[scope],'_')+1)));
  if (scope == s_text)
    {
      if (lines[0] && *lines[0] == '@')
	{
	  register unsigned char *s = lines[0];
	  unsigned char *token = &lines[0][1];
	  unsigned char save;
	  struct block_token *blocktokp;

	  if (need_lemm)
	    xstrcpy(savebuf,*lines);

	  while (*s && !isspace(*s))
	    ++s;
	  save = *s;
	  *s = '\0';
	  blocktokp = blocktok((const char *)token,xxstrlen(token));
	  if (blocktokp && blocktokp->type == TEXT)
	    {
	      if (need_lemm || do_show_insts)
		lem_save_line(savebuf);
	      doctype = blocktokp->etype;
	      setName(np,blocktokp->etype);
	      /* setAttr(np,a_implicit,ucc("0")); */
	      ++lnum;
	      ++lines;
	      if (doctype == e_composite || doctype == e_score)
		mylines = 1;
	      if (doctype == e_score)
		{
		  extern void load_labels_init(void);
		  extern int auto_lg;
		  parse_score_doctype(np, s+1);
		  symbolattr_init();
		  key_init();
		  load_labels_init();
		  check_links = 1;
		  auto_lg = 1;
		}
	    }
	  *s = save;
	}
    }
  while (lines[0] && (is_blank_line(lines[0]) || '#' == *lines[0]))
    {
      if (is_blank_line(lines[0]))
	{
	  ++lnum;
	  ++lines;
	  continue;
	}
      if (need_lemm)
	xstrcpy(savebuf,*lines);
      if (*lines[0] 
	  && xstrncmp(*lines,"#eid:",5) 
	  && !protocol(run, scope, level, p, *lines))
	break;
      else
	{
	  if (need_lemm || do_show_insts)
	    lem_save_line(savebuf);
	  ++lnum;
	  ++lines;
	}
    }
  *end = lines;
  return p;
}

static void
parse_score_doctype(struct node *np, unsigned char *line)
{
  int score_type = 0; /* 0 = synopsis; 1 = matrix */
  int score_mode = 0; /* 0 = parsed; 1 = unparsed */
  int score_word = 0; /* 0 = absent; 1 = present */

  /* parse type: matrix or synopsis */
  while (isspace(*line))
    ++line;
  if (!strncmp((char*)line, "synoptic", 8))
    line += 8;
  else if (!strncmp((char*)line, "matrix", 6))
    {
      score_type = 1;
      line += 6;
    }
  else
    {
      vwarning("bad score type at %s", line);
      return;
    }
  
  if (!isspace(*line))
    {
      vwarning("no space after score type %s", line);
      return;
    }
  
  /* parse mode: parsed or unparsed */
  while (isspace(*line))
    ++line;
  if (!strncmp((char*)line, "parsed", 6))
    line += 6;
  else if (!strncmp((char*)line, "unparsed", 8))
    {
      score_type = 1;
      line += 8;
    }
  else
    {
      vwarning("bad score mode at %s", line);
      return;
    }

  if (*line && !isspace(*line))
    {
      vwarning("no space or end-of-line after score mode %s", line);
      return;
    }

  /* optional 'word' token for matrix parsed */
  while (isspace(*line))
    ++line;
  if (score_type && !score_mode && !strncmp((char*)line, "word", 4))
    score_word = 1;
  else if (*line)
    {
      vwarning("junk at end of @score line at %s", line);
      return;
    }

  setAttr(np, a_score_type, (unsigned char *)(score_type ? "matrix" : "synopsis"));
  setAttr(np, a_score_mode, (unsigned char *)(score_mode ? "unparsed" : "parsed"));
  if (score_word)
    {
      setAttr(np, a_score_word, (unsigned char *)"yes");
      word_matrix = 1;
    }
}

#if 0
static const char *
project_of(const char *p)
{
  static char buf[128];
  while (isspace(*p))
    ++p;
  strcpy(buf,p);
  p = buf + strlen(buf);
  while (isspace(p[-1]))
    --p;
  return buf;
}
#endif

int
protocol(struct run_context *run,
	 enum t_scope scope, enum block_levels level,
	 struct node *parent, unsigned char *line)
{
  struct node *e;
  unsigned char *type = NULL, *s = ++line;
  while (*s && (islower(*s) || '.' == *s))
    ++s;
  if (*s == ':')
    {
      *s = '\0';
      type = line;
      line = s+1;
      while (*line && isspace(*line))
	++line;
    }
  if (type)
    {
      int suppress_output = 0;
      char *endp = (char*)(line + xxstrlen(line));
      struct keypair *kp = NULL;
      while (isspace(endp[-1]))
	--endp;
      if (*endp)
	*endp = '\0';
      if (!xstrcmp(type,"project"))
	{
#if 1
	  project_handler(run, parent, scope, level, line);
#else
	  if (run->proj && run->proj->xpd)
	    {
	      if (strcmp(run->proj->name, project_of(line)))
		{
		  vwarning("project already set to %s (move #project line up?)", run->proj->name);
		  project_handler(run, parent, scope, level, line);
		}
	    }
#endif
	}
      else
	{
	  if (!run->proj || !run->proj->xpd)
	    {
	      static char cdlibuf[5] = { 'c', 'd', 'l', 'i', '\0' };
	      project_handler(run, parent, scope, level, (unsigned char *)cdlibuf);
#if 0
	      vwarning("must give '#project' protocol before '#atf:' protocols");
	      return;
#endif
	    }

	  if (!xstrcmp(type,"atf"))
	    atf_handler(parent, scope, level, line);
	  else if (!xstrcmp(type,"version"))
	    version_handler(parent, scope, level, line);
	  else if (!xstrcmp(type,"lem"))
	    {
	      extern int already_lemmed;
	      suppress_output = 1;
	      if (need_lemm || do_show_insts)
		{
		  unsigned char *oline = NULL;
		  if (already_lemmed)
		    vwarning("multiple #lem: lines not allowed");
		  else
		    ++already_lemmed;
		  if (*line)
		    {
#if 0		      
		      if (line_is_unit)
			{
			  int found_ub = 0;
			  unsigned char *e = line + strlen(line);
			  while (e > line)
			    {
			      if ('.' == e[-1] && ('+' == e[-2] || '-' == e[-2]))
				{
				  found_ub = 1;
				  break;
				}
			      else if (';' == e[-1])
				break;
			      else
				--e;
			    }
			  if (!found_ub)
			    {
			      unsigned char *nline = malloc(strlen(line)+5);
			      sprintf(nline, "%s +.", line);
			      oline = line;
			      line = nline;
			    }
			}
#endif
		      lem_handler(parent, scope,level,line);
		      if (oline)
			{
			  free(line);
			  line = oline;
			}
		    }
		}
	    }
	  else if (!xstrcmp(type,"bib"))
	    {
	      ;
	    }
	  else if (!xstrcmp(type,"etcsri"))
	    {
	      lem_extended = 0;
	    }
	  else if (!xstrcmp(type,"etcsl"))
	    {
	      ;
	    }
	  else if (!xstrcmp(type,"note"))
	    {
	      /* this looks like a protocol but behaves like a comment */
	      *s = ':';
	      return 0;
	    }
	  else if (!xstrcmp(type,"key"))
	    {
	      key_init();
	      kp = key_parse(line);
	      if (doctype == e_score)
		{
		  if (!strcmp(kp->key, "siglum-map"))
		    {
		      char *arrow = strstr(kp->val, "=>");
		      if (arrow)
			{
			  char *from = malloc(strlen(kp->val)+1);
			  strcpy(from,kp->val);
			  from[arrow-kp->val] = '\0';
			  symbolattr_map(textid, from, arrow+2);
			  free(from);
			}
		      else
			{
			  warning("bad syntax in siglum-map, say, e.g., A1=>A");
			}
		    }
		}
	      if (!strcmp(kp->key, "after") || !strcmp(kp->key, "see"))
		{
		  type = (unsigned char*)kp->key;
		}
	    }
	  else if (!xstrcmp(type,"var") || !xstrcmp(type,"basket"))
	    {
	    }
	  else if (!xstrcmp(type,"link"))
	    {
	      link_check_protocol(cc(line));
	      ++has_links;
	    }
	  else if (!xstrcmp(type,"lemmatizer"))
	    lemmatizer_handler(parent, scope, level, line);
	  else if (!xstrcmp(type,"psu"))
	    {
	      ; /*ez_psu_store(line,get_state(s_text).lang);*/
	    }
	  else
	    {
	      vwarning("unknown protocol '%s'", type);
	      suppress_output = 1;
	    }
	}

      if (!suppress_output)
	{
	  e = appendChild(parent,elem(e_protocol,NULL,lnum,level));
	  setAttr(e, a_type, type);
	  if (kp && kp->url)
	    setAttr(e, a_url, (unsigned char*)kp->url);
	  appendChild(e,cdata(kp ? (unsigned char*)kp->val : line));
	}

      if (need_auto_syntax_line_is_unit)
	{
	  need_auto_syntax_line_is_unit = 0;
	  e = appendChild(parent,elem(e_protocol,NULL,lnum,level));
	  setAttr(e, a_type, ucc("syntax"));
	  appendChild(e,cdata(uc("line_is_unit")));
	  line_is_unit = 1;
	}
      return 1;
    }
  else
    return 0;
}

static void
atf_handler(struct node *parent, enum t_scope scope, 
	    enum block_levels level, unsigned char *l)
{
  if (!xstrncmp(l,"lang",4) && isspace(l[4]))
    {
      unsigned char *atsign, *uscore, *altlang = NULL;
      int uscore_logo = 0;
      l += 5;
      while (isspace(*l))
	++l;
      
      if ((uscore = (unsigned char *)strchr((char*)l,'_')))
	{
	  if (!strncmp((char*)uscore,"_logo",5))
	    {
	      unsigned char *save = uscore;
	      while (isspace(uscore[-1]))
		--uscore;
	      *uscore = '\0';
	      uscore_logo = 1;
	      uscore = save + 5;
	    }
	  else
	    ++uscore;
	  while (isspace(*uscore))
	    ++uscore;
	  if (*uscore)
	    {
	      altlang = uscore;
	      while (*uscore && !isspace(*uscore))
		++uscore;
	      if (*uscore)
		*uscore = '\0';
	    }
	}

      if ((atsign = uc(strchr(cc(l),'@'))))
	{
	  warning("specify dialect as in ATF, i.e., say 'nb' not 'akk @NB'\n"
		  "\tSee http://oracc.museum.upenn.edu/doc/builder/XXX");
	}
      if (*l)
	{
	  if (!(text_lang = lang_switch(NULL, (char*)l, NULL, file, lnum)))
	    curr_lang = text_lang = global_lang;
	  if (altlang)
	    {
	      struct lang_context *tmp = lang_switch(NULL,(const char*)altlang,NULL,file,lnum);
	      if (tmp)
		text_lang->altlang = (char*)altlang;
	    }
	  if (uscore_logo)
	    curr_lang->underscore = m_logo;
	}
      else
	warning("atf protocol without argument");
    }
  else if (!xstrncmp(l,"use",3))
    {
      l+=4;
      while (isspace(*l))
	++l;
      if (!xstrncmp(l,"lexical",7))
	{
	  need_auto_syntax_line_is_unit = mylines = lexical = 1;
	  default_ftype = ucc("wp");
	}
      else if (!xstrncmp(l,"bilingual", 9))
	{
	  extern const unsigned char *default_ftype;
	  default_ftype = ucc("mn");
	  bilingual = 1;
	}
      else if (!xstrncmp(l,"mylines",7))
	mylines = 1;
      else if (!xstrncmp(l,"alignment-groups",15))
	agroups = 1;
      else if (!xstrncmp(l,"math",4))
	math_mode = 1;
      else if (!xstrncmp(l,"unicode",7))
	use_unicode = 1;
      else if (!xstrncmp(l,"lemconv",7))
	use_ilem_conv = 1;
      else if (!xstrncmp(l,"legacy",7))
	use_legacy = 1;
      else if (!xstrncmp(l,"cuneiform",9))
	do_cuneify = 1;
      else
	vwarning("bad atf use protocol: %s", l);
    }
  else if (!xstrncmp(l,"script", 6) && isspace(l[6]))
    vwarning("'#atf: script' is no longer supported; use '#atf: lang' instead");
  else
    vwarning("bad atf protocol: %s", l);
}

extern unsigned char *unicodify(register const unsigned char *s);

/*FIXME: should be dynamic*/
#define MAXLEM 1024
static void
lem_handler(struct node *parent, enum t_scope scope, 
	    enum block_levels level, unsigned char *l)
{
  unsigned char *lem[MAXLEM];
  struct node *wline;
  int nlem,j;

  xcl_set_sparse_skipping(NULL);
  wline = lastChild(parent);
  if (wline && !xstrcmp(wline->names->pname,"lg"))
    {
      int i;
      for (i = 0; i < wline->children.lastused; ++i)
	if (!xstrcmp(getAttr(wline->children.nodes[i],"type"),"nts"))
	  break;
      if (i < wline->children.lastused)
	wline = wline->children.nodes[i];
      else
	wline = wline->children.nodes[0];
   }
  if (!wline)
    {
      warning("no words to host lemmata");
      return;
    }
  for (nlem = 0; *l && nlem < MAXLEM; ++nlem)
    {
      lem[nlem] = l;
      while (*l && (*l != ';' || ('+' == l[-1] || '-' == l[-1] 
				  || (l[1] && !isspace(l[1])))))
	++l;
      if (';' == *l)
	{
	  *l++ = '\0';
	  while (isspace(*l))
	    ++l;
	  if ('\0' == *l)
	    warning("spurious ';' at end of #lem: line");
	  else if (';' == *l)
	    warning("empty lemmatization in #lem: line");
	}
    }
  lem[nlem] = NULL;
  j = lemwords(0,wline,lem);
  if (j != nlem)
    {
      if (j < nlem)
	warning("too many lemmata");
      else
	warning("too few lemmata");
    }
}

static int
lemwords(int j, struct node *parent, unsigned char**lem)
{
  register int i;
  for (i = 0; i < nline_words; ++i)
    {
      struct node *cp = line_words[i];
      struct node *parent = cp->parent;
#if 1
      if (sparse_lem && *parent->type == 'e' && !xstrcmp(parent->names->pname,"f"))
	xcl_set_sparse_skipping(((struct attr*)parent->attr.nodes[0])->valpair[1]);
#else
      if (sparse_lem && *cp->type == 'e' && !xstrcmp(cp->names->pname,"f"))
	{
	  char field_buf[5];
	  const unsigned char *f = getAttr(cp,"type");
	  field_buf[1] = f[0]; field_buf[2] = f[1];
	  field_buf[0] = field_buf[3] = ' ';
	  field_buf[4] = '\0';
	  if (strstr(sparse_lem_fields,field_buf))
	    sparse_skipping = 0;
	  else
	    sparse_skipping = 1;
	}
#endif
      if (!sparse_skipping)
	{
	  if (lem[j])
	    lem_save_lemma(cp,(const char *)lem[j++]);
	  else
	    return j+1;
	}
    }
  return j;
}

static void
lemmatizer_handler(struct node *parent, enum t_scope scope, 
		   enum block_levels level, unsigned char *l)
{
  extern unsigned int lem_stop_lnum;
  if (!xstrncmp(l, "sparse", 6) && isspace(l[6]))
    {
      l += 7;
      while (isspace(*l))
	++l;
      if (!xstrncmp(l,"do",2) && isspace(l[2]))
	{
	  l += 3;
	  while (isspace(*l))
	    ++l;
	  xcl_set_sparse_fields((char *)l);
	}
      else
	vwarning("bad sparse protocol: %s", l);
    }
  else if (!xstrcmp(l,"stop"))
    {
      lem_stop_lnum = lnum;
      fprintf(stderr,"lem_stop_lnum = %u\n",lem_stop_lnum);
    }
  else
    vwarning("bad protocol: %s", l);
}

static void
project_handler(struct run_context *run, struct node *parent, enum t_scope scope, 
		enum block_levels level, unsigned char *l)
{
  extern const char *project, *system_project;
  const char *o = NULL;

  if (!project || !*project)
    {
      proj_init(run, (char*)l);
      project = (const char*)npool_copy(l, run->pool);
    }

  /*xpd_init(project, run->pool);*/
  /*system_project = (char*)xpd_option("atf-lem-system");*/
  /*lem_use_defsense = xpd_option_int("lem-default-sense");*/

  cuneify_init(run->proj->xpd);

  lem_simplify_opt = xpd_option(run->proj->xpd,"lem-simplify");
  if (lem_simplify_opt && !strcmp(lem_simplify_opt, "yes"))
    lem_simplify = 1;
  lem_props_strict_opt = xpd_option(run->proj->xpd,"lem-props-strict");
  if (lem_props_strict_opt && !strcmp(lem_props_strict_opt, "yes"))
    lem_props_strict = 1;
  
  o = xpd_option(run->proj->xpd,"trans-dollar-align");
  if (o && !strcmp(o, "yes"))
    dollar_fifo = mylines = 1;
  else
    dollar_fifo = 0;

  /* FIXME: ALL THESE THINGS HAVE TO GO NOW THAT ATF2XTF READS lib/config.xml
     NO, NOT THAT SIMPLE: texts should be correctly parsed without config.xml
   */

  if (!xstrncmp(l,"oatc",4))
    mixed_case_ok = 1;
  else if (!xstrncmp(l,"cams",4))
    {
      mylines = math_mode = saa_mode = 1;
      /* lemmatizer_mode = lem_project_and_global; */
      /*set_uscore_mode("akk",m_lang);*/
    }
  else if (!xstrncmp(l,"dccmt",5))
    {
      mylines = math_mode = saa_mode = 1;
      /*lemmatizer_mode = lem_project_only; */
    }
  else if (!xstrncmp(l,"dcclt",5))
    {
      need_auto_syntax_line_is_unit = lexical = mylines = math_mode = 1;
      default_ftype = ucc("wp");
      /*lemmatizer_mode = lem_global_only;*/
      /* lem_extended = 1; */
    }
  else if (!xstrncmp(l,"hbtin",5) || !xstrncmp(l,"test",4))
    {
      saa_mode = 1;
    }
  else if (!xstrncmp(l,"saa",3))
    {
      extern int trans_parenned_labels, trans_abbrevved_labels;
      do_cuneify = dollar_fifo = mylines = saa_mode = 1;
      if (!system_project)
	{
	  system_project = /* "natclemm" */ "saa";
	  /* shadow_lem = 1; */
	}
      trans_abbrevved_labels = 1;
      trans_parenned_labels = 0;
      /*curr_dialect = "NA";*/
    }
  else if (!xstrncmp(l,"rinap",5) || !xstrncmp(l,"cmawro",6))
    {
      /*system_project = "saa";*/
      /*shadow_lem = 1;*/
      /* fprintf(stderr, "%s triggers dollar_fifo=1\n", l); */
      dollar_fifo = mylines = 1;
    }
  else if (!xstrncmp(l,"pfa",3))
    mylines = 1;
  else if (!xstrncmp(l,"penn",4))
    system_project = "hbtin";
  else if (!xstrncmp(l,"epsd",4))
    lem_extended = 1; /* do_cuneify =  */
}

static void
version_handler(struct node *parent, enum t_scope scope, 
		enum block_levels level, unsigned char *l)
{
  ;
}

#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <stdio.h>
#include "globals.h"
#include "l2.h"
#include "tree.h"
#include "xtfxcl.h"
#include "fname.h"
#include "protocols.h"
#include "block.h"
#include "warning.h"
#include "bit_mac.h"
#include "rnvif.h"
/*#include "file.h"*/
#include "translate.h"
#include "labtab.h"
#include "text.h"
#include "nonx.h"
#include "lemline.h"
#include "lang.h"
#include "label.h"
#include "charsets.h"
#include "inline.h"
#include "ilem.h"
#include "gdlopts.h"
#include "run.h"
#include "proj_context.h"


#define QVEC_BYTES	5000
#define QVEC_SIZ	(QVEC_BYTES*8)

const char *const xtf_xmlns[] =
  {
    "xmlns",      "http://oracc.org/ns/xtf/1.0",
    "xmlns:x",    "http://oracc.org/ns/xtf/1.0",
    "xmlns:g",    "http://oracc.org/ns/gdl/1.0",
    "xmlns:n",    "http://oracc.org/ns/norm/1.0",
    "xmlns:note", "http://oracc.org/ns/note/1.0",
    "xmlns:syn",  "http://oracc.org/ns/syntax/1.0",
    "xmlns:xh",   "http://www.w3.org/1999/xhtml",
    "", ""
  };

extern const unsigned char *catvec, *curr_dialect;
extern size_t max_cat_id;

char text_idbuf[16];

extern int do_cdl;
extern int harvest_notices;
extern int lem_autolem, lem_do_wrapup, lem_extended, 
  lem_harvest_notices, lem_forms_raw, lem_standalone;
extern int lem_system;
extern int named_ents, no_destfile;
extern int ods_mode, perform_lem, perform_dsa, perform_nsa;
extern int use_unicode, use_legacy;
extern int verbose, warn_unlemmatized;
extern FILE*f_log;
extern FILE*f_xml;
enum e_type doctype; /* needs per-text initialization */
extern enum block_levels current_level;
/*unsigned char *text_n = NULL;*/
static int text_n_alloced = 0;

int has_links;
int header_id;
int line_id;
int next_trans_id = 0;
int sym_warning = 0;
extern char line_id_buf[MAX_LINE_ID_BUF+1];
char *line_id_insertp;
struct labtab *labtab;

static char pref[5];
static unsigned char pvec[125000];
static unsigned char qvec[QVEC_BYTES];
extern FILE *f_bad;
extern FILE *f_good;
extern int check_only;
extern int force;
extern int no_xcl_map;
extern const char *project;
extern int check_pnames;
extern int sparse_lem;
extern int validate;
extern void bigrams(struct xcl_context *xc);
extern int check_pname(const char *id,const unsigned char *name);
extern void cdl_create(struct node *text);
extern void psus(struct xcl_context *xc);
extern void reset_forms_insertp(void);
extern void reset_lninfo(void);
static void andline(struct node *np,unsigned char *line);

static void serialize_bad_atf(unsigned char *atf, int len);
static unsigned char *savetext(unsigned char **l, int *lenp);

extern struct xcl_context *text_xc;
struct nsa_context *global_nsa_context;
static void
destfile_pi()
{
  strncpy(pref,textid,4);
  if (output_dir)
    fprintf(f_xml,"<?destfile %s/%s.xtf?>",output_dir,textid);
  else
    {
      const char *oracc = cdl_home();
      fprintf(f_xml,"<?destfile %s/%s/01bld/%s/%s/%s.xtf?>",
	      oracc,project,pref,textid,textid);
    }
}
static void
destfile_pi_tr(const char *code,const char *lang)
{
  strncpy(pref,textid,4);
  if (output_dir)
    fprintf(f_xml,"<?desfile %s/%s_%s-%s.xtr?>",output_dir,textid,code,lang);
  else
    {
      const char *oracc = cdl_home();
      fprintf(f_xml,"<?destfile %s/%s/01bld/%s/%s/%s_%s-%s.xtr?>",
	      oracc,project,pref,textid,textid,code,lang);
    }
}

void
text_init()
{
  textid = (char*)text_idbuf;
}

void
text_term(void)
{
  free(text_n);
  text_n = NULL;
  text_n_alloced = 0;
  savetext(NULL,NULL);
}

void
text_vec_init(void)
{
  memset(pvec,'\0',125000);
  memset(qvec,'\0',400);
}

unsigned char **
skip_rest_of_text(unsigned char **l)
{
  while (l && *l)
    {
      if ('&' == l[0][0])
	{
	  break;
	}
      else
	{
	  ++l;
	  ++lnum;
	}
    }
  return l;
}

unsigned char **
process_text(struct run_context *run, unsigned char **lines)
{
  struct node *text = elem(e_transliteration,NULL,lnum,TEXT);
  int saved_status = status;
  unsigned char *saved_atf = NULL;
  int saved_len = 0;
  int saved_check_only = check_only;

  word_matrix = 0;
  labtab = create_labtab();
  text->xmlns = xtf_xmlns;
  doctype = e_transliteration;
  div_level = sparse_lem = status = 0;
  current_level = TEXT;
  curr_lang = text_lang = global_lang;
  protocol_state = s_text;
  has_links = header_id = line_id = sym_warning = use_unicode = use_legacy = 0;
  curr_dialect = NULL;
  reset_lninfo();
  reset_forms_insertp();
  init_trans();
  texttag_init();
  text_init();
  lemline_init();
  note_initialize_text();

  phase = NULL;

  label_segtab(NULL,NULL);

  /* if we never found an &-line */
  if (*lines == NULL)
    return lines;

  if (!check_only)
    saved_atf = savetext(lines, &saved_len);

  if (need_lemm)
    lem_save_line(*lines);

  andline(text,*lines);

  if (text)
    {
      struct node *text_protocols;
      ++lnum;
      text_protocols = protocols(run, s_text, TEXT, lines+1, &lines, text);
      --lnum;

      if (!run->proj || !run->proj->xpd)
	{
	  if (fallback_project)
	    {
	      proj_init(run, fallback_project);
	      project = (const char*)npool_copy(fallback_project, run->pool);
	    }
	  else
	    {
#if 1
	      vnotice("ox: no known #project: set so far--proceeding with default project 'oracc'");
	      proj_default(run);
#else
	      lines = skip_rest_of_text(lines);
	      return lines;
#endif
	    }
	}

#if 0
      /* if we do the getAttr the xtf:transliteration tag always has xml:lang=sux */
      if (!*(getAttr(text,"xml:lang")))
	setAttr(text,a_xml_lang,ucc(curr_lang->fulltag));
#else
      if (text_lang && text_lang->fulltag)
	setAttr(text,a_xml_lang,ucc(text_lang->fulltag));
#endif	
#if 0
      appendAttr(text,attr(a_hand,ucc(current_state.hand)));
#endif
      if (text_protocols && text_protocols->children.lastused)
	appendChild(text,text_protocols);
      if (!project || !*project)
	{
	  project = "cdli";
	  proj_init(run,project);
	}
      if (project)
	setAttr(text,a_project,ucc(project));
      /* Don't do this until after text protocols have had a chance to set project */
      if (need_lemm)
	{
	  lem_text_init(textid);
	  proj_init(run, project);
	}

      lines = parse_block(run, text, lines);
      max_cells_attr(text);
      if (has_links)
	setAttr(text,a_haslinks,ucc("1"));
      if (!status && validate)
	{
	  rnvif_text_id = textid;
	  status = rnv_validate(text);
	}
      if (do_cdl)
	text_xc = xcl_process(run, text);

      if (need_lemm && !no_xcl_map)
	{
	  int pre_lem_status = status; /* lem errors don't suppress output */
	  if (text_xc->root)
	    {
	      struct sig_context *scp = NULL;
	      if (!text_xc->linkbase)
		{
		  text_xc->linkbase = new_linkbase();
		  text_xc->linkbase->textid = text_xc->textid;
		}

	      scp = text_xc->sigs;
	      scp->xpd = run->proj->xpd;
	      scp->xcp = text_xc;
	      sig_context_langs(scp,text_xc->langs);

	      if (lem_autolem)
		sig_new(text_xc);
	      else
		sig_check(text_xc);

	      if (lem_autolem && !lem_forms_raw)
		bigrams(text_xc);

	      if (lem_extended)
		xcl_map(text_xc,NULL,NULL,NULL,xli_mapper);

	      xcl_map(text_xc,xcl_sentence_labels,NULL,NULL,NULL);

	      /* PSU matching must take place while the lists of
		 candidate matches are still available to prevent
		 match-failures or mismatches like mar shipri
		 matching shipru[//work] */
	      if (!lem_autolem)
		psus2(text_xc);

	      if (lem_do_wrapup)
		xcl_map(text_xc,NULL,NULL,NULL,ilem_wrapup);

#if 0
	      /* Previously, PSUs have had a disambiguating function, 
		 but if we place them after the ilem_wrapup this is
		 no longer true.  The alternative is that we must 
		 defer psu sig generation, moving it from psu processing
		 to post-ilem_wrapup somehow */

#endif

	      /* from this point on the lemmatization is determined and
		 ->finds must always be NULL; this means that no further
		 disambiguation is performed by the recognizers that
		 follow */

	      if (named_ents)
		psa_exec(text_xc);

	      if (perform_dsa)
		dsa_exec(text_xc);

	      if (perform_nsa)
		nsa_xcl_input(text_xc, global_nsa_context);

	      if (harvest_notices)
		xcl_map(text_xc,NULL,NULL,NULL,ilem_harvest_notices);

	      if (warn_unlemmatized)
		xcl_map(text_xc,NULL,NULL,NULL,ilem_unlemm_warnings);

	      xcl_chunkify(text_xc);
	    }
	  status = pre_lem_status;
	}
      if (!check_only && !perform_lem && (!status || force))
	{
	  if (!no_destfile)
	    destfile_pi();
	  serialize(text,do_cdl);
	  if (has_trans())
	    {
	      int i,t;	      
	      for (i=0,t=has_trans(); i < t; ++i)
		{
		  struct translation *trans = translations[i];

		  trans_cols_attr(trans->tree);
		  trans_finish_labels(text, trans);
		  
		  if (validate)
		    status = rnv_validate(trans->tree);
		  if (trans->etype == etu_interlinear)
		    trans_mapdump(trans->tree);
		  if (!no_destfile)
		    destfile_pi_tr(trans->code,trans->lang);
		  serialize(trans->tree,0);
		}
	    }
	}

      if (perform_lem && !exit_status)
	{
	  if (ods_mode)
	    {
	      extern char *odslem_fn;
	      FILE *odslem_fp = NULL;
	      odslem_fp = xfopen(odslem_fn,"wb");
	      lem_ods_serialize(odslem_fp);
	      xfclose(odslem_fn,odslem_fp);
	    }
	  else
	    lem_serialize(stdout);
	}

      if (has_trans())
	{ 
	  if (translations[0]->etype == etu_interlinear)
	    trans_clear();
	  if (dollar_fifo)
	    {
	      if (dollar_clear())
		notice("detected misalignment in translit/translat $-lines");
	    }
	}
      else
	(void)dollar_clear();

      if (status && f_bad)
	  fprintf(f_bad,"%s\n",textid);
      else if (f_good)
	fprintf(f_good,"%s\n",textid);

      if (status && !check_only && !force)
	{
	  if (!no_destfile)
	    destfile_pi();
	  atf_lnum_pi(text);
	  serialize_bad_atf(saved_atf,saved_len);
	}

      if (status)
	exit_status = status;

      status = saved_status;
      check_only = saved_check_only;

      if (perform_nsa)
	{
	  nsa_del_result(text_xc->user);
	  text_xc->user = NULL;
	}

      xcl_destroy(&text_xc);
      lem_text_term();
      destroy_labtab(labtab);

      labtab = NULL;
    }
#if 0 /* I don't believe this can happen any more */
  else
    {
      /* this happens if the text's document element can't be created
	 or the ID number was a duplicate */
      ++lnum;
      ++lines;
      while (*lines && '&' != **lines)
	{
	  ++lnum;
	  ++lines;
	}
    }
#endif

  lemline_term();
  return lines;
}

int
check_and_register(const char *id, int set_ok)
{
  unsigned char *vec;
  unsigned int vmax;
  long idnum = strtoul(id+1,NULL,10);
  unsigned int v, b;
  int tab[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

  /* The vector is created by Perl's vec() function; perldoc -f vec for more info */
  v = idnum/8;
  b = tab[idnum%8];

  if (*id == 'P')
    {
      vec = pvec;
      vmax = 1000000;
    }
  else if (*id == 'Q')
    {
      vec = qvec;
      vmax = QVEC_SIZ;
    }
  else
    return 0;

  if (idnum < vmax && set_ok)
    {
      if (BIT_ISSET(vec[v],b))
	{
	  vwarning("duplicate ID: %s", id);
	  return 1;
	}
      else
	{
	  BIT_SET(vec[v],b);
	}
    }
#if 1
  /* issue this as a notice so it doesn't affect error status */
  if (*id == 'P' && (idnum > max_cat_id || !(BIT_ISSET(catvec[v],b))))
    {
      if (strlen(id) > 7)
	{
	  vwarning("%s: malformed ID; text will be ignored",id);
	  return 1;
	}
      else
	vnotice("%s not in main catalog (new P-IDs may not register for 48 hours)",id);
    }
#else
  else
    {
      vwarning("%s: ID too big (max %d)",id,vmax);
      return 1;
    }
#endif
  return 0;
}

static void
andline(struct node *lnode, unsigned char *lp)
{
  char *tok;
  unsigned char *end = lp+xxstrlen(lp);
  unsigned char *s = lp+1;
  char *idbase = NULL;
  int equals_flag = 0;

  next_trans_id = 0;

  if (!lnode)
    return;

  curr_lang = global_lang;

  while (*s && isspace(*s))
    ++s;
  if (isupper(*s))
    {
      tok = (char*)s++;
      while (isdigit(*s))
	++s;
#if 0
      /* this is not presently permitted in IDs, but we have talked
	 about enabling it from time to time */
      while (islower(*s))
	++s;
#endif
      if (!*s)
	idbase = tok;
      else if (isspace(*s))
	{
	  *s++ = '\0';
	  idbase = tok;
	}
      else if ('=' == *s)
	{
	  *s++ = '\0';
	  equals_flag = 1;
	  idbase = tok;
	}
      else
	{
	  unsigned char save = *s;
	  *s = '\0';
	  vwarning("malformed &-ID beginning %s", tok);
	  *s = save;
	}
    }
  else
    {
      vwarning("malformed &-ID: %s", s);
    }

  if (!idbase)
    {
      static int id = 0;
      static char Xbuf[8];
      sprintf(Xbuf,"X%06d",++id);
      textid = idbase = Xbuf;
    }
  else
    xstrcpy(textid,idbase);

  if (!check_and_register(idbase, 1))
    {
      xstrcpy(line_id_buf,idbase);
      xstrcat(line_id_buf,".");
      line_id_insertp = line_id_buf + xxstrlen(line_id_buf);
      setAttr(lnode,a_xml_id,ucc(idbase));
      tok = NULL;
      if (s < end)
	{
	  if (!equals_flag)
	    while (*s && '=' != *s)
	      ++s;
	  if ('=' == *s)
	    {
	      *s++ = '=';
	      ++equals_flag;
	    }
	  if (!equals_flag)
	    warning("expected '=' in and-line");
	  else
	    {
	      while (*s && isspace(*s))
		++s;
	      if (*s)
		{
		  tok = (char*)s;
		  s = end;
		  while (isspace(s[-1]))
		    --s;
		  *s = '\0';
		}
	    }
	}
      else
	warning("no text name in and-line");
      /* if there was no '= NAME' in the andline we fix up by using the IDBASE
	 and process the text to find further errors; because the warnings set
	 status the text will not be output */
      if (!tok)
	tok = idbase;
      if (check_pnames && 'P' == *textid)
	(void)check_pname(textid,ucc(tok));
      while (xxstrlen(tok)+1 > text_n_alloced)
	{
	  text_n_alloced = text_n_alloced ? text_n_alloced * 2 : 128;
	  text_n = realloc(text_n,text_n_alloced);
	}
      xstrcpy(text_n,tok);
      setAttr(lnode,a_n,ucc(tok));
      if (verbose)
	fprintf(f_log, "%s = %s\n",textid,tok);
    }
  else
    {
      if (!idbase)
	idbase = "X123456";
      xstrcpy(line_id_buf,idbase);
      xstrcat(line_id_buf,".");
      line_id_insertp = line_id_buf + xxstrlen(line_id_buf);
      setAttr(lnode,a_xml_id,ucc(idbase));
      tok = NULL;
      if (!tok)
	tok = idbase;
      if (check_pnames && 'P' == *textid)
	(void)check_pname(textid,ucc(tok));
      if (xxstrlen(tok)+1 > text_n_alloced)
	{
	  text_n_alloced = text_n_alloced ? text_n_alloced * 2 : 128;
	  text_n = realloc(text_n,text_n_alloced);
	}
      xstrcpy(text_n,tok);
      setAttr(lnode,a_n,ucc(tok));
      if (verbose)
	fprintf(f_log, "%s = %s\n",textid,tok);
    }
}

static unsigned char *
savetext(unsigned char **l, int *lenp)
{
  unsigned char *start;
  int len = 0;
  static unsigned char *buf = NULL;
  static int buflen = 0;
  if (!l)
    {
      free(buf);
      return buf = NULL;
    }
  start = *l;
  ++l;
  while (*l && '&' != l[0][0])
    ++l;
  if (!*l)
    {
      --l;
      len = &l[0][strlen((const char*)*l)] - start;
      ++len;
    }
  else
    len = l[0]-start;
  if (len > buflen)
    {
      while (buflen < len)
	buflen += 8192;
      buf = realloc(buf,buflen);
    }
  memcpy(buf,start,len);
  *lenp = len;
  return buf;
}

static void
serialize_bad_atf(unsigned char *atf, int len)
{
  int i;
  fprintf(f_xml,"<atf xml:id=\"%s\">",textid);
  for (i = 0; i < len; ++i)
    {
      switch (atf[i])
	{
	case '<':
	  fputs("&lt;",f_xml);
	  break;
	case '&':
	  fputs("&amp;",f_xml);
	  break;
	case '\0':
	  fputc('\n',f_xml);
	  break;
	default:
	  fputc(atf[i],f_xml);
	  break;
	}
    }
  fputs("</atf>",f_xml);
}

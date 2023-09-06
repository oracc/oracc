#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include "cdf.h"
#include "gdl.h"
#include "nonx.h"
#include "text.h"
#include "translate.h"
#include "tree.h"
#include "warning.h"
#include "xmlnames.h"
#include "labtab.h"
#include "label.h"
#include "pool.h"
#include "note.h"
#include "globals.h"

extern int note_index;
extern void note_initialize_line(void);

Hash_table *trans_hash = NULL;

extern int need_lemm;
const char *const xtr_xmlns[] =
  {
    "xmlns:note","http://oracc.org/ns/note/1.0",
    "xmlns:xtr","http://oracc.org/ns/xtr/1.0",
    "xmlns:xh", "http://www.w3.org/1999/xhtml",
    "","",
  };

#define xctr(p) if (p && !*(getAttr(p,a_class))) appendAttr((p),attr(a_class,ucc("tr")))
#define yctr(p) appendAttr((p),attr(a_class,ucc("tr")))
#define ctr(p) setAttr((p),a_class,ucc("tr"))
#define setClass(n,c) appendAttr((n),attr(a_class,ucc((c))))
#define getClass(n) getAttr((n),"class")

static char *tr_id_buf = NULL;

static char trans_id_base[32];
static unsigned char last_xid[128];
int start_lnum = -1;
extern int next_trans_id;
static int next_trans_p_id;
extern int saa_mode;
extern unsigned const char *curr_line_label;

static int need_dir_rtl = 0;

static int multi_trans_line;
static char trans_p_idbuf[128];
static int trans_wid;

int max_trans_cols = 1;
int need_alignment = 1;
int nocellspan = 0;
int trans_parenned_labels = 1;
int trans_abbrevved_labels = 0;

static int in_note = 0, last_label = 0;
struct translation **translations = NULL;
struct translation *current_trans;

static struct node **last_trans_h = NULL, *last_p;
static int lth_alloced = 0;
static int lth_used = 0;
extern const unsigned char *nth_tlit_hdr(const unsigned char *ref,int nth);

extern void lem_save_line(const char *s);

static void add_trans(struct translation *t);
static void h_arefs(const unsigned char *aref);
static int is_blank_line(unsigned char *l);
static int looks_like_lnum(const unsigned char *s);
static void mapentry(const char *lid,const char*tid);
static int refattrs(struct node *elem,unsigned char *labelp,char *id);
static unsigned char *trans_rest_of_line(unsigned char *s);
static void trwords(struct node *p, unsigned char *s);
static void trwords_w(struct node *p, unsigned char *s);
static void textwords(struct node *p, unsigned char *start, unsigned char *end);

static struct translation *itrans = NULL;
static unsigned char last_label_buf[128];

static const char *xid_prev(const char *x);
static int xid_line(const char *x);
static int xid_diff(const char *x1, const char *x2);

void
set_tr_id(const char *id)
{
  if (id)
    {
      tr_id_buf = realloc(tr_id_buf, strlen(id) + 1);
      strcpy(tr_id_buf, id);
    }
  else
    {
      if (tr_id_buf)
	{
	  free(tr_id_buf);
	  tr_id_buf = NULL;
	}
    }
}

struct node *
translation(unsigned char **lines,struct node*text,enum e_tu_types *transtype)
{
  struct translation*trans = calloc(1,sizeof(struct translation));
  unsigned char *s = &lines[0][13];
  int saved_status = status;
  struct node *dummytext;

  *last_label_buf = '\0';
  *last_xid = '\0';
  current_trans = trans;
  status = 0;
  last_label = 0;
  trans_wid = 0;

  if (dollar_fifo)
    dollar_reset();

  /* trans_inter does not call the translation() routine, which is good
     because we don't want to reinitialize the line notes in interlinear
     translations */
  note_initialize_line();
  note_index = 1;

  if (saa_mode)
    need_alignment = 0;
  else
    need_alignment = 1;

  while (isspace(*s))
    ++s;
  trans->type = cc(s);
  while (*s && !isspace(*s))
    ++s;
  if (*s)
    {
      *s++ = '\0';
      while (isspace(*s))
	++s;
      if (*s)
	{
	  trans->lang = cc(s);
	  while (*s && !isspace(*s))
	    ++s;
	  if (*s)
	    *s++ = '\0';
	  while (isspace(*s))
	    ++s;
	  if (*s)
	    {
	      trans->code = cc(s);
	      while (isalnum(*s))
		++s;
	      if (*s && !isspace(*s))
		{
		  warning("code is limited to letters and digits");
		  trans->code = NULL;
		}
	      else
		if (*s)
		  *s = '\0';
	    }
	}
    }
  /* validate the @translation line */
  if (trans->type && trans->lang && trans->code)
    {
      struct trans_token *tt = transtok(trans->type,xxstrlen(trans->type));
      extern int mylines;
      if (tt && (tt->type == etu_parallel 
		 || tt->type == etu_labeled 
		 || tt->type == etu_unitary))
	{
	  unsigned char *id;
	  
	  trans->etype = tt->type;
	  trans->tree = elem(e_xtr_translation,NULL,lnum,TEXT);
	  trans->tree->xmlns = xtr_xmlns;
	  trans->id = id = uc(getAttr(text,"xml:id"));
	  appendAttr(trans->tree,attr(a_ref,id));
	  appendAttr(trans->tree,attr(a_n, getAttr(text,"n")));
	  appendAttr(trans->tree,attr(a_project, getAttr(text,"project")));
	  appendAttr(trans->tree,attr(a_xml_lang,ucc(trans->lang)));
	  if (langrtl(trans->lang,strlen(trans->lang)))
	    {
	      need_dir_rtl = 1;
	      appendAttr(trans->tree,attr(a_dir,ucc("rtl")));
	    }
	  else
	    need_dir_rtl = 0;
	  appendAttr(trans->tree,attr(a_xtr_type,ucc(trans->type)));
	  appendAttr(trans->tree,attr(a_xtr_code,ucc(trans->code)));
	  sprintf(trans_id_base,"%s_%s-%s",textid,trans->code,trans->lang);
	  if (trans_hash_add(trans_id_base))
	    {
	      warning("duplicate @translation will be ignored");
	      return NULL;
	    }
	  appendAttr(trans->tree,attr(a_xml_id,ucc(trans_id_base)));
	  next_trans_p_id = 0;
	  (void)refattrs(NULL,NULL,NULL);
	  mapentry(NULL,NULL);
	  /* lines = trans_block(lines+1,trans); */
	  /* fixup_refs(trans,text) */
	  add_trans(trans);
	  /* trans_mapdump(trans->tree); */
	  *transtype = tt->type;
	}
      else
	{
	  vwarning("%s is not a legal translation type",trans->type);
	  return NULL;
	}
      /* Catch the user if labeled translations are being used without mylines */
      if ((tt->type == etu_labeled || tt->type == etu_parallel) && !mylines)
	notice("#atf: use mylines recommended with labeled/parallel translations");
    }
  else
    {
      warning("@translation must give all of type/lang/code");
      return NULL;
    }

  status = saved_status;
  dummytext = elem(e_transliteration,NULL,lnum,TEXT);
  return dummytext;
}

static void
se_label(struct node *np, const char *n, const char *l)
{
  char buf[1024];
  sprintf(buf,"%s, %s", n, l);
  setAttr(np,a_xtr_se_label,(unsigned char *)buf);
}

static int
int_of(unsigned const char *line_id)
{
  unsigned const char *s = line_id + xxstrlen(line_id);
  while (s > line_id && s[-1] != '.')
    --s;
  return atoi(cc(s));
}

static unsigned char *
label_prefix(unsigned char *lab)
{
  static unsigned char buf[128], *b;
  xstrcpy(buf,lab);
  b = buf+strlen(cc(buf));
  while (b > buf && !isspace(b[-1]))
    --b;
  if (b > buf)
    {
      while (isspace(b[-1]))
	--b;
      b[1] = '\0';
      return buf;
    }
  else
    return NULL;
}

static unsigned char *
lnum_of(unsigned char *l)
{
  static unsigned char buf[128];
  unsigned char *l_end;
  l_end = l+strlen((char*)l);
  while (l_end > l && !isspace(l_end[-1]))
    --l_end;
  strcpy((char*)buf,(char*)l_end);
  return buf;
}

static void
labeled_labels(struct node *p, unsigned char *lab)
{
  unsigned char *s = lab, *disp = lab, *eref = NULL;
  const unsigned char *xid = NULL;
  int sref = 0, overlap = 0;
  unsigned const char *sref_xid = NULL;
  int saved_start_lnum = start_lnum;
  
  while (*disp)
    {
      if (isspace(*disp) && disp[1] == '=' && isspace(disp[2]))
	break;
      else
	++disp;
    }
  if (*disp)
    {
      unsigned char *end = disp, save = 0;
      while (end > lab && isspace(end[-1]))
	--end;
      *end = '\0';
      disp += 3;
      while (isspace(*disp))
	++disp;
      end = disp;
      while (*end && !isspace(*end))
	++end;
      if (isspace(*end))
	{
	  save = *end;
	  *end = '\0';
	}
      setAttr(p,a_xtr_lab_start_label,pool_copy(disp));
      setAttr(p,a_xtr_lab_start_lnum,pool_copy(lnum_of(disp)));
      if (save)
	*end = save;
      s = end;
    }

  while (*s && (*s != '-' || (s == lab || !isspace(s[-1]) || !isspace(s[1]))))
    ++s;

  if (*s)
    {
      eref = s+1;
      while (isspace(s[-1]))
	--s;
      *s = '\0';
    }

  start_lnum = p->lnum;
  xid = check_label(lab,etu_labeled,NULL);
  start_lnum = saved_start_lnum;

  if (xid)
    {
      sref_xid = xid;
      sref = int_of(xid);
      if (*last_xid && !strcmp((const char*)last_xid, (const char *)xid))
	overlap = 1;
    repeat:
      if (dollar_fifo)
	{
	  /* trap places where a translation line occurs but the
	     next corresponding item in the transliteration is a
	     $-line.  FIXME: what if the translation has a labeled
	     $-line so the translation line matches up to it--is that
	     taken care of by not including labeled dollar lines in the
	     dollar fifo?		 
	  */
	  const char *doll_id = dollar_peek();
	  if (doll_id)
	    {
	      /* doll_id should be later than the trans line */
	      int interval = xid_diff((const char *)xid,doll_id);
	      if (interval > 0)
		{
		  vwarning2(file,p->lnum,"expected $-line to match transliteration.\n\tTo have no corresponding $-line include a spacer in the translation:\n\t\t$ (SPACER)");
		  /* flush the translit dollar line that has no
		     counterpart in the translat */
		  (void)dollar_get();
		  goto repeat; /* trap multiple missing SPACERs */
		}
	    }
	}
      if (last_p && *last_xid)
	{
	  /* automatically provide sref/eref even when user only gives
	     sref */
	  int interval = xid_diff((const char*)xid,(const char *)last_xid);
	  if (interval > 1 || (interval==0 && multi_trans_line))
	    {
	      char buf[10];
	      sprintf(buf,"%d",interval);
	      setAttr(last_p,a_xtr_sref,getAttr(last_p,"xtr:ref"));
	      removeAttr(last_p, "xtr:ref");
	      setAttr(last_p,a_xtr_eref,(unsigned char *)xid_prev((const char *)xid));
	      setAttr(last_p,a_xtr_rows,(unsigned char *)buf);
	    }
	  else if (interval < 1)
	    {
	      vwarning2(file,p->lnum,"translation alignment out of order");
	    }
	}
      else if (*last_xid && dollar_fifo)
	{
	  /* last item was a trans_dollar -- trap backtracking */
	  int interval = xid_diff((const char*)xid,(const char *)last_xid);
	  if (interval < 0)
	    vwarning2(file,p->lnum,"preceding $-line incorrectly aligned");
	}

      setAttr(p,n_xh,(unsigned char *)nstab[n_xh].ns);
      setAttr(p,n_xtr,(unsigned char *)nstab[n_xtr].ns);
      if (eref)
	setAttr(p,a_xtr_sref,xid);
      else
	setAttr(p,a_xtr_ref,xid);
      if (!*getAttr(p,"xtr:lab-start-label"))
	{
	  setAttr(p,a_xtr_lab_start_label,pool_copy(lab));
	  setAttr(p,a_xtr_lab_start_lnum,pool_copy(lnum_of(lab)));
	}
      if (overlap)
	setAttr(p,a_xtr_overlap,(unsigned const char *)"1");
      strcpy((char *)last_xid,(const char *)xid);
      last_p = p;
      h_arefs(xid);
    }
  else
    {
      *last_xid = '\0';
      last_p = NULL;
    }
  
  if (eref)
    {
      s = eref;
      while (isspace(*s))
	++s;
      xid = check_label(s,etu_labeled,NULL);
      if (xid)
	{
	  char buf[5];
	  int interval = xid_diff((const char*)xid,(const char *)sref_xid);
	  if (interval < 1)
	    {
	      vwarning2(file,p->lnum,"end of range comes before start of range");
	      /* keep processing even though it will produce unsightly output */
	    }
	  else
	    strcpy((char *)last_xid,(const char *)xid);
	  sprintf(buf,"%d",1 + int_of(xid) - sref);
	  setAttr(p,a_xtr_eref,xid);
	  setAttr(p,a_xtr_lab_end_label,pool_copy(s));
	  setAttr(p,a_xtr_lab_end_lnum,pool_copy(lnum_of(s)));
	  setAttr(p,a_xtr_rows,ucc(buf));
	  /* This attribute means that the start of this trans block
	     overlaps with the end of the preceding one */
	  if (overlap)
	    setAttr(p,a_xtr_overlap,(unsigned const char *)"1");
	}
      else
	*last_xid = '\0';
      last_p = NULL;
    }

  if (trans_abbrevved_labels && *last_label_buf && xstrcmp(getClass(p),"dollar"))
    {
      unsigned char *prefix = label_prefix(lab);
      if (prefix && !strncmp(cc(last_label_buf),cc(prefix),strlen(cc(prefix))))
	{
	  unsigned char *rendlabel = lab + strlen(cc(prefix));
	  while (isspace(*rendlabel))
	    ++rendlabel;
	  setAttr(p,a_xtr_rend_label,rendlabel);
	}
    }
}

void
trans_finish_labels(struct node *text, struct translation *tp)
{
  struct node *last_trans_node = lastChild(tp->tree);

  if (!last_trans_node)
    return;

  if (!strcmp((char*)getAttr(last_trans_node,"class"),"tr"))
    {
      if (!*(getAttr(last_trans_node,"a_xtr_eref")))
	{
	  int j;
	  const char *xid = NULL;
	  /* drill down to object/surface/column for a transliteration; 
	     just to top level for composite */
	  struct node *last_col;
#if 0
	  last_col = lastChild(text);
	  /* get last tok, see if it's a surface, if so, dig down, 
	     otherwise use its ID as is */
#else
	  /* Not quite right: in a P lastChild(text) can be nonx */
	  if (doctype == e_composite)
	    last_col = text; /* lastChild(text) gives l/nonx level */
	  else
	    last_col = lastChild(lastChild(lastChild(text)));
	  if (last_col)
	    {
	      for (j = last_col->children.lastused-1; j>=0; --j)
		{
		  xid = (const char*)getAttr(last_col->children.nodes[j], "xml:id");
		  if (*xid)
		    break;
		}
	    }
#endif
	  if (xid && *xid)
	    {
	      const char *refid = (const char *)getAttr(last_trans_node,"xtr:ref");
	      if (*refid && strcmp(refid,xid))
		{
		  int interval = xid_diff((const char*)xid,(const char *)last_xid);
		  char buf[10];
		  sprintf(buf,"%d",interval);
		  removeAttr(last_trans_node,"xtr:ref");
		  setAttr(last_trans_node,a_xtr_sref,(unsigned const char *)refid);
		  setAttr(last_trans_node,a_xtr_eref,(unsigned const char *)xid);
		  setAttr(last_trans_node,a_xtr_rows,(unsigned const char *)buf);
		}
	    }
	}
    }
}

void
trans_cols_attr(struct node *tp)
{
  if (tp && !*(getAttr(tp,"xtr:cols")))
    {
      char colsbuf[128];
      (void)sprintf(colsbuf,"%d",max_trans_cols);
      setAttr(tp,a_xtr_cols,ucc(pool_copy((unsigned char*)colsbuf)));
      max_trans_cols = 1;
    }
}

static void
h_arefs(const unsigned char *aref)
{
  int i;
  for (i = 0; i < lth_used; ++i)
    {
      const unsigned char *tlit_h = nth_tlit_hdr(aref,i);
      if (tlit_h)
	appendAttr(last_trans_h[i],attr(a_xtr_hdr_ref,tlit_h));
      else
	appendAttr(last_trans_h[i],attr(a_xtr_ref,aref));
    }
  lth_used = 0;
}

static void
parenify(char *buf)
{
  char *tmp = malloc(strlen(buf)+3);
  if (!tmp)
    {
      fputs("atf2xtf: out of core\n",stderr);
      exit(2);
    }
  strcpy(tmp,"(");
  strcat(tmp,buf);
  strcat(tmp,")");
  strcpy(buf,tmp);
  free(tmp);
}

#define scan_multi_paras(p_type)				   \
  while (1)							   \
    {								   \
      nlines = trans_para(nlines,s,curr_block,p_type,0,NULL,0);	   \
      while (nlines[0] && is_blank_line(nlines[0]))		   \
	{							   \
	  if (need_lemm) lem_save_line("");			   \
	  ++lnum;						   \
	  ++nlines;						   \
	}							   \
      if (!nlines[0]						   \
	  || looks_like_lnum(nlines[0])				   \
	  || nlines[0][0] == '&'				   \
	  || nlines[0][0] == '@'				   \
	  || nlines[0][0] == '$'				   \
	  )							   \
	{							   \
	  lines = nlines;					   \
	  break;						   \
	}							   \
      else							   \
	{							   \
	  s = nlines[0];					   \
	}							   \
    }

#define skip_blank_lines()		      \
  while (lines[1] && is_blank_line(lines[1])) \
    {					      \
      if (need_lemm) lem_save_line("");    \
      ++lnum;				      \
      ++lines;				      \
    }

static const char *
find_marker(unsigned char *s)
{
  static unsigned char mbuf[8];

  while (*s && isspace(*s))
    ++s;
  if ('^' == *s)
    {
      const unsigned char *start = ++s;
      while (*s && '^' != *s && s - start < 7)
	{
	  int oset = s - start;
	  mbuf[oset] = *s++;
	}
      if (s - start == 7)
	{
	  warning("note marker too long or missing closing '^'");
	  return NULL;
	}
      else
	{
	  mbuf[s-start] = '\0';
	  return (const char *)mbuf;
	}
    }
  else
    {
      return NULL;
    }
}

void
trans_hash_init(void)
{
  if (!trans_hash)
    trans_hash = hash_create(1);
}
int
trans_hash_add(const char *t)
{
  static int one = 1;
  if (hash_find(trans_hash, (uccp)t))
    return 1;
  hash_add(trans_hash,pool_copy((uccp)t), &one);
  return 0;
}
void
trans_hash_term(void)
{
  if (trans_hash)
    {
      hash_free(trans_hash,NULL);
      trans_hash = NULL;
    }
}

unsigned char **
trans_block(unsigned char **lines,unsigned char *token,struct block_token*blocktokp, char save)
{
  register unsigned char *s = NULL;
  enum e_tu_types toktype = etu_top;
  struct node *text = current_trans->tree;
  unsigned char**nlines;
  unsigned char label_buf[128], unit_buf[128], *unit_buf_nump, span_buf[128];
  struct node *curr_block = NULL;
  int innerp = 0;

  start_lnum = lnum;
  *label_buf = *unit_buf = *span_buf = '\0';
  sprintf((char*)unit_buf,"%s.U",current_trans->id);
  unit_buf_nump = unit_buf + xxstrlen(unit_buf);
  multi_trans_line = 0;

  if (lines[0][1] == '(')
    {
      note_initialize_line();
      toktype = etu_label;
    }
  else
    {
      s = *lines+xxstrlen(*lines)+1;
      while (*s && isspace(*s))
	++s;
      switch (blocktokp->type)
	{
	case TR_LABEL:
	  toktype = etu_label;
	  note_initialize_line();
	  break;
	case TR_SPAN:
	  toktype = etu_span;
	  note_initialize_line();
	  break;
	case TR_UNIT:
	  toktype = etu_unit;
	  note_initialize_line();
	  break;
	case TR_NOTE:
	  toktype = etu_note;
	  break;
	case TR_H1:
	  toktype = etu_h1;
	  break;
	case TR_H2:
	  toktype = etu_h2;
	  break;
	case TR_H3:
	  toktype = etu_h3;
	  break;
	default:
	  break;
	}
    }

  switch (toktype)
    {
    case etu_label:
    case etu_span:
      if (lines[0][1] == '(')
	{
	  unsigned char *t = lines[0]+2;
	  s = t;
	  while (*s && ')' != *s)
	    ++s;
	  if (*s)
	    {
	      if (s - t)
		{
		  *s = '\0';
		  xstrcpy(label_buf,t);
		}
	      else
		innerp = 1; /* @() */
	      if ('+' == s[1])
		{
		  multi_trans_line = 1;
		  ++s;
		}
	    }
	  else
	    {
	      warning("unclosed ')' in label");
	      start_lnum = -1;
	      return lines+1;
	    }
	  ++s;
	  while (isspace(*s))
	    ++s;
	}
      else
	{
	  Uchar *lp = NULL;
	  /* this is the label of @label */
	  if ('+' == save)
	    {
	      multi_trans_line = 1;
	    }
	  lp = trans_rest_of_line(s);
	  if (!lp || strlen((char*)lp) > 127 || strchr((char*)lp,'@'))
	    {
	      warning("suspicious @label line; add a blank line after it");
	      xstrncpy(label_buf, lp, 127);
	    }
	  else
	    xstrcpy(label_buf,lp);
	  skip_blank_lines();
	  ++lines;
	  ++lnum;
	  s = *lines;
	}

      if (!innerp)
	{
	  curr_block = appendChild(text,
				   elem(e_xh_p,NULL,lnum,DIVISION));
	  curr_block->lnum = start_lnum;
	  ctr(curr_block);
	  setAttr(curr_block,n_xh,(unsigned char *)nstab[n_xh].ns);
	  setAttr(curr_block,n_xtr,(unsigned char *)nstab[n_xtr].ns);
	  /* FIXME: This is not subtle enough--some translation paras
	     will be so long that they should be the #-target
	     themselves, not an earlier para */
	  if (next_trans_p_id > 3)
	    {
	      sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id-3);
	      appendAttr(curr_block,attr(a_xtr_cid,ucc(trans_p_idbuf)));
	    }
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
	  appendAttr(curr_block,attr(a_xml_id,ucc(trans_p_idbuf)));
	  trans_wid = 0;

	  if (NULL == lines[0])
	    {
	      start_lnum = -1;
	      return lines;
	    }
	  
	  nlines = lines;
	  scan_multi_paras(2);

	  labeled_labels(curr_block,label_buf);
	  se_label(curr_block,cc(text_n),cc(label_buf));
	  if (trans_parenned_labels)
	    parenify((char*)label_buf);
	  appendAttr(curr_block, attr(a_xtr_label,label_buf));
	  xstrcpy(last_label_buf,label_buf);
	}
      else
	{
	  lines = trans_para(lines,s,lastChild(text),2,0,NULL,1);	  
	}
      break;
    case etu_unit:
      xstrcpy(unit_buf_nump,trans_rest_of_line(s));
      skip_blank_lines();
      break;
    case etu_note:
      /* Do lookahead for note mark so we can set up the internal note linkage
	 correctly */
      {
	const char *marker = find_marker(s);
	if (!marker)
	  {
	    warning("notes without markers are not supported in translations");
	  }
	else
	  {
	    extern void note_register_note(const unsigned char *mark, struct node *node);
	    curr_block = appendChild(text,
				     elem(blocktokp->etype,NULL,lnum,DIVISION));
	    note_register_note((unsigned char *)marker, curr_block);
	  }
      }
      /* FIXME: validate @note against the ^N^ markers; also, accept unicode 
	 superscript note markers and generate the same as ^N^ markers */
      setAttr(curr_block,a_class,ucc("note"));

      nlines = lines;
      in_note = nocellspan = 1;
      scan_multi_paras(1);
      in_note = nocellspan = 0;
      break;
    case etu_h1:
    case etu_h2:
    case etu_h3:
      curr_block = appendChild(text,
			       elem(blocktokp->etype,NULL,lnum,DIVISION));
      ctr(curr_block);
      if (lth_used == lth_alloced)
	{
	  lth_alloced += 8;
	  last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
	}
      last_trans_h[lth_used++] = curr_block;
      sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
      /* return lines pointing at the blank line that
	 ended the para */
      nocellspan = 1;
      lines = trans_para(lines,s,curr_block,0,0,NULL,1);
      nocellspan = 0;
      break;
    default:
      break;
    }
  /* trans_block must bump lines at least once;
     but it must not bump lnum for the last line because
     the loop in block.c does that */
  if (lnum == start_lnum && *lines)
    ++lines;
  else
    --lnum;

  /* start_lnum = -1; */
  return lines;
}

unsigned char **
trans_dollar(unsigned char **lines)
{
  unsigned char *s = lines[0];
  struct node *curr_block = appendChild(current_trans->tree,
					elem(e_xh_p,NULL,lnum,DIVISION));
  setClass(curr_block,"dollar");
  start_lnum = lnum;
  if (s[1] == '@' && s[2] == '(')
    {
      unsigned char label_buf[128], *lp;
      *label_buf = '\0';
      lp = label_buf;
      s += 3;
      while (*s && *s != ')')
	*lp++ = *s++;
      *lp = '\0';
      if (*s)
	{
	  labeled_labels(curr_block,label_buf);
	  appendAttr(curr_block, attr(a_xtr_silent,ucc("1")));
	}
      else
	warning("label on $-line lacks closing ')'");
    }
  else if (dollar_fifo)
    {
      const char *dollar_id = dollar_get();
      if (dollar_id)
	{
	  setAttr(curr_block,a_xtr_ref,(unsigned char *)dollar_id);
	  if (last_p && *last_xid)
	    {
	      int interval = xid_diff(dollar_id,(const char *)last_xid);
	      if (interval > 1)
		{
		  char buf[10];
		  sprintf(buf,"%d",interval);
		  setAttr(last_p,a_xtr_sref,getAttr(last_p,"xtr:ref"));
		  removeAttr(last_p, "xtr:ref");
		  setAttr(last_p,a_xtr_eref,(unsigned char *)xid_prev(dollar_id));
		  setAttr(last_p,a_xtr_rows,(unsigned char *)buf);
		}
	      last_p = NULL;
	    }
	  strcpy((char *)last_xid,(const char *)dollar_id);
	}
      else
	warning("translation dollar-line has no parallel in transliteration");
    }
  else
    {
      setAttr(curr_block,a_xtr_standalone,(unsigned char *)"1");
    }

#if 0 /* $ does not work like headings */
  if (lth_used == lth_alloced)
    {
      lth_alloced += 8;
      last_trans_h = realloc(last_trans_h, lth_alloced * sizeof(struct node *));
    }
  last_trans_h[lth_used++] = curr_block;
#endif

  ++s;
  while (isspace(*s))
    ++s;
  (void)trans_inline(curr_block,s,NULL,0);

  /* start_lnum = -1; */

  return lines;
}

static int
is_blank_line(unsigned char *l)
{
  if (!*l)
    return 1;
  while (isspace(*l))
    ++l;
  return !*l;
}

static int
looks_like_lnum(const unsigned char *s)
{
  if (isdigit(*s))
    {
      while (*s && !isspace(*s))
	++s;
      return s[-1] == '.';
    }
  return 0;
}

unsigned char **
trans_para(unsigned char **lines, unsigned char *s, struct node *p, int p_elem, 
	   int with_id, unsigned const char *label, int stop_at_lnum)
{
  int nchars = 0;
  unsigned char **start = lines,*sol = s,*text,*first_sol = s;
  int is_comment = 0, spanall = 0;
  start_lnum = lnum;
  if (p == NULL)
    p = current_trans->tree;
  if (s == lines[0] && *lines && lines[0][0] == '#')
    is_comment = 1;
  if (*s == '|' && isspace(s[1]))
    {
      spanall = 1;
      *s = ' ';
      s += 2;
    }
  while (*lines)
    {
      if (lnum > start_lnum && !isspace(*s))
	{
	  if (stop_at_lnum
	      && (current_trans->etype == etu_interlinear 
		  || current_trans->etype == etu_parallel))
	    break; /* break at non-space sol after first line if parallel/interlinear */
	  else if (*s == '&' || *s == '$' 
		   || (*s == '#' && !is_comment))
	    {
	      notice("possible failure to end translation para with blank line (found `&' or '$')");
	      goto stop;
	    }
	  else if (!stop_at_lnum && looks_like_lnum(s))
	    {
	      notice("possible failure to end translation para with blank line");
	      goto stop;
	    }
	  else if (*s == '@')
	    {
	      /* break on a block token; keep processing otherwise */
	      unsigned char *t = s+1, *e = s+1;
	      char save;
	      while (*e && !isspace(*e))
		++e;
	      save = *e;
	      *e = '\0';
	      if (blocktok((const char *)t,e-t))
		{
		  *e = save;
		  break;
		}
	      else
		*e = save;
	      
	    }
	}
      while (isspace(*s))
	++s;
      if (!*s)
	break; /* always break at a blank link */
      else
	{
	  if (need_lemm && sol == lines[0])
	    lem_save_line((const char *)sol);
	  nchars += xxstrlen(sol);
	  ++nchars;
	  ++lines;
	  ++lnum;
	  s = sol = *lines;
	}
    }
  text = malloc(nchars+1);
  *text = '\0';
  s = first_sol;
  while (start < lines)
    {
      xstrcat(text,s);
      xstrcat(text,"\n");
      ++start;
      s = *start;
    }
  if (xxstrlen(text) > 0)
    text[xxstrlen(text)-1] = '\0';
  if (p_elem)
    {
      p = appendChild(p,elem(p_elem == 2 ? e_xh_innerp : e_xh_p,NULL,lnum,DIVISION));
      if (spanall)
	appendAttr(p,attr(a_xtr_spanall,(unsigned char *)"1"));
      if (with_id)
	{
	  sprintf(trans_p_idbuf,"%s.%d",trans_id_base,next_trans_p_id++);
	  appendAttr(p,attr(a_xml_id,ucc(trans_p_idbuf)));
	  trans_wid = 0;
	  /* non-NULL label means caller is trans_inter;
	     this is used for interlinear and parallel;
	     only interlinear can use line_id_buf */
	  if (label)
	    {
	      appendAttr(p,attr(a_xtr_label,label));
	      se_label(p,cc(text_n),cc(label));
	      if (current_trans->etype == etu_interlinear)
		{
		  if (tr_id_buf)
		    appendAttr(p,attr(a_xtr_ref,ucc(tr_id_buf)));
		  else
		    appendAttr(p,attr(a_xtr_ref,ucc(line_id_buf)));

		  setAttr(p,n_xh,(unsigned char *)nstab[n_xh].ns);
		  setAttr(p,n_xtr,(unsigned char *)nstab[n_xtr].ns);

		  h_arefs(ucc(line_id_buf));
		}
	      else
		{
		  const unsigned char *xid = check_label(label,etu_parallel,NULL);

		  setAttr(p,n_xh,(unsigned char *)nstab[n_xh].ns);
		  setAttr(p,n_xtr,(unsigned char *)nstab[n_xtr].ns);

		  if (xid)
		    {
		      appendAttr(p,attr(a_xtr_ref,ucc(xid)));
		      h_arefs(ucc(xid));
		    }
		  else
		    {
		      vwarning("%s: label used in parallel translation is not in transliteration", label);
		    }
		}
	    }
	}
      if (is_comment)
	setAttr(p,a_class,ucc("tr-comment"));
      else if (p_elem != 2)
	ctr(p);
    }
  else if (label && last_trans_h)
    h_arefs(ucc(line_id_buf));

  if (is_comment)
    appendChild(p,textNode(text));
  else if (strstr((const char *)text,"@&"))
    {
      int cols_this_para = 0;
      int init_cell = 0;

      while (isspace(*text))
	++text;

      if (text[0] == '@' && text[1] == '&')
	text += 2;

      init_cell = 1;

      /* at the start of this loop, text points at either the span 
	 digits following @& or at the text to process after @& */
      while (*text)
	{
	  unsigned char *c = text, *resume;
	  struct node *cc = appendChild(p,elem(e_xh_span,NULL,lnum,CELL));
	  if (!in_note)
	    {
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		appendAttr(cc,attr(a_dir,(uccp)"rtl"));
	      appendAttr(cc,attr(a_xtr_span,ucc("1")));
	    }
	  if (init_cell)
	    {
	      char spanbuf[10];
	      int i = 0;
	      while (isdigit(*c))
		{
		  if (i < 9)
		    spanbuf[i++] = *c++;
		  else
		    {
		      warning("translation cell span too long (max 999999999");
		      break;
		    }
		}
	      spanbuf[i] = '\0';
	      if (!*spanbuf) /* FIXME: @column in parallel trans */
		strcpy(spanbuf,"1");
	      setAttr(cc,a_xtr_span,ucc(spanbuf));
	      cols_this_para += atoi(spanbuf);
	    }

	  while (*text && ('@' != *text || '&' != text[1]))
	    ++text;
	  resume = text;
#if 1
	  if (*resume)
	    init_cell = 1;

	  else
	    init_cell = 0;
#else
	  if (*resume)
	    {
	      resume += 2; /* skip @& */
	      ++cols_this_para;
	      if (isspace(*resume))
		{
		  while (isspace(*resume))
		    ++resume;
		  init_cell = 0;
		}
	      else
		init_cell = 1;
	    }
#endif
	  while (isspace(text[-1]))
	    --text;
	  *text = '\0';
	  
	  (void)trans_inline(cc,c,NULL,1);
	  text = resume;
	  if (init_cell)
	    text += 2;
	}
      if (cols_this_para > max_trans_cols)
	max_trans_cols = cols_this_para;
    }
  else
    {
      struct node *cc = p;
      while (isspace(*text))
	++text;
      if (*text)
	{
	  if (!nocellspan)
	    {
	      cc = appendChild(p,elem(e_xh_span,NULL,lnum,CELL));
	      setClass(cc,"cell");
	      if (need_dir_rtl)
		appendAttr(cc,attr(a_dir,(uccp)"rtl"));
	      appendAttr(cc,attr(a_xtr_span,ucc("1")));
	    }
	  (void)trans_inline(cc,text,NULL,1);
	}
      else
	{
	  /* unwind the spurious innerp node */
	  p = p->parent;
	  removeLastChild(p);
	}
    }
 stop:
  if (lnum == start_lnum)
    {
      ++lnum;
      ++lines;
    }

  /*start_lnum = -1;*/
  
  return lines;
}

static int ntrans = 0;
static int ntrans_alloc = 0;
static void
add_trans(struct translation *t)
{
  if (t == NULL)
    {
      int i;
      for (i = 0; i < ntrans; ++i)
	translations[i]->tree = NULL;
      ntrans = 0;
    }
  else
    {
      if (ntrans == ntrans_alloc)
	{
	  if (ntrans_alloc)
	    ntrans_alloc *= 2;
	  else
	    ntrans_alloc = 1;
	  translations = realloc(translations, ntrans_alloc*sizeof(struct translation*));
	}
      translations[ntrans++] = t;
    }
}

struct node *
curr_trans_tree(void)
{
  if (ntrans)
    return translations[ntrans-1]->tree;
  else
    return NULL;
}

int
has_trans()
{
  return ntrans;
}
void
init_trans()
{
  ntrans = 0;
  *last_xid = '\0';
  last_p = NULL;
}

static unsigned char *
trans_rest_of_line(unsigned char *s)
{
  unsigned char *ret;
  while (isspace(*s))
    ++s;
  ret = s;
  s += xxstrlen(s);
  while (isspace(s[-1]))
    --s;
  if (isspace(*s))
    *s = '\0';
  return ret;
}

static int
refattrs(struct node *elem,unsigned char *labelp,char *id)
{
  static int last_label = 0;
  int s_index,e_index;
  unsigned char *elabel;

  if (elem == NULL)
    {
      last_label = 0;
      return 0;
    }
  appendAttr(elem, attr(a_xtr_label,labelp));
  se_label(elem,cc(text_n),cc(labelp));

  if (*labelp == '\0')
    return 0;
  elabel = uc(strstr(cc(labelp)," - "));
  if (elabel)
    {
      *elabel++ = '\0';
      elabel += 2;
    }
  s_index = e_index = find_label(labtab,last_label,labelp);
  if (s_index >= 0)
    {
      /*       char * refs; */
      int i, nrefs;
      appendAttr(elem,
		 attr(a_xtr_sref,
		      labtab->table[s_index].xmlid));
      if (elabel)
	{
	  e_index = find_label(labtab,s_index,elabel);
	  if (e_index < 0)
	    {
	      vwarning("end label %s not found in transliteration",elabel);
	      if (need_alignment)
		return -1;
	    }
	  else
	    {
	      appendAttr(elem,
			 attr(a_xtr_eref,
			      labtab->table[e_index].xmlid));
	    }
	}
      nrefs = 1 + e_index - s_index;
      for (i = 0; i < nrefs; ++i)
	mapentry(cc(labtab->table[s_index+i].xmlid),id);
      last_label = e_index;
    }
  else if (labtab->used)
    {
      vwarning("start label %s not found in transliteration",labelp);
      if (need_alignment)
	return -1;
    }
  return 0;
}
#define MAP_BUFSIZ 32
struct map { char lid[MAP_BUFSIZ]; char tid[MAP_BUFSIZ]; };
static struct map *pmap = NULL;
static int map_alloced = 0, map_used = 0;

static void
mapentry(const char *lid,const char *tid)
{
  if (lid == NULL)
    {
      map_used = 0;
      return;
    }
  if (map_used == map_alloced)
    {
      map_alloced = map_alloced ? (map_alloced*2) : 128;
      pmap = realloc(pmap,map_alloced * sizeof(struct map));
    }
  if (strlen(lid) >= MAP_BUFSIZ || strlen(tid) >= MAP_BUFSIZ)
    {
      fprintf(stderr,"atf2xtf: internal error: id overflow in translate map. Stop.\n");
      exit(-1);
    }
  strcpy(pmap[map_used].lid,lid);
  strcpy(pmap[map_used].tid,tid);
  ++map_used;
}
void
trans_mapdump(struct node *p)
{
  struct node *xmap = appendChild(p,elem(e_xtr_map,NULL,lnum,TEXT));
  register int i;
  for (i = 0; i < map_used; ++i)
    {
      struct node *m = appendChild(xmap,elem(e_xtr_l2t,NULL,0,TEXT));
      appendAttr(m,attr(a_lid,ucc(pmap[i].lid)));
      appendAttr(m,attr(a_tid,ucc(pmap[i].tid)));
    }
}
static int
all_upper(const char *s)
{
  while (*s)
    if (!isupper(*s) && '.' != *s)
      return 0;
    else
      ++s;
  return 1;
}

#if 0
static void
discretionary(unsigned char *s)
{
  while (*s)
    {
      /* @- is discretionary hyphen which is two bytes in UTF8
	 so we just overwrite the @- and back up to reprocess
      */
      --s;
    }
}
#endif

/* FIXME: what about notes in inline translations?
 */
unsigned char *
trans_inline(struct node*parent,unsigned char *text,const char *until, int with_trwords)
{
  unsigned char *s = text, *start = text;
  int ocurly = 0, nested_curly = 0;

  /*  discretionary(s); now a no-op */

  while (*s)
    {
      if (until && !xstrncmp(s,until,xxstrlen(until)))
	{
	  *s = '\0';
	  s += xxstrlen(until);
	  break;
	}
      else
	{
	  /* We can't preprocess @- blindly because of constructs
	     like @"bow"@-star.  Processing @- here is safe because
	     we are only here at the start of @ sequences */
	  if ('@' == s[0] && '-' == s[1])
	    {
	      s[0] = (unsigned char)0xc2;
	      s[1] = (unsigned char)0xad;
	    }
	  switch (*s)
	    {
	    case '@':
	      *s = '\0';
	      if (s++ > start)
		{
		  if (with_trwords)
		    trwords(parent,start);
		  else
		    textwords(parent,start,s);
		}
	      start = s;
	      if (*s == '"')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span,
							      NULL,lnum,FIELD));
		  setClass(span,"literal");
		  ++s;
		  start = s = trans_inline(span,s,"\"@",1);
		}
	      else if (*s == '?')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span,
							      NULL,lnum,FIELD));
		  setClass(span,"uncertain");
		  ++s;
		  start = s = trans_inline(span,s,"?@",1);
		}
#if 0 /* @- now means discretionary hyphen */
	      else if (*s == '-' || *s == '+')
		{
		  struct node *span = appendChild(parent,elem(e_xh_span
							      ,NULL,lnum,FIELD));
		  if (s[1] != '{')
		    warning("@- and @+ must be followed by '{'");
		  setClass(span,*s == '-' ? "smaller" : "larger");
		  parent = span;
		  start = s+2;
		  ++ocurly;
		}
#endif
	      else if (*s == '&')
		{
		  /* silently ignore @& until we implement it */
		  while (isspace(s[1]))
		    ++s;
		  start = ++s;
		}
	      else
		{
		  while (*s > 127 || isalnum(*s) || '.' == *s 
			 || ('(' == *s || ')' == *s)
			 || ('@' == *s && '-' == s[1]))
		    {
		      if ('@' == *s)
			{
			  s[0] = (unsigned char)0xc2;
			  s[1] = (unsigned char)0xad;
			  s += 2;
			}
		      else
			++s;
		    }
		  if (s > start)
		    {
		      if ('{' == *s)
			{
			  struct node *span = appendChild(parent,elem(e_xh_span,
								      NULL,lnum,FIELD));
			  *s = '\0';
			  setClass(span,start);
			  start = ++s;
			  parent = span;
			  ++ocurly;
			}
		      else
			{
			  unsigned char save = *s;
			  struct node *span = appendChild(parent,elem(e_xh_span,
								      NULL,lnum,FIELD));
			  *s = '\0';
			  appendChild(span,textNode(start));
			  if (all_upper((char *)start))
			    setClass(span,"sign");
			  else
			    setClass(span,"foreign");
			  
			  *s = save;
			  start = s;
			}
		    }
		  else
		    {
		      warning("unexpected character after '@'");
		      start = s;
		    }
		}
	      break;
	    case '{':
	      ++nested_curly;
	      ++s;
	      break;
	    case '}':
	      if (nested_curly > 0)
		{
		  --nested_curly;
		  ++s;
		}
	      else if (ocurly)
		{
		  *s++ = '\0';
		  if (s > start)
		    {
		      const char *class = cc(getAttr(parent,"class"));
		      if (!strcmp(class,"akk")
			  || !strcmp(class,"gdl")
			  || !strcmp(class,"sum"))
			{
			  struct node *res = NULL;
			  unsigned char *buf = NULL;
			  if (strcmp(class, "gdl"))
			    {
			      buf = malloc(strlen(cc(start)) + 6);
			      sprintf((char*)buf,"%%%s %s",class,start);
			      res = gdl(buf, GDL_FRAG_OK);
			      removeAttr(parent,"class");
			      setClass(parent,"gdl");
			    }
			  else
			    res = gdl(start, GDL_FRAG_OK);
			  if (res)
			    {
			      int i;
			      for (i = 0; i < res->children.lastused; ++i)
				appendChild(parent,res->children.nodes[i]);
			    }
			}
		      else if (with_trwords)
			trwords(parent,start);
		      else
			textwords(parent,start,s);
		    }
		  start = s;
		  parent = parent->parent;
		  --ocurly;
		}
	      else
		vwarning("%s: } with no matching {",s++);
	      break;
	    case '^':
	      *s = '\0';
	      if (s++ > start)
		{
		  if (with_trwords)
		    trwords(parent,start);
		  else
		    textwords(parent,start,s);
		  start = s;
		}
	      start = s;
	      while (isalnum(*s))
		++s;
	      if (s > start && '^' == *s)
		{
		  struct node *cnode = NULL;
		  struct node *span = NULL;
		  *s = '\0';
		  cnode = (parent->parent ? parent->parent : parent);
		  if (!strcmp(cc(getAttr(cnode, "class")), "note"))
		    {
		      const unsigned char *nauto = getAttr(cnode,"note:auto");
		      span = appendChild(parent,elem(e_xh_span,NULL,lnum,FIELD));
		      if (nauto && *nauto)
			appendChild(span,textNode(nauto));
		      else
			appendChild(span,textNode(start));
		      setClass(span,"notemark");
		    }
		  else
		    {
		      span = appendChild(parent,elem(e_xh_span,NULL,lnum,FIELD));
		      note_register_tag(start, span);
		      setClass(span,"notelink");
		    }
		  start = ++s;
		}
	      else
		{
		  warning("bad note in ^...^");
		  start = ++s;
		}
	      break;
	    case '#':
	    case '$':
	      if (s > text && s[-1] == '\0')
		goto stop;
	      ++s;
	      break;
	    default:
	      ++s;
	      break;
	    }
	}
    }
 stop:
  if (s > start)
    {
      if (with_trwords)
	trwords(parent,start);
      else
	textwords(parent,start,s);
    }
  return s;
}

static void
textwords(struct node *p, unsigned char *start, unsigned char *end)
{
  unsigned char save = *end;
  *end = '\0';
  appendChild(p,textNode(start));
  *end = save;
}

static void
protect_roman(struct node *w, unsigned char *start)
{
  if (strpbrk((const char *)start,"[]()"))
    {
      unsigned char *formbuf = malloc(strlen((const char *)start)), *formbufp = formbuf, *s;
      s = start;
      while (*s)
	{
	  switch (*s)
	    {
	    case '[':
	    case ']':
	    case '(':
	    case ')':
	      {
		struct node *span;
		char b[2];
		b[0] = *s;
		*s++ = b[1] = '\0';
		appendChild(w,textNode(start));
	       	if ((span = appendChild(w,elem(e_xh_span,NULL,lnum,FIELD))))
		  {
		    setClass(span,"r");
		    appendChild(span,textNode(ucc(b)));
		  }
		start = s;
	      }
	      break;
	    default:
	      *formbufp++ = *s++;
	      break;
	    }
	}
      *formbufp = '\0';
      if (s > start)
	appendChild(w,textNode(start));
      if (*formbuf && !xstrcmp(getAttr(w,"class"), "w"))
	setAttr(w,a_xtr_form,formbuf);
      free(formbuf);
    }
  else
    appendChild(w,textNode(start));
}

static void
trwords(struct node *p, unsigned char *s)
{
  unsigned char *start = s;
  while (*s)
    {
      if (isspace(*s))
	{
	  unsigned char save = *s;
	  *s = '\0';
	  trwords_w(p,start);
	  *s = save;
	  start = s++;
	  while (isspace(*s))
	    ++s;
	  save = *s;
	  *s = '\0';
	  protect_roman(p,start); /*appendChild(p,textNode(start));*/
	  start = s;
	  *s = save;
	}
      else
	++s;
    }
  if (s > start && *start)
    trwords_w(p,start);
}

static void
trwords_w(struct node *p, unsigned char *s)
{
  unsigned char *start = s,*backslash, *punct, punctsave;
  struct node *w = elem(e_xh_span,NULL,lnum,FIELD);
  unsigned char wid[512];

  sprintf((char*)wid,"%s.%d",trans_p_idbuf, trans_wid++);
  appendAttr(w,attr(a_class,ucc("w")));
  appendAttr(w,attr(a_xml_id,wid));

  if (*start && *start < 128 && !isalnum(*start))
    {
      unsigned char save;
      punct = start++;
      while (*start && *start < 128 && !isalnum(*start))
	++start;
      save = *start;
      *start = '\0';
      protect_roman(p,punct);
      /*appendChild(p,textNode(punct));*/
      *start = save;
    }

  if (!*start)
    return;

  punct = start + strlen(cc(start));
  while (punct > start && punct[-1] < 128 && !isalnum(punct[-1]))
    --punct;
  if (*punct)
    {
      punctsave = *punct;
      *punct = '\0';
    }
  else
    punctsave = 0;

  backslash = uc(strchr(cc(start),'\\'));
  if (backslash)
    {
      *backslash++ = '\0';
      if (*backslash)
	appendAttr(w,attr(a_xtr_disamb,backslash));
      else
	appendAttr(w,attr(a_xtr_disamb,ucc("0")));
    }

  protect_roman(w,start);

  appendChild(p, w);

  if (punctsave)
    {
      *punct = punctsave;
      protect_roman(p,punct);
      /* appendChild(p,textNode(punct));*/
    }
}

void
trans_clear()
{
  add_trans(NULL);
  free(itrans);
  itrans = NULL;
}

unsigned char **
trans_inter(unsigned char **lines)
{
  unsigned char *t;
  if (!itrans)
    {
      itrans = calloc(1,sizeof(struct translation));
      itrans->type = "interlinear";
      itrans->id = uc(textid);
      itrans->code = "project";
      itrans->etype = etu_interlinear;
      itrans->tree = elem(e_xtr_translation,NULL,lnum,TEXT);
      itrans->tree->xmlns = xtr_xmlns;
      appendAttr(itrans->tree,attr(a_project, (unsigned char *)project));
      t = lines[0];
      while (*t && *t != '.' && *t != ':')
	++t;
      if (*t == '.')
	{
	  ++t;
	  itrans->lang = cc(t);
	  while (*t && *t != ':')
	    ++t;
	  if (*t)
	    {
	      unsigned char *lp = NULL;
	      *t++ = '\0';
	      lp = (ucp)itrans->lang;
	      while (*lp)
		{
		  if (!islower(*lp) && '-' != *lp)
		    {
		      warning("#tr.<lang>: has non-language content");
		      break;
		    }
		  ++lp;
		}
	    }
	  else
	    {
	      warning("#tr.<lang> doesn't end with colon");
	      return lines+1;
	    }
	}
      else if (*t == ':')
	{
	  itrans->lang = "en";
	  ++t;
	}
      else
	{
	  warning("#tr doesn't end with colon");
	  return lines+1;
	}
      appendAttr(itrans->tree,attr(a_ref,itrans->id));
      appendAttr(itrans->tree,attr(a_n, text_n));
      appendAttr(itrans->tree,attr(a_xml_lang,ucc(itrans->lang)));
      appendAttr(itrans->tree,attr(a_xtr_code,ucc("project")));
      appendAttr(itrans->tree,attr(a_xtr_type,ucc(itrans->type)));
      sprintf(trans_id_base,"%s_%s-%s",textid,itrans->code,itrans->lang);
      appendAttr(itrans->tree,attr(a_xml_id,ucc(trans_id_base)));
      next_trans_p_id = 0;
      (void)refattrs(NULL,NULL,NULL);
      mapentry(NULL,NULL);
      /* add the trans to the array now; we'll fill it in as we
	 read the text, and then it will get dumped like any other
	 translation when the transliteration is finished */
      add_trans(itrans);
#if 0
      lines = trans_block(lines+1,trans);
      /* fixup_refs(trans,text) */
#endif
      current_trans = itrans;
      trans_wid = 0;
    }
  else
    {
      t = lines[0];
      while (*t && *t != ':')
	++t;
      if (*t)
	++t;
      else
	{
	  warning("#tr doesn't end with colon");
	  return lines+1;
	}
    }

  while (isspace(*t))
    ++t;

  /* process line with ref to current line id */
  if (*t)
    {
      trans_wid = 0;
      
      lines = trans_para(lines,t,itrans->tree,1,1,curr_line_label,1);
    }
  else
    {
      ++lines;
      ++lnum;
    }

  return lines;
}

static int
xid_line(const char *x)
{
  const char *start;
  if (!x)
    return 0;
  start = x;
  x += strlen(x);
  while (x > start && x[-1] != '.')
    --x;
  if (isdigit(*x))
    return atoi(x);
  else
    return 0;
}

/*WATCHME: this only works perfectly if the line id mechanism
  increments the line id by 1 for each line_mts--the current
  implementation may be good enough for extant uses */
static int
xid_diff(const char *x1, const char *x2)
{
  int i1,i2;
  i1 = xid_line(x1);
  i2 = xid_line(x2);
  return i1-i2;
}

static const char *
xid_prev(const char *x)
{
  static char idbuf[128], *idbufp;
  strcpy(idbuf,x);
  idbufp = idbuf+strlen(idbuf);
  while (idbufp > idbuf && idbufp[-1] != '.')
    --idbufp;
  if (idbufp != idbuf)
    sprintf(idbufp,"%d",atoi(idbufp)-1);
  return idbuf;
}

#include <ctype128.h>
#include "hash.h"
#include "warning.h"
#include "tree.h"
#include "loadfile.h"
#include "memblock.h"
#include "lemline.h"
#include "ilem_form.h"
#include "xcl.h"
#include "sigs.h"
#include "xmd.h"
#include "inline.h"
#include "tokenizer.h"
#include "npool.h"
#include "xmlutil.h"
#include "lang.h"
#include "props.h"

#define uxpool_copy(x,p) (const unsigned char*)npool_copy((unsigned char*)x,p)

struct serializer_frame 
{
  char *pre;
  char *lit;
  char *pst;
};

struct lem_save
{
  unsigned char *line;
  int lnum;
  int* cells; /* parallel with forms */
  /*struct ilem_form **forms;*/
  struct xcl_ilem **forms;
  int forms_used;
  int forms_alloced;
  struct lem_save *cont;
};

int lem_simplify = 0;
extern int lem_autolem, lem_dynalem;
extern const char *lem_dynalem_tab;

extern const char *curr_dialect;
static void set_sframe(struct serializer_frame *sfp, char *literal);

struct lem_save *lem_lines = NULL;
int lem_lines_used = 0;
int lem_lines_alloced = 0;
unsigned int lem_stop_lnum = UINT_MAX;

extern const char *project;
extern const char *textid;
extern int lem_standalone;
extern int verbose;
Hash_table *word_form_index;

static const char *peri, *prov,*genr,*subg;

struct xcl_context *lemline_xcp;

static struct mb *xi_mem = NULL;

static struct lem_save*
new_lsp(void)
{
  struct lem_save *lsp;
  if (lem_lines_used == lem_lines_alloced)
    {
      lem_lines_alloced += 1024;
      lem_lines = realloc(lem_lines,
			   lem_lines_alloced * sizeof(struct lem_save));
    }
  lsp = &lem_lines[lem_lines_used++];
  lsp->forms_used = lsp->forms_alloced = 0;
  lsp->forms = NULL;
  lsp->cont = NULL;
  lsp->lnum = lnum;
  lsp->cells = NULL;
  return lsp;
}

static struct lem_save *curr_lsp, *last_lsp;
static Hash_table *lem_dynalem_hash = NULL;

static void *
lem_dynalem_ambig(const unsigned char *l1, const unsigned char *l2)
{
  char *n = malloc(strlen((const char*)l1)+strlen((const char*)l2)+2);
  sprintf(n, "%s|%s", l1, l2);
  return n;
}

static void
lem_dynalem_load(const char *tab)
{
  static int loaded = 0;
  unsigned char **tablines = NULL, *fmem;
  size_t nlines = 0, i;

  if (loaded)
    return;
  
  tablines = loadfile_lines3((unsigned char *)tab,&nlines,&fmem);
  lem_dynalem_hash = hash_create(1024);
  for (i = 0; i < nlines; ++i)
    {
      unsigned char *tab = (unsigned char*)strchr((const char*)tablines[i], '\t');
      if (tab)
	{
	  const unsigned char *f1, *f2;
	  const unsigned char *lem = NULL;
	  f1 = tablines[i];
	  f2 = tab+1;
	  *tab = '\0';
	  if (strchr((const char *)f2, '\t'))
	    fprintf(stderr, "%s:%d: too many tabs in line\n", tab, (int)i+1);
	  else if ((lem = hash_find(lem_dynalem_hash,f1)))
	    hash_add(lem_dynalem_hash, f1, lem_dynalem_ambig(lem,f2));
	  else
	    hash_add(lem_dynalem_hash, f1, (void*)f2);
	}
      else
	fprintf(stderr, "%s:%d: no tab in line\n", tab, (int)i+1);
    }
  loaded = 1;
}

void
lemline_init(void)
{
  if (lem_dynalem)
    {
      lem_dynalem_load(lem_dynalem_tab);
      inline_functions(lem_save_form_dynalem,lem_unform,lem_reset_form);      
    }
  else
    inline_functions(lem_save_form,lem_unform,lem_reset_form);
  lemline_xcp = calloc(1,sizeof(struct xcl_context));
  lemline_xcp->project = xcl_project = project;
  lemline_xcp->textid = textid;
  lemline_xcp->pool = npool_init();
  lemline_xcp->sigs = sig_context_init();
  xi_mem = mb_init(sizeof(struct xcl_ilem), 1024);
}

void
lemline_term(void)
{
  npool_term(lemline_xcp->pool);
#if 0
  sig_context_term(lemline_xcp->sigs);
#endif
  free(lemline_xcp);
  lemline_xcp = NULL;
  mb_free(xi_mem);
}

void
lem_text_init(const char *pq)
{
  Hash_table *lem_xmd;
  xmd_init();
  lem_xmd = xmd_load(project, pq);
  peri = hash_find(lem_xmd,(unsigned char *)"period");
  prov = hash_find(lem_xmd,(unsigned char *)"provenience");
  genr = hash_find(lem_xmd,(unsigned char *)"genre");
  subg = hash_find(lem_xmd,(unsigned char *)"subgenre");
  /*  hash_free(lem_xmd, NULL); */ /* NO: xmd_init does this for itself */
  word_form_index = hash_create(200);
}

void
lem_text_term()
{
  if (word_form_index)
    hash_free(word_form_index, NULL);
  if (lem_lines)
    {
      int i;
      for (i = 0; i < lem_lines_used; ++i)
	{
	  free(lem_lines[i].forms);
	  free(lem_lines[i].cells);
	}
      lem_lines_used = lem_lines_alloced = 0;
      free(lem_lines);
      lem_lines = NULL;
    }
  xmd_term();
}

static const char *
lem_base_type(const char *form)
{
  if (isdigit(*form) || 
      (('n' == *form || 'N' == *form) && (form[1] == '\0' || form[1] == '(')))
    return "n";
  else if (strchr(form,'x') || strchr(form,'$') || !strcmp(form,"o"))
    return "u";
  else if (!form[1] && form[0] == '-')
    return "-";
  else
    return "X";
}

const char *
lem_meta_id(void)
{
  static char buf[16];
  sprintf(buf,"%s.m0",textid);
  return buf;
}

const char *const*
lem_text_meta(void)
{
  const char**m = malloc(10*sizeof(char*));
  m[0] = "period";
  m[1] = peri;
  m[2] = "provenience";
  m[3] = prov;
  m[4] = "genre";
  m[5] = genr;
  m[6] = "subgenre";
  m[7] = subg;
  m[8] = m[9] = NULL;
  return m;
}

void
lem_save_line(unsigned char *lp)
{
  last_lsp = curr_lsp = new_lsp();
  curr_lsp->line = npool_copy(lp,lemline_xcp->pool);
}

void
lem_save_cont(unsigned char *lp)
{
  struct lem_save *sp;
  for (sp = curr_lsp; sp->cont; sp = sp->cont)
    ;
  sp->cont = new_lsp();
  sp->cont->line = npool_copy(lp,lemline_xcp->pool);
}

void
lem_reset_form(const char *ref, const char *form)
{
  struct xcl_ilem /*ilem_form*/ *fp = hash_find(word_form_index,(unsigned char *)ref);
  if (fp)
    {
      fp->i->f2.form = npool_copy((unsigned char *)form,lemline_xcp->pool);
      form = (const char*)fp->i->f2.form;
    }	
}

/* This routine should not set anything but FORM at the f2 level;
   that is the job of ilem_parse */
void
lem_save_form(const char *ref, const char *lang, 
	      const char *formstr, struct lang_context *langcon,
	      const char *field)
{
  struct ilem_form *form = mb_new(lemline_xcp->sigs->mb_ilem_forms);
  extern int curr_cell;
  struct xcl_ilem *xip = NULL;
  
  form->f2.owner = form;
  form->ref = (char*)ref;
  if (lang)
    {
      form->f2.lang = (unsigned char*)lang;
      form->f2.core = langcore_of(lang);
      if (strstr(lang,"949"))
	  BIT_SET(form->f2.flags,F2_FLAGS_LEM_BY_NORM);
    }
  if (BIT_ISSET(form->f2.flags,F2_FLAGS_LEM_BY_NORM))
    {
      form->f2.norm = (unsigned char *)formstr;
      form->f2.form = (const unsigned char *)"*";
    }
  else
    form->f2.form = (unsigned char *)formstr;
  form->file = (char*)file;
  form->lnum = lnum;
  form->lang = langcon;

  if (field)
    {
      props_add_prop(form,
		     uxpool_copy("env",lemline_xcp->pool),
		     uxpool_copy("field",lemline_xcp->pool),
		     uxpool_copy(field,lemline_xcp->pool),
		     NULL,
		     NULL,
		     NULL,
		     -1);
    }
  
  if (!ref[0])
    return;

  if (!curr_lsp->forms_alloced
      || curr_lsp->forms_used == curr_lsp->forms_alloced)
    {
      curr_lsp->forms_alloced += 1024;
      curr_lsp->cells = realloc(curr_lsp->cells,
				curr_lsp->forms_alloced*sizeof(int));
      curr_lsp->forms = realloc(curr_lsp->forms,
				curr_lsp->forms_alloced*sizeof(struct xcl_ilem *));
      if (curr_lsp->forms_used < 0)
	curr_lsp->forms_used = 0;
    }

  /* when curr_cell = 0 we are in a line with no cells; by definition,
     all content in such a line is in cell 2 (because cell 1 is the line
     number) */
  curr_lsp->cells[curr_lsp->forms_used] = (curr_cell ? curr_cell : 2);

  curr_lsp->forms[curr_lsp->forms_used] = xip = mb_new(xi_mem);
  xip->i = form;
  hash_add(word_form_index,npool_copy((unsigned char*)ref,lemline_xcp->pool),
	   xip /* &curr_lsp->forms[curr_lsp->forms_used]*/ /*form*/);

  ++curr_lsp->forms_used;
}

static const unsigned char *
lem_dynalem_lem(const char *lang, const char *formstr)
{
  static unsigned char *tmpbuf = NULL;
  static int tmpbuf_alloc = 0;
  const unsigned char *lem = NULL;
  if (NULL == lang)
    {
      free(tmpbuf);
      tmpbuf_alloc = 0;
      return NULL;
    }
  if (tmpbuf_alloc < (strlen(lang)+strlen(formstr)+3))
    {
      tmpbuf_alloc = tmpbuf_alloc ? 2*tmpbuf_alloc : 512;
      tmpbuf = realloc(tmpbuf, tmpbuf_alloc);
    }
  sprintf((char*)tmpbuf, "%%%s:%s", lang, formstr);
  if ((lem = hash_find(lem_dynalem_hash, tmpbuf)))
    return lem;
  else
    {
      if (!strcmp(formstr, "n")
	  || !strcmp(formstr, "(n)")
	  || (*(const unsigned char *)formstr < 128 && isdigit(*formstr)))
	return (const unsigned char *)"n";
      else if (!strcmp(formstr, "x")
	       || !strcmp(formstr, "(x)")
	       || (strstr(formstr, "..")
		   || strstr(formstr, "-x")
		   || strstr(formstr, "x-"))
	       ) {
	return (const unsigned char *)"u";
      } else {
	return (const unsigned char *)"X";
      }
    }
}

void
lem_save_form_dynalem(const char *ref, const char *lang, 
		      const char *formstr, struct lang_context *langcon,
		      const char *field)
{
  if (ref && *ref && formstr)
    {
      const unsigned char *dynalem = NULL;
      struct xcl_ilem /*ilem_form*/ *form = NULL;
      lem_save_form(ref, lang, formstr, langcon, field);
      dynalem = lem_dynalem_lem(lang, formstr); /* always returns at least 'u' or 'X' */
      form = hash_find(word_form_index, (const unsigned char *)ref);
      if (form)
	form->i->literal = (char*)npool_copy(dynalem,lemline_xcp->pool);
    }
}
static struct node *
find_w_node(struct node *n)
{
  int i;
  for (i = 0; i < n->children.lastused; ++i)
    {
      struct node *c = n->children.nodes[i];
      if (c)
	{
	  if (c->names
	      && (!strcmp(c->names->pname,"g:w")
		  || !strcmp(c->names->pname,"n:w"))
	      )
	    return c;
	  else if (c->children.lastused)
	    {
	      c = find_w_node(c);
	      if (c)
		return c;
	    }
	}
    }
  return NULL;
}

void
lem_save_lemma(struct node *wp, const char *lemma)
{
  struct xcl_ilem /*ilem_form*/ *form = NULL;
  unsigned const char *xmlid = NULL;
  while (isspace(*lemma))
    ++lemma;
  if (lemma && strlen(lemma))
    {
      struct node *wp2 = NULL;
      if (strcmp(wp->names->pname,"g:w")
	  && strcmp(wp->names->pname,"n:w"))
	wp2 = find_w_node(wp);
      if (wp2)
	wp = wp2;
      xmlid = getAttr(wp,"xml:id");
      form = hash_find(word_form_index, xmlid);
      if (form)
	form->i->literal = (char*)npool_copy((unsigned char *)lemma,lemline_xcp->pool);
      else
	vwarning("internal error: word_form_index lookup failed; lemma=%s; xml:id=%s", lemma, xmlid);
    }
  else
    warning("empty lemmatization entry");
}

void
lem_unform(void)
{
  if (curr_lsp)
    --curr_lsp->forms_used;
}

#define xuse_literal(xl) ((xl) && ((xl)[1] || ((xl)[0] != 'X' && (xl)[0] != 'L')))

#define use_literal(xl) (xl)

static void
lem_f2_serialize(FILE *fp, struct f2 *f2)
{
  if (BIT_ISSET(f2->flags, F2_FLAGS_NOT_IN_SIGS))
    {
      if (f2->pos)
	{
	  fputs((char*)f2->pos,fp);
	}
      else
	fputs("X",fp);
    }
  else
    {
      if (f2->cf || (f2->norm && strcmp((char*)f2->norm, "X")))
	{
	  if (strncmp((char*)f2->lang,"sux",3) && f2->norm)
	    fputs((char*)f2->norm,fp);
	  else
	    fputs((char*)f2->cf,fp);
	  if (f2->sense
	      && strncmp((const char *)f2->gw,"unmng",5)
	      && strcmp((const char *)f2->gw,"cnne")
	      && strcmp((const char *)f2->gw,"cvne")
	      && strcmp((const char *)f2->gw,"cvve"))
	    {
	      char *comma = NULL;
	      Uchar *tmp = (Uchar*)f2->sense;

	      if (lem_simplify)
		{
		  if ((comma = strchr((const char *)f2->sense, ',')))
		    *comma = '\0';
		  if (!strncmp((const char*)f2->sense, "(to be) ", 8))
		    tmp += 8;
		  else if (!strncmp((const char*)f2->sense, "to ", 3))
		    {
		      tmp += 3;
		      if (!strncmp((const char*)tmp, "be ", 3))
			tmp += 3;
		      else if (!strncmp((const char*)tmp, "make ", 5))
			tmp += 5;
		    }
		  if (tmp[strlen((const char *)tmp)-1] == ')')
		    {
		      unsigned char *end = tmp+strlen((const char *)tmp);
		      while (end > tmp)
			{
			  if (*--end == '(')
			    *end = '\0';
			}
		    }
		}

	      while (' ' == tmp[strlen((const char*)tmp)-1])
		tmp[strlen((const char*)tmp)-1] = '\0';

	      fprintf(fp,"[%s]",(char*)tmp);
	    }
	  else
	    fprintf(fp,"[%s]",f2->gw);
	  if (!lem_simplify)
	    {
	      if (f2->epos && strcmp((char*)f2->pos,(char*)f2->epos))
		fprintf(fp,"'%s",f2->epos);
	      else
		fputs((char*)f2->pos,fp);
	    }
	  else
	    {
	      if (f2->pos && f2->pos[1] == 'N')
		fputs((char*)f2->pos,fp);
	    }
	}
      else
	{
	  if (f2->pos)
	    fputs((char*)f2->pos,fp);
	  else
	    fputs("X",fp);
	}
    }
}

static void
lem_serialize_one(FILE *fp, struct ilem_form *f, struct serializer_frame *sfp)
{
  if (sfp->lit)
    fputs(sfp->lit,fp);
  else
    {
      if (*sfp->pre)
	fputs(sfp->pre, fp);
      lem_f2_serialize(fp, &f->f2);
      if (*sfp->pst)
	fputs(sfp->pst, fp);
    }
  
  if (f->ambig)
    {
      fputc('|', fp);
      lem_serialize_one(fp,f->ambig, sfp);
    }
}

static void
lem_serialize_one_xml(FILE *fp, struct ilem_form *f, struct serializer_frame *sfp)
{
  if (sfp->lit)
    {
      fputs((char*)xmlify((unsigned char*)sfp->lit),fp);
    }
  else
    {
      if (*sfp->pre)
	fputs((char*)xmlify((unsigned char *)sfp->pre), fp);

      /*warning("weird error: form->literal is NULL");*/
      if (f->f2.norm || f->f2.cf)
	{
	  if (strcmp((char*)f->f2.lang,"sux") && f->f2.norm)
	    fputs((char*)xmlify((unsigned char*)f->f2.norm),fp);
	  else
	    fputs((char*)xmlify(f->f2.cf),fp);
	  if (f->f2.sense)
	    fprintf(fp,"[%s]",(char*)xmlify((unsigned char *)f->f2.sense));
	  else
	    fprintf(fp,"[%s]",xmlify(f->f2.gw));
	  if (f->f2.epos && strcmp((char*)f->f2.pos,(char*)f->f2.epos))
	    fprintf(fp,"'%s",xmlify(f->f2.epos));
	  else
	    fputs((char*)xmlify(f->f2.pos),fp);
	}
      else if (f->f2.pos)
	{
	  fputs((char*)xmlify(f->f2.pos),fp);
	}
      else
	{
	  fputs("???",fp);
	}

      if (*sfp->pst)
	fputs((char*)xmlify((unsigned char *)sfp->pst), fp);
    }
  if (f->ambig)
    {
      fputc('|', fp);
      lem_serialize_one_xml(fp,f->ambig,sfp);
    }
}
void
lem_serialize(FILE *fp)
{
  int i;
  for (i = 0; i < lem_lines_used; ++i)
    {
      struct lem_save *lp;
      for (lp = &lem_lines[i]; lp; lp = lp->cont)
	fprintf(fp,"%s\n",lp->line);
      
      if (lem_lines[i].forms && *lem_lines[i].line != '#') /* suppress #lem: output for #note: @akk{pu} */
	{
	  int j;
	  fprintf(fp,"#lem: ");
	  for (j = 0; j < lem_lines[i].forms_used; ++j)
	    {
	      /* use the ilem_form that was processed by XCL */
	      struct ilem_form *f = lem_lines[i].forms[j]->x ? lem_lines[i].forms[j]->x->f : lem_lines[i].forms[j]->i;
	      static struct serializer_frame sframe;

	      if (BIT_ISSET(f->instance_flags, ILEM_FLAG_SPARSE_SKIP))
		continue;

	      set_sframe(&sframe, ((lem_autolem || lem_dynalem) && f->finds) ? NULL : f->literal);
	      if (sframe.lit)
		{
		  fputs(sframe.lit,fp);
		}
	      else
		{
#if 1 /* CHECK WHAT HAPPENS WITH n AND u */
		  if (!f->f2.pos || (*f->f2.pos == 'X' && !f->f2.pos[1]) || *f->f2.pos == 'L')
		    f->f2.pos = (unsigned char*)lem_base_type((char*)f->f2.form);
#endif
		  lem_serialize_one(fp,f,&sframe);
		  if (f->f2.parts)
		    {
#if 1
		      int k;
		      /* FIXME: parts should work with the sframe stuff */
		      for (k = 0; f->f2.parts[k]; ++k)
			{
			  fputc('&', fp);
			  lem_f2_serialize(fp, f->f2.parts[k]);
			}
#else
		      for (i = 0; f->f2.parts[i]; ++i)
			{
			  fputc('&', fp);
			  lem_f2_serialize(fp, f->f2.parts[i]);
			}
#endif
		    }
		}

	      if (j + 1 < lem_lines[i].forms_used)
		fputs("; ", fp);
	    }
	  fputc('\n',fp);
	  if (i < (lem_lines_used-1) && lem_lines[i+1].line && *lem_lines[i+1].line)
	    fputc('\n',fp);
	}
    }
}

void
lem_ods_serialize(FILE *fp)
{
  extern int ods_cols;
  /*extern char *odsods_fn;*/
  int i;
  fprintf(fp,"<lem cols=\"%d\">", ods_cols);
  for (i = 0; i < lem_lines_used; ++i)
    {
      if (lem_lines[i].forms)
	{
	  int j;
	  fprintf(fp,"<l n=\"%d\">",lem_lines[i].lnum);
	  for (j = 0; j < lem_lines[i].forms_used; ++j)
	    {
	      struct ilem_form *f = lem_lines[i].forms[j]->x->f;
	      fprintf(fp,"<c n=\"%d\">",lem_lines[i].cells[j]);
	      if (use_literal(f->literal))
		{
		  fputs((char*)xmlify((unsigned char*)f->literal),fp);
		}
	      else
		{
		  static struct serializer_frame sframe;
		  set_sframe(&sframe, f->literal);

		  if (!f->f2.pos || (*f->f2.pos == 'X' && !f->f2.pos[1]) || *f->f2.pos == 'L')
		    f->f2.pos = (unsigned char*)lem_base_type((char*)f->f2.form);
		multi_entry:
		  lem_serialize_one_xml(fp,f,&sframe);
		  if (f->multi)
		    {
		      f = f->multi;
		      fputs("&amp;", fp);
		      goto multi_entry;
		    }
		}
#if 1
	      fputs("</c>",fp);
#else
	      if (j + 1 < lem_lines[i].forms_used)
		fputs("; ", fp);
#endif
	    }
	  fputs("</l>\n",fp);
	}
    }
  fputs("<xi:include xmlns:xi=\"http://www.w3.org/2001/XInclude\""
	" href=\"orig-content.xml\"/></lem>", 
	fp);
}

#if 0
void
lem_psu(char *psuline)
{
  char *psu_ngram, *psu_sig;
  
  if (!currlang->runsigs)
    {
      currlang->runsigs = mb_new(lemline_xcp->sigs->mb_sigsets);
      /* other runsigs initialization */
    }
  psu_ngram = psuline;
  while (*psu_ngram && !isspace(*psu_ngram))
    ++psu_ngram;
  while (isspace(*psu_ngram))
    ++psu_ngram;
  if (psu_sig)
    {
      char *psu_ngram_end = psu_sig;
      while (psu_ngram_end > psu_ngram && isspace(psu_ngram_end[-1]))
	--psu_ngram_end;
      *psu_ngram_end = '\0';
      psu_sig += 2;
      while (isspace(*psu_sig))
	++psu_sig;
      psu_register(file,lnum,currlang->runsigs,currlang,psu_ngram,NULL);
    }
}
#endif

#if 0
      if (fp->literal)
	{
	  if ('X' == *fp->literal && (!fp->literal[1] || isspace(fp->literal[1])))
	    fp->literal = NULL;
	}
      else if (fp->finds[0]->literal)
	{
	  if ('X' != *fp->literal && (!fp->literal[1] || !isspace(fp->literal[1])))
	    fp->literal = fp->finds[0]->literal;
	}
#endif

static void
set_sframe(struct serializer_frame *sfp, char *literal)
{
  char *X = NULL;
  if (!literal || ('X' == literal[0] && '\0' == literal[1]))
    {
      sfp->pre = sfp->pst = "";
      sfp->lit = NULL;
    }
  else
    {
      int found_X = 0;
      X = literal;
      while ((X = strchr(X, 'X')))
	{
	  if (X == literal && isspace(X[1]))
	    {
	      sfp->pre = "";
	      sfp->pst = X+1;
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else if (!X[1] && isspace(X[-1]))
	    {
	      *X = '\0';
	      sfp->pre = literal;
	      sfp->pst = "";
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else if (isspace(X[-1]) && isspace(X[1]))
	    {
	      sfp->pre = literal;
	      *X = '\0';
	      sfp->pst = X+1;
	      sfp->lit = NULL;
	      found_X = 1;
	      break;
	    }
	  else
	    {
	      ++X;
	    }
	}
      if (!found_X)
	{
	  sfp->pre = sfp->pst = "";
	  sfp->lit = literal;
	}      
    }
}

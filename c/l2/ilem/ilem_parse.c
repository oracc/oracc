#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <psd_base.h>
#include "npool.h"
#include "memblock.h"
#include "atf.h"
#include "charsets.h"
#include "xcl.h"
#include "lemline.h"
#include "warning.h"
#include "f2.h"
#include "ilem_form.h"
#include "ilem_para.h"
#include "props.h"
#include <lng.h>
#include <atf2utf.h>

extern int bootstrap_mode;
extern int ngram_obey_lines;

#define cc(s) ((const char *)s)
#define uc(s) ((unsigned char *)s)
#define xstrcat(a,b) (unsigned char *)strcat((char *)a,(char*)b)
#define xstrcpy(a,b) (unsigned char *)strcpy((char *)a,(char*)b)

int lem_unicode = 0;
extern int ignore_plus;
static char post_lem_sentence;
static unsigned char lem_attr_buf[1024], *lem_next_lem;
static unsigned char lem_alt_buf[1024], *lem_next_alt;
static char lem_iflags[256];

#ifdef  CHARCONV
static unsigned char *lem_utf8(const unsigned char *lem, const char *f, ssize_t l);
#endif/*CHARCONV*/

static void lem_init(const unsigned char *lemma_attr);
static unsigned char *lem_next(struct xcl_context *xc);
static void alt_init(const unsigned char *lem);
static unsigned char *alt_next(struct xcl_context *xc);
static unsigned char *lem_end(unsigned char *lem);

static const unsigned char *
ilem_conv(struct xcl_l *l, const unsigned char *str)
{
  const unsigned char *x = NULL;
  if (str)
    {
      int entry_chartrie_er = chartrie_suppress_errors;
      curr_lang_ctxt = l->f->lang;
      chartrie_suppress_errors = 1;
#if 1
      x = atf2utf(mloc_file_line(l->xc->file,l->lnum), str,0);
#else
      x = natf2utf((char*)str,(char*)str+strlen((char*)str),0,l->xc->file,l->lnum);
#endif
      if (strcmp((char*)x,(char*)str))
	str = npool_copy(x,l->xc->pool);
      chartrie_suppress_errors = entry_chartrie_er;
    }
  return str;
}

static char *
make_inst(struct xcl_context *xc, struct ilem_form *ifp)
{
  char buf[1024];
  sprintf(buf,"%%%s:%s=",ifp->f2.lang,ifp->f2.form);
  if (ifp->sublem)
    strcat(buf,ifp->sublem);
  return (char*)npool_copy((unsigned char*)buf,xc->pool);
}

static int
check_cf(const char *f, size_t lnum, const char *cf)
{
  const char *cf_orig = cf;
  if (!cf)
    return 0;

  while (*cf)
    if (isspace(*cf))
      {
	vwarning2(f, lnum, "%s: spaces not allowed in CF (lemmatize components not PSUs)", cf_orig);
	return 1;
      }
    else
      ++cf;

  return 0;
}

/* caller should now resolve word_id against word_form_index before
   calling and pass the result as form arg if non-NULL; NULL arg means
   form is embedded in lemma */
void
ilem_parse(struct xcl_context *xc, struct xcl_ilem /*ilem_form*/ *xi, int first_word)
{
  unsigned char *lem;
  int newflag = 0;
  extern const char *phase;
  unsigned char *lemma = NULL;
  extern int lem_dynalem, use_ilem_conv;
  
#define LANGBUF_LEN 32
  char langbuf[LANGBUF_LEN+1];

#if 0
#define FORMBUF_LEN 128
  char formbuf[FORMBUF_LEN+1];
#endif
  
  struct xcl_l *master_lp = NULL;
  struct ilem_form *master_formp = NULL;

  if (!xc)
    {
      vwarning("internal error: ilem_parse called with NULL args");
      return;
    }

  if (!xi || !xi->i)
    {
      /* this can happen after ATF parse errors */
      return;
    }
  else
    master_formp = xi->i;

  phase = "lem";

  /*#define  lemma   (master_formp->literal)*/

  if (master_formp->literal)
    {
      lemma = npool_copy((unsigned char *)master_formp->literal, xc->pool);
    }
  else
    {
      struct xcl_l*lp = xcl_lemma(xc,NULL,master_formp->ref,master_formp,NULL,0);
      lp->lnum = master_formp->lnum;
      /*lp->f = master_formp;*/ /*now done in xcl_lemma*/
      lp->inst = make_inst(xc,lp->f);
      phase = NULL;
      xi->x = lp;
      return;
    }

  if (NULL == master_formp->f2.lang)
    {
      if ('%' == *lemma)
	{
	  char *langbufp = langbuf;
	  for (++lemma; *lemma != ':' && *lemma != '-'; )
	    {
	      if (langbufp - langbuf == LANGBUF_LEN)
		{
		  langbuf[LANGBUF_LEN] = '\0';
		  vwarning2(file,lnum,"[91]: lang starting with '%s' is too long (MAX %d)",langbuf,LANGBUF_LEN);
		  phase = NULL;
		  return;
		}
	      else
		*langbufp++ = *lemma++;
	    }
	  if ('-' == *lemma)
	    {
	      while (*lemma && ':' != *lemma)
		++lemma;
	      if (!*lemma)
		{
		  vwarning2(file,lnum,"[92]: lang starting with '%s' has no ':'",langbuf);
		  phase = NULL;
		  return;
		}
	    }
	}
      else
	{
	  vwarning2(file,lnum,"[96]: no lang set for form");
	  phase = NULL;	  
	  return;
	}
      master_formp->f2.lang = npool_copy((unsigned char *)langbuf,xc->pool);
      master_formp->f2.core = langcore_of(langbuf);
    }
  else if ('%' == *lemma && '%' != lemma[1])
    {
      while (*lemma && ':' != *lemma)
	++lemma;
      if (':' != *lemma)
	{
	  vwarning2(file,lnum,"lang has no ':'");
	  return;
	}
      ++lemma;
    }

  /* Now we know that lemma points to the start of the lemmatization */
  lem_init((const unsigned char *)lemma);

  /* This outer loop splits on '&' */
  while (1)
    {
      struct xcl_l*lp;
      int alt_count = 0;
      int iflags = 0;
      struct ilem_form *curr_f = NULL;
  
      lem = lem_next(xc);
      if (!lem)
	break;

      lp = xcl_lemma(xc,NULL,master_formp->ref,master_formp,NULL,0);
      lp->inst = master_formp->literal;
      lp->lnum = lnum;
      lp->ante_para = ilem_para_parse(xc, lem, &lem,master_formp->lnum, ilem_para_pos_ante, lp);
      if (lem)
	{
	  unsigned char *post = NULL;
	  while (isspace(*lem))
	    ++lem;
	  post = lem_end(lem);
	  lp->post_para = ilem_para_parse(xc, post,NULL,master_formp->lnum, ilem_para_pos_post, lp);
	  if (isspace(*post))
	    {
	      while (post > lem && isspace(post[-1]))
		--post;
	      *post = '\0';
	    }
	  ilem_para_boundaries(lp,xc);
	}
      else
	{
	  vwarning2(file,master_formp->lnum,"[96]: lem `%s' failed syntax stripping",lem);
	  break;
	}

      alt_init(lem);

      if (master_formp->mcount)
	{
	  struct ilem_form *mrover = NULL;
	  /*lp->f = NULL;*/ /* NEW ILEM_FORM  form_allocator();*/
	  lp->f = mb_new(xc->sigs->mb_ilem_forms);
	  lp->f->f2.owner = lp->f;
	  lp->f->newflag = newflag;
	  lp->f->f2.lang = master_formp->f2.lang;
	  lp->f->f2.core = master_formp->f2.core;
	  lp->f->mcount = -1;
	  if (master_formp->mcount == 1)
	    {
	      master_formp->type = "cof-head";
	      /* This master_lp set is suspect */
	      if (!master_lp)
		master_lp = lp;
	      master_lp->cof_tails = list_create(LIST_SINGLE);
	    }
	  lp->f->type = "cof-tail";
	  lp->cof_head = master_lp;
	  list_add(lp->cof_head->cof_tails, lp);

	  ++master_formp->mcount;
	  /* efficiency doesn't matter here as we will have relatively 
	     few of these */
	  for (mrover = master_formp; mrover->multi; mrover = mrover->multi)
	    ;
	  mrover->multi = lp->f;
	  /*lp->f->master = master_formp;*/
	  lp->f->file = master_formp->file;
	  lp->f->lnum = master_formp->lnum;
	  lp->ref = lp->f->ref = master_formp->ref;
	  lp->f->f2.form = master_formp->f2.form;
	  lp->f->literal = NULL;
	}
      else
	{
	  lp->f = master_formp;
	  lp->f->mcount = 1;
	  lp->f->newflag = newflag;
	  lp->ref = lp->f->ref;
	  lp->f->type = NULL;
	  xi->x = master_lp = lp;
	}

      /* This inner loop splits on '|'; it is where each lemma is actually
	 handled */
      while (1)
	{
	  lem = alt_next(xc);
	  if (!lem)
	    break;
	  iflags = 0;

	  while (lem_iflags[*lem])
	    {
	      switch (*lem)
		{
		case '+':
		  ++lem;
		  /*newflag = !ignore_plus; */
		  BIT_SET(iflags, F2_FLAGS_LEM_NEW);
		  break;
		case '!':
		  ++lem;
		  BIT_SET(iflags, F2_FLAGS_PSU_STOP);
		  break;
		case '-':
		  ++lem;
		  BIT_SET(iflags, F2_FLAGS_PSU_SKIP);
		  break;
		case '`':
		  lem = (unsigned char *)"X";
		  break;
		}
	    }

	  if (first_word && lem_dynalem)
	    BIT_SET(iflags, F2_FLAGS_PSU_STOP);
	  
	  if (bootstrap_mode && !BIT_ISSET(iflags, F2_FLAGS_LEM_NEW))
	    BIT_SET(iflags, F2_FLAGS_LEM_NEW);

	  if (BIT_ISSET(iflags,F2_FLAGS_LEM_NEW))
	    {
	      char *tmp = malloc(strlen((const char *)lem) + 2);
	      sprintf(tmp, "+%s", lem);
	      lem = npool_copy((unsigned char *)tmp, xc->pool);
	      free(tmp);
	    }

	  if (alt_count++)
	    {
	      struct ilem_form *last_alt = NULL, *f = NULL;
	      if (!lem)
		break;

	      /*f->f2 = NULL form_allocator();*/
	      f = mb_new(xc->sigs->mb_ilem_forms);
	      f->f2.owner = f;
	      /* f->newflag = newflag; */
	      lp->f->ref = master_formp->ref;
	      f->instance_flags = iflags;
	      f->f2.lang = master_formp->f2.lang;
	      f->f2.core = master_formp->f2.core;
	      f->f2.form = master_formp->f2.form;
	      if (BIT_ISSET(iflags, F2_FLAGS_LEM_NEW))
		{
		  BIT_SET(f->f2.flags, F2_FLAGS_LEM_NEW);
		  if ('+' == *lem) /* should always be true */
		    ++lem;
		}
	      f->lnum = master_formp->lnum;
	      f->file = master_formp->file;
	      f->instance_flags = iflags;
	      f->sublem = (char*)npool_copy(lem,xc->pool);

	      /* link this into the master_formp */
	      for (last_alt = master_formp; 
		   last_alt->ambig; 
		   last_alt = last_alt->ambig)
		;
	      curr_f = last_alt->ambig = f;
	    }
	  else
	    {
	      lp->f->sublem = (char*)npool_copy(lem,xc->pool);
	      lp->f->instance_flags = iflags;
	      curr_f = lp->f;
	      if (BIT_ISSET(iflags, F2_FLAGS_LEM_NEW))
		{
		  BIT_SET(curr_f->f2.flags, F2_FLAGS_LEM_NEW);
		  if ('+' == *lem) /* should always be true */
		    ++lem;
		}
	    }

	  /* Instance parsing cannot result in a form with && being
	     processed using f2_parse_cof, so we can just pass a NULL
	     final argument; 2024-10-18: this turns out to be a lie so
	     pass xc->sigs */
	  f2_parse((Uchar*)lp->f->file, lp->f->lnum, lem, &curr_f->f2, 
		   (Uchar**)&curr_f->psu_sense, xc->sigs);

	  if (check_cf((char*)lp->f->file, lp->f->lnum, (char*)curr_f->f2.cf))
	    BIT_SET(curr_f->f2.flags, F2_FLAGS_INVALID);

	  if (use_ilem_conv && curr_f->lang)
	    {
	      curr_lang_ctxt = curr_f->lang;
	      if (!BIT_ISSET(curr_f->f2.flags,F2_FLAGS_CF_QUOTED))
		curr_f->f2.cf = ilem_conv(lp,curr_f->f2.cf);
	      curr_f->f2.norm = ilem_conv(lp,curr_f->f2.norm);
	      curr_f->f2.base = ilem_conv(lp,curr_f->f2.base);
	      curr_f->f2.cont = ilem_conv(lp,curr_f->f2.cont);
	    }
	  curr_f->sublem = make_inst(xc,curr_f);
	}
    }
}

static void
lem_init(const unsigned char *lemma_attr)
{
  (void)xstrcpy(lem_attr_buf,lemma_attr);
  if (*lemma_attr)
    lem_next_lem = lem_attr_buf;
  else
    lem_next_lem = NULL;
  lem_iflags['+'] = lem_iflags['!'] = lem_iflags['-'] = lem_iflags['`'] = 1;
}

static unsigned char *
lem_next(struct xcl_context *xc)
{
  unsigned char *this_lem = lem_next_lem;
  int saw_square = 0;
  if (this_lem)
    {
      lem_next_lem = this_lem;
      while (*lem_next_lem)
	if ('&' == *lem_next_lem
	    && saw_square
	    && (lem_next_lem[-1] != '\\'
		&& lem_next_lem[-1] != '+' && lem_next_lem[-1] != '-'))
	  {
	    *lem_next_lem++ = '\0';
	    if (lem_next_lem[-2] == '.')
	      {
		post_lem_sentence = lem_next_lem[-3];
		/* fprintf(stderr,"post_lem_sentence = %c\n",post_lem_sentence); */
		lem_next_lem[-3] = '\0';
	      }
	    break;
	  }
	else if ('[' == *lem_next_lem)
	  {
	    ++saw_square;
	    ++lem_next_lem;
	  }
	else
	  ++lem_next_lem;
      if (*lem_next_lem == '\0')
	lem_next_lem = NULL;
      return npool_copy(this_lem, xc->pool);
    }
  else
    return this_lem;
}

static void
alt_init(const unsigned char *lem)
{
  (void)xstrcpy(lem_alt_buf,lem);
  if (*lem_alt_buf)
    lem_next_alt = lem_alt_buf;
  else
    lem_next_alt = NULL;
}

static unsigned char *
alt_next(struct xcl_context *xc)
{
  unsigned char *this_lem = lem_next_alt;
  if (this_lem)
    {
      lem_next_alt = this_lem;
      while (*lem_next_alt)
	if ('|' == *lem_next_alt 
	    && (lem_next_alt > this_lem && lem_next_alt[-1] != '\\')
	    && lem_next_alt[-1] != '.'
	    && lem_next_alt[-1] != '-'
	    && lem_next_alt[-1] != '}'
	    && lem_next_alt[-1] != '('
	    && lem_next_alt[1]
	    && lem_next_alt[1] != '.'
	    && lem_next_alt[1] != '-'
	    && lem_next_alt[1] != '{'
	    && lem_next_alt[1] != ')'
	    )
	  {
	    *lem_next_alt++ = '\0';
	    break;
	  }
	else
	  ++lem_next_alt;
      if (*lem_next_alt == '\0')
	lem_next_alt = NULL;
      return npool_copy(this_lem, xc->pool);
    }
  else
    return NULL;
}

static unsigned char *
lem_end(unsigned char *lem)
{
  unsigned char *s;

  if ((s = (unsigned char *)strchr((char*)lem,'[')))
    {
      while (*s && (']' != *s || '\\' == s[-1]))
	++s;
      if (*s && s[1] == '|')
	return s+1;
      else
	{
	  while (*s && !isspace(*s))
	    ++s;
	  return s;
	}
    }
  else
    {
      while (*lem && !isspace(*lem))
	++lem;
      return lem;
    }
}

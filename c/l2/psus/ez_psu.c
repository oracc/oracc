#include <ctype128.h>
#include <string.h>
#include "../cdf/warning.h"
#include "list.h"
#include "ngram.h"
#include "pool.h"
#include "lem.h"
#include "xff.h"

#define xstrlen(s)   strlen((const char *)s)
#define xstrstr(a,b) (unsigned char*)strstr((const char*)a,(const char*)b)

extern int lnum;
static Hash_table *psu_save_lists;
struct NL **ez_psu_nlps;

struct ez_psu_save
{
  unsigned char *line;
  size_t         lnum;
  struct lang *  lang;
};

/*
 * EZ PSU's are given in one of two syntaxes:
 *
 * 1) To declare a new PSU:
 * 
 * #psu: %akk ina[in]PRP + irtu[chest]N => ina_irat[opposite]PRP
 *
 * 2) To declare new data for an existing PSU:
 *
 * #psu: [[%LANG]? [FORM] =]? [LEMMA]
 *
 * where [LEMMA] can be a full lemmatization with SENSE, NORM0,
 * or any other data specifiable in a lemmatization.
 *
 * e.g. (full form):
 *
 * #psu: %akk i-na i-rat = ina_irat[opposite//against]PRP$ina_irat
 *
 * or, if the FORM is already known and the LANG is the default 
 * for the line:
 * 
 * #psu: ina_irat[opposite//against]PRP$
 *
 * Note:
 * (a) that in contexts where the LEMMA parser expects a single
 *     word, underscores must be used instead of spaces.
 * (b) that spaces are required in the operators ' + ', ' = ' 
 * and ' => '.
 */

struct ez_psu
{
  char *         lang;
  enum langcode  code;
  unsigned char *form;
  unsigned char *operator;
  unsigned char *parts;
  unsigned char *lemma;
};

int ez_psus = 0;
static unsigned char *ez_psu_create_input(struct ez_psu *psave);
static void ez_psu_process_save(struct ez_psu_save *p, 
				struct xcl_context *xc, 
				struct NL *nlp,
				const char *lang);
static int ez_psu_parse(struct ez_psu_save *psave, struct ez_psu*p);

struct NL *
ez_psu_nlp(struct xcl_context *xc, const char *l, int n)
{
  struct lang *ltok = langtok(l,strlen(l));
  if (!ez_psu_nlps)
    ez_psu_nlps = calloc(c_count, sizeof(struct NL_context*));
  if (ltok)
    {
      enum langcode c = ltok->code;
      if (!ez_psu_nlps[c])
	{
	  ez_psu_nlps[c] = nl_setup(&xc->psus[c], nlcp_action_psu, l);
	  ez_psu_nlps[c]->parsed_ngrams = calloc(n, sizeof(struct NLE));
	}
      return ez_psu_nlps[c];
    }
  return NULL;
}

void
ez_psu_process_save_list(struct xcl_context *xc, const char *lang)
{
  List *psu_save_list = hash_find(psu_save_lists,(const unsigned char *)lang);
  ++ez_psus;
  if (list_len(psu_save_list))
    {
      struct ez_psu_save *psave;
      struct NL *nlp;

      if ((nlp = ez_psu_nlp(xc,lang,list_len(psu_save_list))))
	{
	  for (psave = list_first(psu_save_list);
	       psave; 
	       psave = list_next(psu_save_list))
	    ez_psu_process_save(psave,xc,nlp,lang);
	  list_free(psu_save_list, list_xfree);
	  psu_save_list = NULL;
	  (void)ez_psu_create_input(NULL);
	}
    }
}

static unsigned char *
ez_psu_create_input(struct ez_psu *p)
{
  static unsigned char *buf = NULL;
  static int buflen = 0;
  if (!p)
    {
      if (buf)
	free(buf);
      buf = NULL;
      buflen = 0;
      return NULL;
    }
  if (xstrlen(p->parts)+xstrlen(p->lemma)+5 >= buflen)
    {
      if (buflen)
	buflen *= 2;
      else
	buflen = 512;
      buf = realloc(buf,buflen);
    }
  sprintf((char*)buf,"%s += %s",p->parts,p->lemma);
  return buf;
}

static void
ez_psu_process_save(struct ez_psu_save *psave, 
		    struct xcl_context *xc,
		    struct NL *nlp,
		    const char *lang)
{
  struct ez_psu p;
  memset(&p,'\0',sizeof(struct ez_psu));
  p.lang = (char*)lang;
  if (ez_psu_parse(psave,&p))
    return;

  /* for +=, add the PSU spec to the relevant ngrams list */
  if (*p.operator == '+')
    {
      struct form *fp = new_persistent_form();
      nl_set_location(file, psave->lnum);
      parse_ngram_line(nlp, 
		       (char*)ez_psu_create_input(&p), 
		       nlp->nngrams, nlp->owner, NULL);
      ++nlp->nngrams;
      /* then create a form structure and add it to the cache */
      fp->lang = p.lang;
      fp->file = (char*)file;
      fp->lnum = psave->lnum;
      lemparse(p.lemma,fp);
      update_cache(xc,fp);
    }
  else
    {
      /* for =, find the form structure in cache or dictionary and
	 clone it so we can add the new information
       */
    }
}

static int
ez_psu_parse(struct ez_psu_save *psave, struct ez_psu*p)
{
  unsigned char *line = psave->line;

  while (isspace(*line))
    ++line;
#if 0
  if ('%' == *line)
    {
      p->lang = (char*)line;
      while (*line && !isspace(*line))
	++line;
      if (*line)
	{
	  *line++ = '\0';
	  while (isspace(*line))
	    ++line;
	}
      else
	{
	  vwarning2(file,psave->lnum,"nothing after %LANG in #psu: spec");
	  return 1;
	}
    }
#endif
  if ((p->operator = xstrstr(line," = "))
      || (p->operator = xstrstr(line," += ")))
    {
      *p->operator++ = '\0';
      p->lemma = p->operator;
      while (*p->lemma)
	{
	  if (isspace(*p->lemma))
	    {
	      *p->lemma++ = '\0';
	      break;
	    }
	  else
	    ++p->lemma;
	}
      if (!*p->lemma)
	{
	  vwarning2(file,psave->lnum,"no LEMMA in #psu: spec");
	  return 1;
	}
      if (p->operator[1]) /* => */
	p->parts = line;
      else
	p->form = line;
    }
  else
    {
      p->lemma = line;
    }
  line += xstrlen(line);
  while (isspace(line[-1]))
    --line;
  if (*line)
    *line = '\0';
  if (p->operator && p->operator[1])
    {
      if (!p->parts || !*p->parts)
	{
	  vwarning2(file,psave->lnum,"no PARTS in #psu: => spec");
	  return 1;
	}
    }
  else
    {
      if (!p->form || !*p->form)
	{
	  vwarning2(file,psave->lnum,"no FORM in #psu: = spec");
	  return 1;
	}
    }
  return 0;
}

void
ez_psu_store(unsigned char *line, struct lang *lang)
{
  struct ez_psu_save *psave = malloc(sizeof(struct ez_psu_save));
  List *lp = NULL;
  const char *langname = NULL;

  if (lang && lang->name)
    langname = lang->name;
  
  while (isspace(*line))
    ++line;
  if (*line == '%')
    {
      langname = (char*)++line;
      while (*line && !isspace(*line))
	++line;
      if (*line)
	*line++ = '\0';
    }

  if (!psu_save_lists)
    psu_save_lists = hash_create(1);
  if ((lp != hash_find(psu_save_lists,(unsigned char *)langname)))
    {
      lp = list_create(LIST_SINGLE);
      hash_add(psu_save_lists,(unsigned char *)langname,lp);
    }
  psave->line = pool_copy(line);
  psave->lnum = lnum;
  psave->lang = lang;
  if (lp)
    list_add(lp, psave);
}

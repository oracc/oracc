#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype128.h>
#include "warning.h"
#include "memblock.h"
#include "hash.h"
#include "list.h"
#include "pool.h"
#include "atf.h"
#include "charsets.h"
#include "ngram.h"
#include "loadfile.h"

#define CF_BLOCK_SIZE    16
#define PRED_BLOCK_SIZE  16

struct mb *nl_context_mem;
struct mb *nl_mem;
struct mb *nle_mem;
struct mb *ngram_lines_mem;
struct mb *cf_mem;
struct mb *pred_mem;

extern int use_unicode;

const char *nl_file;
int nl_lnum;

/* This is lazy ... */
static unsigned char *
add_str(char *dest, const char *src)
{
  if (!src || !strlen(src))
    return (unsigned char *)dest;

  if (dest)
    {
      strcat(dest, " ");
      strcat(dest, src);
    }
  else
    {
      dest = malloc(1024);
      strcpy(dest,src);
    }
  return (unsigned char *)dest;
}

static unsigned char *
fin_str(const char *str, struct npool *pool)
{
  if (str)
    {
      unsigned char *tmp = npool_copy((unsigned char *)str,pool);
      free((char*)str);
      return tmp;
    }
  else
    return NULL;
}

static struct f2 *
parse_psu(struct NLE *nlep, List *components, unsigned char *ngm)
{
  unsigned char *sig;
  int i = 0;

  nlep->psu_form = mb_new(nlep->owner->owner->owner->owner->mb_f2s);
  /* PSU's don't use the && notation for COFs, so we can pass a NULL
     final arg here safely */
  f2_parse((unsigned char*)nlep->owner->file,
	   nlep->lnum,
	   npool_copy((unsigned char *)nlep->psu,nlep->owner->owner->pool),
	   nlep->psu_form, NULL,
	   NULL);
  nlep->psu_form->file = (unsigned char *)nlep->owner->file;
  nlep->psu_form->lnum = nlep->lnum;
  nlep->psu_form->lang = (unsigned char *)nlep->owner->owner->lang;
  nlep->psu_form->core = langcore_of((const char*)nlep->psu_form->lang);
  for (i = 0, sig = list_first(components); sig; sig = list_next(components), ++i)
    {
      struct f2 *f = mb_new(nlep->owner->owner->owner->owner->mb_f2s);
      f2_parse((unsigned char*)nlep->owner->file,
	       nlep->lnum,
	       npool_copy(sig,nlep->owner->owner->pool),
	       f, NULL, NULL);
      nlep->cfs[i]->f2->form = f->form;
      nlep->psu_form->form = add_str((char*)nlep->psu_form->form, (char*)f->form);
      nlep->psu_form->norm = add_str((char*)nlep->psu_form->norm, (char*)f->norm);
      nlep->psu_form->base = add_str((char*)nlep->psu_form->base, (char*)f->base);
      nlep->psu_form->cont = add_str((char*)nlep->psu_form->cont, (char*)f->cont);
      nlep->psu_form->morph = add_str((char*)nlep->psu_form->morph, (char*)f->morph);
      nlep->psu_form->morph2 = add_str((char*)nlep->psu_form->morph2, (char*)f->morph2);
      if (!nlep->psu_form->project)
	nlep->psu_form->project = f->project;
      components->rover->data = f;
    }
  nlep->psu_form->psu_ngram = ngm;
  nlep->psu_form->parts = (struct f2 **)list2array(components);
  nlep->psu_form->form = fin_str((char*)nlep->psu_form->form,nlep->owner->owner->pool);
  nlep->psu_form->norm = fin_str((char*)nlep->psu_form->norm,nlep->owner->owner->pool);
  nlep->psu_form->base = fin_str((char*)nlep->psu_form->base,nlep->owner->owner->pool);
  nlep->psu_form->cont = fin_str((char*)nlep->psu_form->cont,nlep->owner->owner->pool);
  nlep->psu_form->morph = fin_str((char*)nlep->psu_form->morph,nlep->owner->owner->pool);
  nlep->psu_form->morph2 = fin_str((char*)nlep->psu_form->morph2,nlep->owner->owner->pool);
  return nlep->psu_form;
}

struct CF *
new_cf(List *lp)
{
  list_add(lp, mb_new(cf_mem));
  return list_last(lp);
}

struct PRED *
new_pred(List *lp)
{
  list_add(lp, mb_new(pred_mem));
  return list_last(lp);
}

static void
nleps_add(Hash_table *hash, struct NLE*nlep)
{
  struct NLE_set *nleps;
  if (!(nleps = hash_find(hash,(unsigned char *)nlep->cfs[0]->cf)))
    {
      hash_add(hash,(unsigned char *)nlep->cfs[0]->cf,
	       nleps = calloc(1,sizeof(struct NLE_set)));
      nleps->pp = calloc(nleps->pp_alloced=8, sizeof(struct NLE*));
    }
  else
    {
      if (nleps->pp_used == nleps->pp_alloced)
	nleps->pp = realloc(nleps->pp,(nleps->pp_alloced+=8)*sizeof(struct NLE*));
    }
  nleps->pp[nleps->pp_used++] = nlep;
}

struct f2 *
parse_ngram_line(struct NL*nlp, const char *line, int ngram_index, 
		 struct NL_context*nlcp, List *components, void *user)
{
  struct NLE *nlep = mb_new(nle_mem);
  char *end_cts, *tts, *psu = NULL;
  int implicit_tts = 0;
  struct f2 *psu_form = NULL;
  /*char *cts_save = NULL;*/
  /*int cts_len = 0;*/

  nlep->file = nl_file;
  nlep->lnum = nl_lnum;
  nlep->owner = nlp;
  nlep->user = user;
  nlep->line = (char*)npool_copy((unsigned char *)line, nlcp->pool);
  nlep->chop = (char*)npool_copy((unsigned char *)line, nlcp->pool);
  nlep->priority = nlcp->nngrams++;

  if (!(tts = strstr(nlep->chop,"=>")))
    {
      implicit_tts = 1;
      if ((psu = strstr(nlep->chop, "+=")))
	end_cts = psu;
      else
	end_cts = nlep->chop + strlen(nlep->chop);
    }
  else
    {
      end_cts = tts;
      tts += 2;
      while (isspace(*tts))
	++tts;
      psu = strstr(tts,"+=");
    }

  if (nl_parse_cts(nlep->chop,end_cts,nlep, 0))
    return NULL;

  if (!(nlep->implicit_tts = implicit_tts))
    nl_parse_cts(tts, psu ? psu : (tts+strlen(tts)), nlep, 1);

  if (psu)
    {
      psu += 2;
      while (isspace(*psu))
	++psu;
      nlep->psu = psu;
      psu_form = parse_psu(nlep, components, npool_copy((unsigned char*)line, nlcp->pool));
    }

  nleps_add(nlcp->active_hash,nlep);
  if (nlp->parsed_ngrams)
    {
      nlp->last_parsed_ngram->next = nlep;
      nlp->last_parsed_ngram = nlep;
    }
  else
    {
      nlp->parsed_ngrams = nlp->last_parsed_ngram = nlep;
    }

  return psu_form;
}

struct NL_context *
nl_new_context(struct sigset *sp, enum nlcp_actions act, const char *lang)
{
  struct NL_context *nlcp = NULL;
  nlcp = mb_new(nl_context_mem);
  nlcp->owner = sp;
  nlcp->pool = npool_init();
  nlcp->posNgrams = hash_create(1024);
  nlcp->psuNgrams = hash_create(1024);
  if (act == nlcp_action_rewrite)
    nlcp->active_hash = nlcp->posNgrams;
  else
    nlcp->active_hash = nlcp->psuNgrams;
  nlcp->lang = lang;
  /*nlcp->negNgrams = hash_create(1024);*/

  nlcp->action = act;

  if (act == nlcp_action_psu)
    nlcp->func = links_psu;

  return nlcp;
}

void
nl_free_context(struct NL_context *nlcp)
{
  if (nlcp->posNgrams)
    {
      hash_free(nlcp->posNgrams, NULL);
      nlcp->posNgrams = NULL;
    }
  if (nlcp->psuNgrams)
    {
      hash_free(nlcp->psuNgrams, NULL);
      nlcp->psuNgrams = NULL;
    }
}

struct NL*
nl_new_nl(struct NL_context *nlcp)
{
  struct NL *nlp = NULL, *tmp = NULL;

  if (!nlcp)
    return NULL;

  nlp = mb_new(nl_mem);
  if (nlcp->nlp)
    {
      for (tmp = nlcp->nlp; tmp->next; tmp = tmp->next)
	;
      tmp->next = nlp;
    }
  else
    nlcp->nlp = nlp;

  nlp->owner = nlcp;
  nlp->file = (char*)npool_copy((unsigned char*)nl_file, nlcp->pool);
  nlp->initial_line = nlcp->nngrams;

  return nlp;
}

struct f2 *
nl_process_one_line(struct NL *nlp, const char *lp, List *components)
{
  struct f2 *psu_form = NULL;
  if (!isspace(*lp) && '#' != *lp)
    {
      switch (*lp)
	{
	case ' ':
	case '\t':
	case '#':
	  break;
	case '@':
	  vwarning2(nl_file,nl_lnum,"unknown @-line: %s",*lp);
	  break;
	default:
	  psu_form = parse_ngram_line(nlp, lp, nlp->nngrams, nlp->owner, components, NULL);
	  break;
	}
    }
  ++nlp->nngrams;
  ++nl_lnum;
  return psu_form;
}

void
nl_set_location(const char *fn,int ln)
{
  nl_file = fn;
  nl_lnum = ln;
}

struct NL_context *
nl_load_file(struct sigset *sp,
	     struct NL_context*nlcp, const char *fname, 
	     enum nlcp_actions act, const char *lang)
{
  char **ngram_lines;
  unsigned char *fmem;
  size_t nlines, i, nngrams;
  struct NL *nlp = nl_setup(NULL, act, lang);
  nlcp = nlp->owner;
  nlcp->nlp = nlp;
  nlcp->owner = sig_new_context_free_sigset();
  nlp->file = fname;
  nl_set_location(fname,1);
  ngram_lines = (char**)loadfile_lines3((unsigned char *)fname,&nlines,&fmem);
  for (nngrams = i = 0; i < nlines; ++i)
    nl_process_one_line(nlp, ngram_lines[i], NULL);
  free(fmem);
  free(ngram_lines);
  return nlcp;
}

void
nl_init(void)
{
  if (!nl_context_mem)
    {
      nl_context_mem = mb_init(sizeof(struct NL_context),1);
      nl_mem = mb_init(sizeof(struct NL),1);
      nle_mem = mb_init(sizeof(struct NLE),16);
      ngram_lines_mem = mb_init(sizeof(char *),16);
      cf_mem = mb_init(sizeof(struct CF),32);
      pred_mem = mb_init(sizeof(struct PRED),32);
    }
}

void
nl_term(void)
{
  if (nl_context_mem)
    {
      mb_free(nl_context_mem);
      mb_free(nl_mem);
      mb_free(nle_mem);
      mb_free(ngram_lines_mem);
      mb_free(cf_mem);
      mb_free(pred_mem);
    }
}

struct NL *
nl_setup(struct NL_context**xnlcp, enum nlcp_actions act, const char *lang)
{
  struct NL *nlp = mb_new(nl_mem);
  struct NL_context *nlcp;
  if (!xnlcp || !*xnlcp)
    {
      nlcp = nl_new_context(NULL, act, lang);
      nlp->owner = nlcp;
    }
  else
    {
      struct NL*tmp;
      nlcp = *xnlcp;
      for (tmp = nlcp->nlp; tmp->next; tmp = tmp->next)
	;
      tmp->next = nlp;
      nlp->owner = nlcp;
    }
  if (xnlcp)
    *xnlcp = nlcp;
  return nlp;
}

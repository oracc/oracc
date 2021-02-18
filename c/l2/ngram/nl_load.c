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

static unsigned char *
add_str(char *dest, const char *src, struct npool *pool)
{
  if (!src || !strlen(src))
    return (unsigned char *)dest;
  
  if (dest)
    {
      int len = strlen(dest) + strlen(src) + 2;
      char *n = malloc(len);
      sprintf(n,"%s %s",dest, src);
      dest = (char*)npool_copy((unsigned char *)n, pool);
      free(n);
    }
  else
    {
      dest = (char*)npool_copy((unsigned char *)src, pool);
    }
  return (unsigned char *)dest;
}

static unsigned char *
fin_str(const char *str, struct npool *pool)
{
  return (unsigned char *)str;
#if 0  
  if (str)
    {
      unsigned char *tmp = npool_copy((unsigned char *)str,pool);
      free((char*)str);
      return tmp;
    }
  else
    return NULL;
#endif
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

      /* Not necessary any more but need to add f->pos to psu form string */
      /* nlep->cfs[i]->f2->form = f->form; */
      
      nlep->psu_form->form = add_str((char*)nlep->psu_form->form, (char*)(f->form ? f->form : f->pos),
				     nlep->owner->owner->pool);
      nlep->psu_form->norm = add_str((char*)nlep->psu_form->norm, (char*)f->norm,
				     nlep->owner->owner->pool);
      nlep->psu_form->base = add_str((char*)nlep->psu_form->base, (char*)f->base,
				     nlep->owner->owner->pool);
      nlep->psu_form->cont = add_str((char*)nlep->psu_form->cont, (char*)f->cont,
				     nlep->owner->owner->pool);
      nlep->psu_form->morph = add_str((char*)nlep->psu_form->morph, (char*)f->morph,
				      nlep->owner->owner->pool);
      nlep->psu_form->morph2 = add_str((char*)nlep->psu_form->morph2, (char*)f->morph2,
				       nlep->owner->owner->pool);
      if (!nlep->psu_form->project)
	nlep->psu_form->project = f->project;
      components->rover->data = f;
    }
  nlep->psu_form->psu_ngram = ngm;
  nlep->psu_form->parts = (struct f2 **)list2array(components);
  list_free(components,NULL);
  nlep->psu_form->form = fin_str((char*)nlep->psu_form->form,nlep->owner->owner->pool);
  nlep->psu_form->norm = fin_str((char*)nlep->psu_form->norm,nlep->owner->owner->pool);
  nlep->psu_form->base = fin_str((char*)nlep->psu_form->base,nlep->owner->owner->pool);
  nlep->psu_form->cont = fin_str((char*)nlep->psu_form->cont,nlep->owner->owner->pool);
  nlep->psu_form->morph = fin_str((char*)nlep->psu_form->morph,nlep->owner->owner->pool);
  nlep->psu_form->morph2 = fin_str((char*)nlep->psu_form->morph2,nlep->owner->owner->pool);
  return nlep->psu_form;
}

void
nlep_free(struct NLE *p)
{
  if (p->psu_form && p->psu_form->parts)
    free(p->psu_form->parts);
  if (p->cfs)
    free(p->cfs);
  if (p->tts)
    free(p->tts);
}

void
nleps_free(struct NLE_set *p)
{
  int i;
  if (p->pp)
    {
      for (i = 0; i < p->pp_used; ++i)
	nlep_free(p->pp[i]);
      free(p->pp);
    }
  free(p);
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
  /* set key here to ->cf or ->pos */
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

#if 0
  if (!strncmp(line, "n ", 2))
    {
      fprintf(stderr, "found 'n '\n");
    }
#endif
  
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
      while (isspace(end_cts[-1]))
	--end_cts;
      *end_cts = '\0';		     
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
  if (!nlcp)
    return;

  if (nlcp->posNgrams)
    {
      hash_free(nlcp->posNgrams, (void (*)(void *))nleps_free);
      nlcp->posNgrams = NULL;
    }
  if (nlcp->psuNgrams)
    {
      hash_free(nlcp->psuNgrams, (void (*)(void *))nleps_free);
      nlcp->psuNgrams = NULL;
    }
  if (nlcp->pool)
    {
      npool_term(nlcp->pool);
      nlcp->pool = NULL;
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
      if (verbose > 2)
	ngdebug("loading:%d: %s",nl_lnum,lp);
      switch (*lp)
	{
	case ' ':
	case '\t':
	case '#':
	  break;
#if 0
	case '@':
	  vwarning2(nl_file,nl_lnum,"unknown @-line: %s",lp);
	  break;
#endif
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
  const char *n = NULL;
  unsigned char *fmem;
  size_t nlines, i, init_line = 0;
  struct NL *nlp = NULL;

  if (!fname)
    return NULL;
  
  nlp = nl_setup(NULL, act, lang);
  nlcp = nlp->owner;
  nlcp->nlp = nlp;
  nlcp->owner = sig_new_context_free_sigset();
  nlp->file = fname;
  n = strrchr(fname, '/');
  if (n)
    nlp->name = xstrdup(n+1);
  else
    nlp->name = xstrdup(fname);
  ngdebug("input file=%s", fname);
  nl_set_location(xstrdup(fname),1);
  ngram_lines = (char**)loadfile_lines3((unsigned char *)fname,&nlines,&fmem);
  if (ngram_lines[0] && !strncmp(ngram_lines[0], "@role",strlen("@role")))
    {
      char *tmp = ngram_lines[0] + strlen("@role");
      while (*tmp && isspace(*tmp))
	++tmp;
      nlcp->role = xstrdup(tmp);
      ++init_line;
    }
  for (i = init_line; i < nlines; ++i)
    {
      if (ngram_lines[i] && *ngram_lines[i])
	nl_process_one_line(nlp, ngram_lines[i], NULL);
    }
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

#include <stdio.h>
#include <string.h>
#include <psdtypes.h>
#include <fname.h>
#include <npool.h>
#include <memblock.h>
#include <loadfile.h>
#include <ctype128.h>
#include "warning.h"
#include "globals.h"
#include "xpd2.h"
#include "lang.h"
#include "f2.h"
#include "ilem.h"
#include "ilem_form.h"
#include "psus.h"
#include "../bigrams/bigrams.h"
#include "sigs.h"
#include "words.h"
#include "sas.h"
#include "xli.h"

extern int lem_extended;
extern int lem_autolem;

#ifndef strdup
extern char *strdup(const char*);
#endif

static struct sig_context *global_scp;

static void sig_lang_free(List *lp);

int lang_sigs_cmp(struct sigset *a,char *b)
{
  return strcmp((char*)(a->file ? a->file : a->project),b);
}

List *
sig_autoload_sets(struct sig_context *scp,
		  const char *project,
		  const char *lang)
{
  return sig_context_register(scp,project,lang,0);
}

List *
sig_context_register(struct sig_context *scp, 
		     const char *lproject, 
		     const char *lang,
		     int auto_ok)
{
  static Uchar fname[_MAX_PATH];
  List *sigs_for_lang = NULL;
  static int bad = 1;
  const char *sigslang = NULL, *lang_to_be_lemmed = NULL, 
    *proj_to_be_lemmed = (char*)scp->xpd->project;
  const char *sigsproj;
  char *colon = NULL, *projlang = NULL;
  int no_sigfile = 0;

  if (!lang)
    {
      char *pproj = (char*)npool_copy((const Uchar *)lproject,scp->pool);
      if ((colon = strchr(pproj, ':')))
	{
	  sigsproj = pproj;
	  pproj = colon;
	  *pproj++ = '\0';
	  lang_to_be_lemmed = sigslang = pproj;
	}
    }
  else if ((colon = strchr(lproject, ':')))
    {
      /* allow checking ancestor glossaries:
	 <option name="%akk-x-ltebab" value=". .:akk"/>
       */
      char *pproj = (char*)npool_copy((const Uchar *)lproject,scp->pool);
      sigsproj = pproj;
      pproj = pproj + (colon - lproject);
      *pproj++ = '\0';
      lang_to_be_lemmed = lang;
      sigslang = pproj; /* leave lang set to the language we're going to be checking */
    }
  else
    {
      lang_to_be_lemmed = sigslang = lang;
      sigsproj = lproject;
    }

  if ('.' == sigsproj[0] && !sigsproj[1])
    sigsproj = proj_to_be_lemmed;

  if (!scp || !sigsproj || !sigslang || !lang_to_be_lemmed)
    {
      fprintf(stderr,"sig_context: register received NULL argument\n");
      goto ret;
    }

  if (!strncmp(lang_to_be_lemmed,"sux",3) && !scp->aliases_file)
    {
      const char *opt = xpd_option(scp->xpd,"sum-alias");
      if (opt && !strcmp(opt, "yes"))
	{
	  char buf[1024];
	  sprintf(buf,"%s/pub/epsd2/aliases.asa",oracc_home());
	  scp->aliases_file = (char*)npool_copy((unsigned char *)buf,scp->pool);
	  if (!xaccess(buf,R_OK,0))
	    {
	      scp->aliases = sas_asa_load(buf);
	      if (scp->aliases)
		{
		  if (verbose)
		    fprintf(stderr,"ox: using aliases %s\n",scp->aliases_file);
		}
	      else
		fprintf(stderr,"ox: failed to load readable aliases %s\n",scp->aliases_file);
	    }
	}
    }
  
  if (fuzzy_aliasing || xpd_option(scp->xpd, "fuzzy-alias"))
    scp->fuzzy = fuzzy_aliasing = 1; /* FIXME: this should be language-specific */

  strcpy((char*)fname,oracc_home());
  if (fname[strlen((char*)fname)-1] != '/')
    strcat((char*)fname,"/");
  sprintf((char*)fname+strlen((char*)fname),
	  "pub/%s/lemm-%s.sig",sigsproj,sigslang);

  projlang = malloc(strlen(scp->xcp->project)+strlen(lang)+2);
  sprintf(projlang,"%s%%%s",proj_to_be_lemmed,lang_to_be_lemmed);
  if (!(sigs_for_lang = hash_find(scp->langs, (Uchar*)projlang)))
    {
      sigs_for_lang = list_create(LIST_SINGLE);
      hash_add(scp->langs, npool_copy((Uchar*)projlang,scp->pool), sigs_for_lang);
      /* Check that the file is accessible and warn if it is not unless auto
	 is non-zero */
      if (!access((char*)fname, R_OK))
	{
	  if (verbose)
	    fprintf(stderr,
		    "sig_context: registering sigs for %s:%s from `%s'\n", 
		    proj_to_be_lemmed, lang_to_be_lemmed, fname);
	}
      else
	{
	  if (!auto_ok)
	    {
	      if (strcmp(lang_to_be_lemmed, "qcu"))
		fprintf(stderr,
			"sig_context: can't register sigs for %s:%s: can't read `%s'\n", 
			proj_to_be_lemmed, lang_to_be_lemmed, fname);
	    }
	    no_sigfile = 1;
	}
    }
  free(projlang);

  if (!list_find(sigs_for_lang, fname, (list_find_func*)lang_sigs_cmp))
    {
      struct sigset *sp = mb_new(scp->mb_sigsets);
      sp->owner = scp;
      sp->project = npool_copy((Uchar*)sigsproj,scp->pool);
      sp->lang = npool_copy((Uchar *)sigslang,scp->pool);
      sp->core = langcore_of((const char *)sp->lang);
      if (no_sigfile)
	sp->file = NULL;
      else
	sp->file = npool_copy(fname,scp->pool);
      list_add(sigs_for_lang, sp);
      /* Note that we do not actually load the sigs here--that is done lazily
	 when we try to do some lemmer operation in the language */
    }
  else
    {
      if (!hash_find(scp->known_bad,fname))
	{
	  if (verbose)
	    fprintf(stderr,"sig_context: ignoring duplicate data source `%s'\n", fname);
	  hash_add(scp->known_bad,npool_copy(fname,scp->pool),&bad);
	}
    }

 ret:
  return sigs_for_lang;
}

struct sigset *
sig_new_context_free_sigset(void)
{
  struct sigset *sp = mb_new(global_scp->mb_sigsets);
  sp->owner = global_scp;
  return sp;
}

struct sig_context *
sig_context_init()
{
  if (!global_scp)
    {
      global_scp = calloc(1, sizeof(struct sig_context));
      global_scp->pool = npool_init();
      global_scp->known_bad = hash_create(1);
      global_scp->langs = hash_create(1);
      global_scp->mb_f2s = mb_init(sizeof(struct f2),256);
      global_scp->mb_f2ps = mb_init(sizeof(struct f2),32);
      global_scp->mb_ilem_forms = mb_init(sizeof(struct ilem_form),64);
      global_scp->mb_sigs = mb_init(sizeof(struct sig),256);
      global_scp->mb_sigsets = mb_init(sizeof(struct sigset),1);
      w2_init();
      sigs_cof_init();
    }
  return global_scp;
}

void
sig_context_term(void)
{
  if (global_scp)
    {
      if (global_scp->aliases)
	sas_asa_unload(global_scp->aliases);
      ilem_select_clear();
      hash_free(global_scp->known_bad, NULL);
      hash_free(global_scp->langs, (hash_free_func*)sig_lang_free);
      npool_term(global_scp->pool);
      mb_free(global_scp->mb_f2s);
      mb_free(global_scp->mb_f2ps);
      mb_free(global_scp->mb_ilem_forms);
      mb_free(global_scp->mb_sigs);
      mb_free(global_scp->mb_sigsets);
      w2_term();
      sigs_cof_term();
      free(global_scp);
    }
}

static Uchar *
find_form(const Uchar *ln)
{
  static Uchar buf[1024];
  Uchar *colon = NULL, *equals_or_tab = NULL;
  size_t len = 0;
  *buf = '\0';
  if ((colon = (Uchar*)strchr((char*)ln,':')))
    {
      equals_or_tab = (Uchar *)strchr((char*)++colon,'=');
      if (!equals_or_tab)
	equals_or_tab = (Uchar*)strchr((char*)colon,'\t');
      len = equals_or_tab-colon;
      strncpy((char*)buf,(char*)colon,len);
      buf[len] = '\0';
    }
  return buf;
}

static Uchar *
find_cf_or_norm(const Uchar *ln)
{
  static Uchar buf[1024];
  const Uchar *start;

  if (strstr((const char*)ln,"%akk"))
    {
      const Uchar *dollar = sigs_field_begin(ln,(const Uchar *)"$");
      if (dollar)
	{
	  const Uchar *endp = sigs_field_end(dollar+1);
	  Uchar *bufp = buf;
	  const Uchar *dollp = dollar+1;
	  while (dollp < endp && '\t' != *dollp)
	    *bufp++ = *dollp++;
	  *bufp = '\0';
	  return buf;
	}
      /* else fall through to do CF finding */
    }

  if (!(start = (unsigned char *)strchr((char*)ln,'=')))
    start = ln;
  else
    ++start;

  {
    Uchar *bufp = buf;
    const Uchar *lnp = start;
    while (*lnp && '[' != *lnp)
      *bufp++ = *lnp++;
    *bufp = '\0';
    return buf;
  }
}

#if 0
static int
validate_sig_list(struct sig *s)
{
  while (s)
    if (s == s->next)
      {
	return 1;
      }
    else
      s = s->next;
  return 0;
}

static void
vsigs1(struct sig *s)
{
  struct sig *orig = s;
  while (s)
    if (s == s->next)
      {
	fprintf(stderr,"%p: circular sig (1)\n",(void*)orig);
	break;
      }
    else
      s = s->next;
}

static void
vsigs2(struct sig *s)
{
  struct sig *orig = s;
  while (s)
    if (s == s->next)
      {
	fprintf(stderr,"%p: circular sig (2)\n",(void*)orig);
	break;
      }
    else
      s = s->next;
}

static void
vsigs3(struct sig *s)
{
  struct sig *orig = s;
  while (s)
    if (s == s->next)
      {
	fprintf(stderr,"%p: circular sig (3)\n",(void*)orig);
	break;
      }
    else
      s = s->next;
}
#endif

static int
sig_load_sigs(struct sig_context*scp, struct sigset *sp)
{
  int i;
  if (xaccess((const char *)sp->file, R_OK, 0))
    {
      fprintf(stderr,"unable to load signatures from %s\n", sp->file);
      return 1;
    }
  sp->lines = loadfile_lines3(sp->file, NULL,&sp->fmem);
  for (i = 0; sp->lines[i]; ++i)
    sigs_load_one_sig(scp, sp, sp->lines[i], i, NULL);
  return 0;
}

void
sigs_load_one_sig(struct sig_context*scp, struct sigset *sp, const unsigned char *sig, int lnum, struct ilem_form *ifp)
{
  Uchar *form = NULL, *cf_or_norm = NULL;
  struct sig *s = NULL, *s2 = NULL, *try = NULL;
  char *tab = NULL;
      
  form = find_form(sig);

  if (!sp || !form || !*form)
    return;

  s = mb_new(scp->mb_sigs);
  /*sprintf(pbuf,"%p",(void*)s);*/
  cf_or_norm = find_cf_or_norm(sig);
  s->lnum = lnum;
  s->set = sp;
  s->sig = sig;
  s->ifp = ifp;
  if ((tab = strchr((char*)s->sig,'\t')))
    {
      *tab++ = '\0';
      s->freq = atoi(tab);
      if ((tab = strchr(tab, '\t')))
	{
	  *tab++ = '\0';
	  /* s->pct = atoi(tab); */
	  s->freq = atoi(tab); /* fake pct into freq */
	}
      /* the end of the line contains the xis ref but that is benign */
    }

  if ('{' == *s->sig)
    {
      Uchar *lp = npool_copy(s->sig, scp->pool), *endp;
      ++lp;
      endp = (Uchar *)strstr((char*)lp,"}::");
      if (endp)
	{
	  List *components = list_create(LIST_SINGLE);
	  Uchar *psu = lp;
	  struct f2 *psu_form = NULL;
	  *endp = '\0';
	  lp = endp + 3;
	  while (1)
	    {
	      list_add(components, lp);
	      lp = (unsigned char *)strstr((char*)lp,"++");
	      if (lp)
		{
		  *lp = '\0';
		  lp += 2;
		}
	      else
		break;
	    }
	  psu_form = psu2_register((char*)sp->file,lnum,sp,(char*)sp->lang,psu,components);
	  if (psu_form)
	    {
	      form = (Uchar*)psu_form->form;
	      cf_or_norm = (Uchar*)psu_form->cf;
	      s->f2p = psu_form;
	    }
	  else
	    return;
	      
	}
      else
	{
	  vwarning2((char*)sp->file,lnum,"malformed PSU in sigfile");
	  return;
	}
    }

  if (sp && sp->forms && (try = hash_find(sp->forms, form)))
    {
      if (try->next)
	{
	  s->next = try->next;
	  try->next = s;
	}
      else
	{
	  try->next = s;
	}
      ++try->count;
    }
  else
    {
      s->count = 1;
      hash_add(sp->forms, npool_copy(form, scp->pool), s);
      if (verbose > 1)
	fprintf(stderr, "sigs_load_sigs: registering form %s\n", form);
    }

  /* FIXME: should rewrite this so that the parsed form in s and s2
     can be shared between the two */
  s2 = mb_new(scp->mb_sigs);
  *s2 = *s;
  s2->next = NULL;
  if (sp && sp->norms && (try = hash_find(sp->norms, cf_or_norm)))
    {
      if (try->next)
	{
	  s2->next = try->next;
	  try->next = s2;
	}
      else
	{
	  try->next = s2;
	}
      ++try->count;
    }
  else
    {
      s2->count = 1;
      hash_add(sp->norms, npool_copy(cf_or_norm, scp->pool), s2);
      if (verbose > 1)
	fprintf(stderr, "sigs_load_sigs: registering cf_or_norm %s\n", cf_or_norm);
    }
}

/* FIXME: this is supposed to return 0 on success and 1 on error */
int
sig_load_set(struct sig_context*scp, struct sigset *sp)
{
  sp->loaded = 1;
  sp->forms = hash_create(1000);
  sp->norms = hash_create(1000);

  /* there is no sp->file when the sigset is being autocreated from a '+' lem
     in a context where there is no glossary for a language, e.g., when the
     project is brand new or when the language is occurring in a  project for
     the first time */
  if (sp->file)
    {
      sig_load_sigs(scp,sp);
      if (lem_autolem)
	bigrams_init(sp);
      if (verbose)
	fprintf(stderr,"sig_context: loaded sigs from %s\n", sp->file);
    }
  return 0;
}

static void
sig_free(struct sig *s)
{
}

static void
sigset_free(struct sigset *sp)
{
  if (sp->lines)
    {
      free(sp->lines);
      free(sp->fmem);
    }
  if (sp->forms)
    hash_free(sp->forms,(hash_free_func*)sig_free);
  if (sp->norms)
    hash_free(sp->norms,(hash_free_func*)sig_free);
  if (sp->psus)
    nl_free_context(sp->psus);
  if (sp->bigrams_hash)
    free(sp->bigrams);
  if (sp->bigrams)
    nl_free_context(sp->bigrams);
}

static void
sig_lang_free(List *lp)
{
  list_free(lp, (list_free_func*)sigset_free);
}

void
sig_context_langs(struct sig_context *scp, const char *langs)
{
  char **langp = NULL, *langtmp;
  List *langlist = NULL;
  int i;

  if (!langs || !*langs)
    return;

  /*
    foreach LANG
      foreach PROJ
        register SIGS PROJ LANG
   */
  langtmp = malloc(strlen(langs)+1);
  strcpy(langtmp,langs);
  langlist = list_from_str(langtmp,NULL,LIST_SINGLE);
#if 0
  if (qpn_mode)
    list_add(langlist, strdup("qpn"));
#endif
  if ((langp = (char**)list2array(langlist)))
    {
      for (i = 0; langp[i]; ++i)
	{
	  const char *proj = NULL;
	  if (system_project && !strcmp(system_project, "."))
	    proj = system_project;
	  else if (scp->xpd)
	    proj = xpd_lang(scp->xpd,langp[i]);
	  else
	    {
	      fprintf(stderr, "sigs_context: can't continue with no project set\n");
	      exit(1);
	    }
	  if (proj)
	    {
	      char **projp = NULL, *projtmp = NULL;
	      List *projlist = NULL;
	      int j;
	      if (verbose)
		fprintf(stderr,"sigs_context: lang=%s; projects=%s\n",langp[i],proj);
	      projtmp = malloc(strlen(proj)+1);
	      strcpy(projtmp,proj);
	      projlist = list_from_str(projtmp,NULL,LIST_SINGLE);
	      projp = (char**)list2array(projlist);
	      for (j = 0; projp[j]; ++j)
		sig_context_register(scp, projp[j], langp[i], 0);
	      free(projp);
	      free(projtmp);
	      list_free(projlist,list_xfree);
	    }
	  else
	    {
	      if (verbose)
		fprintf(stderr,"sigs_context: lang=%s not in config; using only %s\n", langp[i],scp->xpd->project);
	      sig_context_register(scp, scp->xpd->project, langp[i], 0);
	    }

	  /* Initialize extended lemmatization if we are going to need
	     it */
	  if (lem_extended)
	    {
	      struct langcore *lcp = langcore_of(langp[i]);
	      if (lcp)
		{
		  if (!xlem_tried[lcp->code])
		    {
		      struct xli *xlip;
		      xlem_tried[lcp->code] = 1;
		      if ((xlip = xli_system(langp[i])))
			{
			  xlem[lcp->code] = xlip;
			  if (xlip->init)
			    xlip->init(xlip);
			  else
			    xli_init_external(xlip);
			}
		    }
		}
	    }

	  free(langp[i]);
	}
      free(langp);
    }
  free(langtmp);
  if (langlist)
    list_free(langlist,NULL); /* langp[i] already freed */
}

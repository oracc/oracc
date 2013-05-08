#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "npool.h"
#include "hash.h"
#include "runexpat.h"
#include "memblock.h"
#include "xcl.h"
#include "ngram.h"
#include "props.h"
#include "lang.h"

#if 0
struct props_user
{
  struct props_context *props;
  struct props_lang_data *curr_lang;
  struct props_term *curr_term;
  enum langcode l;
};
#else
struct props_user
{
  struct props_context *context;
  List *terms;
  List *props;
  struct props_lang_data *curr_lang;
  struct props_term *curr_term;
  struct props_prop *curr_prop;
  struct npool *pool;
};
#endif

extern const char *project;
extern int verbose;
static Hash_table *props_loaded = NULL;

static void props_warning(const char *mess, const char *cause);
static struct props_context *props_load(const char *fname, const char *name, struct xcl_context *xc);
static struct NL_context *props_load_lang(struct props_lang_data *props_lang);

static struct props_context *
known_props(const char *name)
{
  struct props_context *ret = NULL;
  if (props_loaded)
    ret = hash_find(props_loaded, (unsigned char *)name);
  if (!ret)
    props_loaded = hash_create(1);
  return ret;
}

void
props_free(void)
{
  hash_free(props_loaded, (void(*)(void*))props_term);
}

struct props_context *
props_init(struct xcl_context *xc, const char *name)
{
  static char buf[48];
  struct props_context *props = NULL;
  int i;

  if ((props = known_props(name)))
    return props;

  sprintf(buf,"/usr/local/oracc/lib/data/%s.xml",name);
  if (!xaccess(buf,R_OK,0))
    props = props_load(buf, name, xc);

  if (!props)
    {
      fprintf(stderr,"props: no data file found for `%s'\n", name);
      return NULL;
    }

  for (i = 0; i < c_count; ++i)
    {
      if (props->langs[i])
	{
	  props->nlcps[i] = props_load_lang(props->langs[i]);
	  props->nlcps[i]->action = nlcp_action_props;
	  props->nlcps[i]->func = props_ngram_match;
	  if (verbose)
	    {
	      fprintf(stderr,"loaded %s props for lang %s from file %s\n", 
		      name, props->langs[i]->lang->name,buf);
	      if (verbose > 1)
		{
		  char fname[128];
		  FILE *fp;
		  sprintf(fname,"props-%s-%s.xml",props->name,props->langs[i]->lang->name);
		  fp = fopen(fname,"w");
		  nl_dump(fp,props->nlcps[i]);
		  fclose(fp);
		}
	    }
	}
    }

  props->heads = hash_create(1);
  props->todo = hash_create(1);

  ngramify_init();

  return props;
}

void
props_term(struct props_context *pcp)
{
  ngramify_term();
}

static struct NL_context *
props_load_lang(struct props_lang_data *props_lang)
{
  struct NL *nlp = nl_setup(NULL, nlcp_action_props, props_lang->lang->name);
  struct NL_context *nlcp = nlp->owner;
  size_t i;
  nlcp->nlp = nlp;
  nlcp->owner = sig_new_context_free_sigset();
  nlp->file = nl_file = "psa.xml";
  /* nlp->parsed_ngrams = calloc(props_lang->terms_used, sizeof(struct NLE)); */
  for (i = 0; i < props_lang->terms_used; ++i)
    {
      parse_ngram_line(nlp, (char *)props_lang->term[i]->ngram, i, 
		       nlcp, NULL, props_lang->term[i]);
    }
  nlp->nngrams = props_lang->terms_used;
  return nlcp;
}

static const char *props_ns = "http://oracc.org/ns/psa/1.0";
#define props_offset (strlen(props_ns))

static int
ref_nth(const char *ref)
{
  int nth = 0;

  if (*ref == '-')
    nth = atoi(ref);
  else
    {
      if (*ref == '#')
	++ref;
      if (*ref == 'p')
	{
	  if (!strcmp(ref,"prevs"))
	    nth = -2;
	  else
	    nth = -1;
	}
      else if (*ref == 'h')
	nth = -100000;
      else if (*ref != 't')
	nth = 1;
    }
  return nth;
}

static struct props_prop *
new_prop(struct props_user *udata, const char **atts)
{
  struct props_prop*curr_prop = NULL;

  if (!udata->curr_term)
    return NULL;

  udata->curr_prop = curr_prop = mb_new(udata->context->mb_props_props);
  curr_prop->name = npool_copy((unsigned char *)findAttr(atts,"name"), udata->pool);
  curr_prop->group = npool_copy((unsigned char *)findAttr(atts,"group"), udata->pool);
  if (!curr_prop->group || !*curr_prop->group)
    {
      struct propdef *p = props_auto_name(curr_prop->name);
      if (p)
	curr_prop->group = p->group;
      else
	fprintf(stderr,"props: no group for property `%s'\n", curr_prop->name);
    }
  curr_prop->value = npool_copy((unsigned char *)findAttr(atts,"value"), udata->pool);
  curr_prop->maxlem = atoi(findAttr(atts,"maxlem"));
  if (curr_prop->maxlem < 0)
    curr_prop->maxlem *= -1;
  else if (curr_prop->maxlem == 0)
    curr_prop->maxlem = 1;
  if (!strcmp(findAttr(atts,"headshift"),"1"))
    curr_prop->headshift = 1;
  else
    curr_prop->headshift = 0;
  if (*curr_prop->value == '#')
    {
      int nth = ref_nth((const char *)curr_prop->value);
      char buf[32];
      sprintf(buf,"#%d",nth);
      curr_prop->value = npool_copy((unsigned char *)buf, udata->pool);
    }
  curr_prop->where = ref_nth((const char *)findAttr(atts,"on"));
  return curr_prop;
}

static struct props_term *
new_term(struct props_user *udata)
{
#if 0
  if (udata->curr_lang->terms_used == udata->curr_lang->terms_alloced)
    {
      udata->curr_lang->terms_alloced += 16;
      udata->curr_lang->term 
	= realloc(udata->curr_lang->term,
		  udata->curr_lang->terms_alloced*sizeof(struct props_term));
    }
  udata->curr_term = &udata->curr_lang->term[udata->curr_lang->terms_used++];
  memset(udata->curr_term,'\0',sizeof(struct props_term));
#else
  udata->curr_term = mb_new(udata->context->mb_props_terms);
  return udata->curr_term;
#endif
}

static void
props_sH(void *userData, const char *name, const char **atts)
{
  if (!strncmp(name,props_ns,props_offset))
    {
      const char *lname = name + props_offset;
      struct props_user *udata = userData;
      if (*lname == ':')
	{
	  ++lname;
	  if (!strcmp(lname,"lang"))
	    {
	      const char *lang = findAttr(atts, "lang");
	      struct langcore *lp = lang_core(lang,strlen(lang));
	      if (lp)
		{
#if 0
		  ((struct props_user*)->curr_lang 
		    = ((struct props_user*)userData)->props->langs[lp->code]
		    = calloc(1,sizeof(struct props_lang_data));
		  ((struct props_user*)userData)->props->langs[lp->code]->lang = lp;
#else
		   udata->curr_lang 
		   = udata->context->langs[lp->code]
		   = mb_new(udata->context->mb_props_lang_datas);
		   udata->context->langs[lp->code]->lang = lp;
#endif
		}
	      else
		{
		  props_warning("unknown lang", lname);
		  udata->curr_lang = NULL;		  
		}
	    }
	  else if (!strcmp(lname,"term") && udata->curr_lang)
	    {
	      if (!udata->terms)
		udata->terms = list_create(LIST_SINGLE);
	      list_add(udata->terms, new_term(udata));
	    }
	  else if (!strcmp(lname,"prop"))
	    {
	      if (!udata->props)
		udata->props = list_create(LIST_SINGLE);
	      list_add(udata->props, new_prop(userData,atts));
	    }
	  else if (!strcmp(lname,"ngram"))
	    {
	      charData_discard();
	    }
	  else if (!strcmp(lname,"config"))
	    ;
	  else
	    props_warning("unknown props tag", lname);
	}
    }
}

static void
props_eH(void *userData, const char *name)
{
  if (!strncmp(name,props_ns,props_offset))
    {
      const char *lname = name + props_offset;
      struct props_user *udata = userData;
      if (*lname == ':')
	{
	  ++lname;
	  if (!strcmp(lname,"ngram") && udata->curr_term)
	    udata->curr_term->ngram = npool_copy((unsigned char *)charData_retrieve(),
						 udata->pool);
	  else if (!strcmp(lname,"term") && udata->curr_term)
	    {
	      udata->curr_term->props = (struct props_prop**)list2array(udata->props);
	      udata->curr_term->props_used = list_len(udata->props);
	      list_free(udata->props, NULL);
	      udata->props = NULL;
	      udata->curr_prop = NULL;
	    }
	  else if (!strcmp(lname,"lang"))
	    {
	      udata->curr_lang->term = (struct props_term**)list2array(udata->terms);
	      udata->curr_lang->terms_used = list_len(udata->terms);
	      list_free(udata->terms, NULL);
	      udata->terms = NULL;
	      udata->curr_term = NULL;
	      udata->curr_lang = NULL;
	    }
	}
    }
}

static struct props_context *
props_load(const char *fname, const char *name, struct xcl_context *xc)
{
  const char *fns[2];
  struct props_context *props = calloc(1, sizeof(struct props_context));
  struct props_user *props_user = calloc(1, sizeof(struct props_user));
  props->name = name;
  props->nlcps = calloc(c_count, sizeof(struct NL_context*));
  props->langs = calloc(c_count, sizeof(struct props_lang_data*));
  props->mb_props_terms = mb_init(sizeof(struct props_term),32);
  props->mb_props_props = mb_init(sizeof(struct props_prop),32);
  props->mb_props_lang_datas = mb_init(sizeof(struct props_lang_data),1);
  props_user->context = props;
#if 0
  props_user->l = c_none;
#endif
  props_user->pool = xc->pool;
  fns[0] = fname;
  fns[1] = NULL;
  runexpatNSuD(i_list, fns, props_sH, props_eH, ":", props_user);
  free(props_user);
  return props;
}

static void
props_warning(const char *mess, const char *cause)
{
  fprintf(stderr, "props: `%s': %s\n", cause, mess);
}

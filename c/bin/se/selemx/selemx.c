#include <stdarg.h>
#include <psd_base.h>
#include <ctype128.h>
#include <dbi.h>
#include <index.h>
#include <alias.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>
#include <locale.h>
#include "memblock.h"
#include "atflocale.h"
#include "charsets.h"

#include "fields.h"
#include "property.h"
#include "se.h"
#include "../types.h"
#include "selib.h"
#include "vid.h"
#include "f2.h"
#include "sigs.h"

int l2 = 1;
/* This is unpleasant; declared only because we have to invoke sig_context_init() which pulls in the sigs library */
int lem_autolem = 0;
int fuzzy_aliasing = 0;
int cbd_lem_sigs = 0;
int bootstrap_mode = 0;

static struct est *estp;

const char *gdl_ns_uri = "http://oracc.org/ns/gdl/1.0";
const char *norm_ns_uri = "http://oracc.org/ns/norm/1.0";
const char *xcl_ns_uri = "http://oracc.org/ns/xcl/1.0";
const char *xtf_ns_uri = "http://oracc.org/ns/xtf/1.0";

static Hash_table *lemindex, *lemparses;
static struct npool *lempool;

static struct vid_data *vidp;

static struct mb *f2_mem;

#ifndef strdup
extern char *strdup(const char *);
#endif

extern void grapheme_decr_start_column(void);
extern void grapheme_end_column_logo(void);

#undef xmalloc
#undef xrealloc
#undef xcalloc
#undef xfree
#define xmalloc malloc
#define xrealloc realloc
#define xcalloc calloc
#define xfree free

#include "addgraph.c"

Dbi_index *dip;

static int aliases_only = 0;
static int in_qualified = 0;
static int role_logo = 0;
static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex;
int one_big_stdin = 0;
static char loc_project_buf[_MAX_PATH];
int verbose = 0;

struct sig_context *lem_scp = NULL;

const char *curr_project = "cdli", *curr_index = "lem", *proxies_arg = "";
const char *project = NULL; /* for now we need this for vid.c */
Four_bytes curr_line;
int debug_flag = 0, index_flag = 1;
const char *debug_label;
FILE *debug_f;
FILE *f_mangletab = NULL;

static int cache_elements = 16;

struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

int curr_sentence_id, curr_clause_id, curr_phrase_id, curr_lemma_id;

int indexing = 0;
int quiet = 0;

const char **proxies = NULL;

static FILE *keysf;
/* static void process_cdata(Uchar*); */
static void fn_expand(void *p);

enum pending_boundary_types { pb_none , pb_space , pb_hyphen };
enum pending_boundary_types pending_boundary = pb_none;

extern void grapheme_decr_start_column(void);
extern void signmap_init(void);
extern void signmap_term(Dbi_index *);
static void set_proxies(const char *pxpath);

static void startElement_gdl(void *userData, const char *name, const char **atts);
static void endElement_gdl(void *userData, const char *name);
static void startElement_norm(void *userData, const char *name, const char **atts);
static void endElement_norm(void *userData, const char *name);
static void startElement_xtf(void *userData, const char *name, const char **atts);
static void endElement_xtf(void *userData, const char *name);

static int
is_ns(const char *name, const char *uri)
{
  const char *ns_end = strchr(name, '|');
  if (ns_end && !strncmp(name, uri, ns_end - name))
    return strlen(uri)+1;
  else
    return 0;
}

static void
do_boundary()
{
  switch (pending_boundary)
    {
    case pb_space:
      grapheme_boundary(' ');
      break;
    case pb_hyphen:
      grapheme_boundary('-');
      break;
    case pb_none:
      break;
    }
  pending_boundary = pb_none;
}

#if 0
static const char *
pos(const char **atts)
{
  int i;
  for (i = 0; atts[i] != NULL; ++i)
    if (!strcmp(atts[i],"pos"))
      return atts[i+1];
  return "";
}
#endif

static void
pos_props(const char *pos)
{
  if (pos)
    {
      struct prop_tab *p = propmask(pos,strlen(pos));
      if (p)
	{
	  if (!p->prop)
	    {
	      if (strlen(pos) == 2)
		{
		  int p2 = 0;
		  if (pos[1] == 'N')
		    p2 = PROP_XN;
		  else
		    p2 = PROP_OL;
		  set_property(curr_properties,p2);
		}
	      else
		no_word_props(curr_properties);
	    }
	  else
	    set_property(curr_properties,p->prop);
	}
    }
  else
    no_word_props(curr_properties);
}

static void
loc_project(const char **atts)
{
  strcpy(loc_project_buf, findAttr(atts, "project"));
}

static void
lem_index(struct location8 *l8, const char *field, const char *toks, ...)
{
  struct sn_tab *snp = statnames(field,strlen(field));
  if (snp)
    {
      va_list ap;
      const char *tok;
      int saved_start_column = start_column;

      BIT_SET(start_column,snp->uid);
      va_start(ap,toks);
      tok = toks;
      while (1)
	{
	  int ntok = 0;
	  if (tok)
	    {
	      char *tmp = malloc(strlen(tok)+1), *end;
	      static char manglebuf[128];
	      end = strcpy(tmp,tok);
	      start_column = saved_start_column|snp->uid;
	      if (ntok++)
		grapheme_boundary(' ');

	      while (1)
		{
		  char *t = end;
		  int punct = 0;

		  while (*end && !strchr(". -=,:;", *end))
		    ++end;

		  if (*end)
		    {
		      punct = *end;
		      *end++ = '\0';
		    }

		  if (debug_flag)
		    fprintf(debug_f, "%s %s\n", debug_label, t);

		  grapheme((char*)keymangler((unsigned char*)t,KM_FOLD,manglebuf,128, estp));

		  if (punct)
		    {
		      grapheme_boundary('-');
#if 0
		      if ('=' == punct)
			grapheme_decr_start_column();
#endif
		      punct = 0;
		    }
		  
		  if (!*end)
		    break;
		}
	      tok = va_arg(ap,const char *);
	    }
	  else
	    break;
	}
      va_end(ap);
      BIT_CLR(start_column,snp->uid);
      start_column = saved_start_column;
    }
}

void
startElement(void *userData, const char *name, const char **atts)
{
  if (!strncmp(name,gdl_ns_uri,strlen(gdl_ns_uri)) && indexing)
    startElement_gdl(userData, name + strlen(gdl_ns_uri) + 1, atts);
  else if (!strncmp(name,xtf_ns_uri,strlen(xtf_ns_uri)))
    startElement_xtf(userData, name + strlen(xtf_ns_uri) + 1, atts);
  else if (!strncmp(name,norm_ns_uri,strlen(norm_ns_uri)) && indexing)
    startElement_norm(userData, name + strlen(norm_ns_uri) + 1, atts);
}

void
endElement(void *userData, const char *name)
{
  if (!strncmp(name,gdl_ns_uri,strlen(gdl_ns_uri)) && indexing)
    endElement_gdl(userData, name + strlen(gdl_ns_uri) + 1);
  else if (!strncmp(name,xtf_ns_uri,strlen(xtf_ns_uri)))
    endElement_xtf(userData, name + strlen(xtf_ns_uri) + 1);
  else if (!strncmp(name,norm_ns_uri,strlen(norm_ns_uri)) && indexing)
    endElement_norm(userData, name + strlen(norm_ns_uri) + 1);
}

static void
startElement_xtf(void *userData, const char *name, const char **atts)
{
  /*  flush_graphemes(name[3], EVENT_BEGIN); */
  switch (*name)
    {
    case 't':
      if (!strcmp(name,"transliteration"))
	{
	  reset(indexed_mm);
	  loc_project(atts);
	}
      break;
    case 'c':
      if (!strcmp(name,"composite"))
	{
	  reset(indexed_mm);
	  loc_project(atts);
	}
      break;
    case 'l':
      if (name[1] == '\0')
	{
	  curr_parallel = NULL;
	  begin_indexed();
	  begin_parallels();
	  indexing = 1;
	}
      break;
    }
}

static void
startElement_gdl(void *userData, const char *name, const char **atts)
{
  switch (*name)
    {
    case 'w':
      {
	static char qualified_id[128];
	const char *sig = NULL;

	if (l2)
	  {
	    if (debug_flag)
	      debug_label = xml_id(atts);

	    sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	    wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	  }
	else
	  {
	    strcpy(qualified_id, xml_id(atts));
	    wid2loc8(qualified_id, xml_lang(atts),&l8);
	  }

	/* Index the LEM components */
	if ((sig = hash_find(lemindex, (unsigned char *)qualified_id)))
	  {
	    struct f2 *f2 = NULL;
	    int len;
	    (void)sscanf(sig, "%d.%d:%n",&curr_sentence_id, &curr_lemma_id, &len);
	    sig += len;
	    f2 = hash_find(lemparses, (unsigned char *)sig);
	    pos_props((const char*)f2->pos);
	    if (!strcmp((const char *)f2->lang, "sux"))
	      lem_index(&l8, "n", (const char*)f2->cf, NULL);
	    else
	      lem_index(&l8, "n", (const char*)f2->cf, f2->norm, NULL);
	    lem_index(&l8, "g", (const char*)f2->gw, f2->sense, NULL);
	    lem_index(&l8, "m1", (const char*)f2->morph, NULL);
	    lem_index(&l8, "m2", (const char*)f2->morph2, NULL);
	    /*lem_index(&l8, "p", (const char*)f2->epos);*/
	  }
      }
      break;
    case 'v':
      do_boundary();
      charData_discard();
      break;
    case 'd':
      do_boundary();
      begin_option();
      charData_discard();
      break;
    case 's':
      if (!strcmp(findAttr(atts,"g:role"),"logo"))
	role_logo = 1;
      do_boundary();
      charData_discard();
      break;
    case 'c':
    case 'n':
      do_boundary();
      charData_discard();
      grapheme((const char*)attr_by_name(atts,"form"));
      break;
    case 'q':
      do_boundary();
      in_qualified = 1;
      break;
    }
}

static void
startElement_norm(void *userData, const char *name, const char **atts)
{
  if (!strcmp("w",name))
    charData_discard();
}

static void
endElement_xtf(void *userData, const char *name)
{
  if (*name == 'l' && name[1] == '\0')
    {
      end_parallels();
      end_indexed();
      indexing = 0;
    }
}

static void
endElement_gdl(void *userData, const char *name)
{
  switch (*name)
    {
    case 'w':
      pending_boundary = pb_space;
      break;
    case 'q':
      in_qualified = 0;
      pending_boundary = pb_hyphen;
      break;
    case 'd':
      end_option();
      pending_boundary = pb_hyphen;
      break;
    case 'v':
    case 's':
      {
	Char *g = (Char*)charData_retrieve();
	grapheme((const char *)g);
	if (*name == 'v')
	  {
	    extern Hash_table *signmap;
	    Char *s = hash_find(signmap,g);
	    if (s)
	      {
		progress("indexing value %s as sign %s\n",g, s);
		grapheme_decr_start_column();
		grapheme((const char *)s);
	      }
	  }
	else
	  {
	    if (role_logo)
	      grapheme_end_column_logo();
	    role_logo = 0;
	  }
	if (!in_qualified)
	  pending_boundary = pb_hyphen;
      }
      break;
    case 'c':
    case 'n':
      if (!in_qualified)
	pending_boundary = pb_hyphen;
      break;
    }
}

static void
endElement_norm(void *userData, const char *name)
{
  if (*name == 'w' && name[1] == '\0')
    {
      grapheme((const char *)charData_retrieve());
      pending_boundary = pb_space;
    }
}

void
startLemIndex(void *userData, const char *name, const char **atts)
{
  int post_ns;
  if ((post_ns = is_ns(name,xcl_ns_uri)))
    {
      if (!strcmp(name+post_ns,"l"))
	{
	  const char *sig = findAttr(atts, "sig");
	  if (!sig || !*sig)
	    sig = findAttr(atts, "newsig");
	  if (sig && *sig)
	    {
	      struct f2 *f2 = mb_new(f2_mem);
	      static char qualified_id[128];
	      char *tmp = malloc(strlen(sig)+16);
	      ++curr_lemma_id;

	      f2_parse(NULL, 0, npool_copy((unsigned char *)sig,lempool), f2, NULL, lem_scp);
	      sprintf(qualified_id, "%s:%s", loc_project_buf, 
		      (unsigned char *)findAttr(atts, "ref"));
	      /* add this without the locator prefix */
	      hash_add(lemparses,
		       npool_copy((unsigned char*)sig,lempool),
		       f2);

	      /* add this with the locator prefix */
	      sprintf(tmp, "%d.%d:%s", curr_sentence_id, curr_lemma_id, sig);
	      sig = (const char*)npool_copy((unsigned char *)tmp, lempool);
	      free(tmp);
	      hash_add(lemindex,
		       npool_copy((unsigned char *)qualified_id, lempool),
		       (void*)sig);
	    }
	}
      else if (!strcmp(name+post_ns,"c"))
	{
	  if (!strcmp(findAttr(atts,"type"), "sentence"))
	    {
	      ++curr_sentence_id;
	      curr_lemma_id = 0;
	    }
	  else if (!strcmp(findAttr(atts,"type"), "text"))
	    {
	      curr_sentence_id = 0;
	    }
	}
      return;
    }
  else if ((post_ns = is_ns(name,xtf_ns_uri)))
    {
      name = name + post_ns;
      if (!strcmp(name,"composite") || !strcmp(name,"transliteration"))
	loc_project(atts);
    }
  charData_discard();
}

void
endLemIndex(void *userData, const char *name)
{
  charData_discard();
}

void
dumpalias(unsigned char *a,void*h)
{
  fprintf(keysf,"%s\n",a);
}

int
main (int argc, char **argv)
{
  const char *index_dir, *keys;
  const unsigned char *key;
  Dbi_index *mapdb;

  setlocale(LC_ALL,LOCALE);
  f2_mem = mb_init(sizeof(struct f2), 128);

  options (argc, argv, "2ac:gp:qsx:v");
  project = curr_project;
  set_proxies(proxies_arg);
  alias_index_aliases(curr_project,curr_index);
  if (aliases_only)
    exit(0);

  if (debug_flag)
    debug_f = fopen("sel24x.dbg", "w");

  if (l2)
    vidp = vid_load_data(se_file(curr_project,"cat","vid.dat"));

  f_mangletab = create_mangle_tab(curr_project,"lem");
  
  signmap_init();
  charsets_init();

  lem_scp = sig_context_init();

  index_dir = se_dir (curr_project, curr_index);
  progress ("indexing %s ...\n", index_dir);
  indexed_mm = init_mm (sizeof (struct indexed), 256);
  parallels_mm = init_mm (sizeof (struct parallel), 256);
  grapheme_mm = init_mm (sizeof (struct grapheme), 256);
  node_mm = init_mm (sizeof (struct node), 256);

  /*  alias_check_date ("", TRUE); */
  dip = dbi_create (curr_index, index_dir, 10000, /* hash_create will adjust */
		    sizeof(struct location24), DBI_ACCRETE);
  dbi_set_user(dip,d_l24);
  if (NULL == dip) 
    error (NULL, "unable to create index for %s", curr_index);
  if (cache_elements > 0)
    dbi_set_cache (dip, cache_elements);
  trie_init ();

  estp = est_init(curr_project, "lem");

  alias_fast_init (curr_project, curr_index);

  lempool = npool_init();
  lemindex = hash_create(1024);
  lemparses = hash_create(1024);

  if (one_big_stdin)
    {
#if 1
      fprintf(stderr, "sel24x: one-big-file input no longer supported\n");
      exit(1);
#else
      runexpatNS(i_stdin, NULL, startElement, endElement, "|");
#endif
    }
  else
    {
      char *fn;
      List *files = list_create(LIST_SINGLE);
      while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
	{
	  fn[strlen(fn)-1] = '\0';
	  list_add(files,strdup(fn));
	}
      fnlist = malloc((1+files->count) * sizeof(const char *));
      findex = 0;
      list_exec(files,fn_expand);
      fnlist[findex] = NULL;
      runexpatNS(i_list, fnlist, startLemIndex, endLemIndex, "|");
      runexpatNS(i_list, fnlist, startElement, endElement, "|");
    }

  /*  progress (NULL); */
  trie_term ();
  if (dip->h.entry_count)
    dbi_flush (dip);
  else
    {
      if (!quiet)
	fprintf(stderr,"sel24x: no keys in input\n");
      exit(1);
    }
  dbi_free (dip);

  if (l2)
    {
      vid_free(vidp);
      vidp = NULL;
    }

  est_dump(estp);
  est_term(estp);

  index_dir = se_dir (curr_project, curr_index);
  dip = dbi_open("lem", index_dir);
  keys = se_file (curr_project, "lem", "key.lst");
  keysf = fopen(keys,"w");

  if (aliases)
    hash_exec2(aliases,dumpalias);
  alias_fast_term ();

  while (NULL != (key = (dbi_each(dip))))
    fprintf(keysf,"%s\n",key);
  fclose(keysf);
  dbi_close(dip);

  mapdb = dbi_create ("signmap", se_dir (curr_project, curr_index), 8000, /* hash_create will adjust */
		      PADDED_GRAPHEME_LEN, DBI_BALK);
  signmap_term(mapdb);
  dbi_flush(mapdb);
  /*  dbi_close(mapdb);*/

  mb_free(f2_mem);
  npool_term(lempool);
  hash_free(lemindex, NULL);
  hash_free(lemparses, NULL);

  sig_context_term();
  charsets_term();

  ce_cfg(curr_project,"lem","tr","txh",ce_byid, proxies);

  progress ("index files written to `%s'\n", se_dir(curr_project,curr_index));

  return 0;
}

/********************************************************************/

void
add_graphemes ()
{
  struct grapheme *gnp;
  for (gnp = grapheme_list_base; gnp; gnp = gnp->next)
    {
      Uchar *tmp;
      if (!gnp->text)
	continue;
      if (aliases)
	{
	  tmp  = alias_fast_get_alias (gnp->text);
	  if (verbose && tmp && strcmp((const char*)tmp,(const char *)gnp->text))
	    fprintf(stderr,"%s indexed under alias '%s'\n", gnp->text, tmp);
	  addgraph (dip, tmp ? tmp : gnp->text, &gnp->node->l);
	}
      else
	addgraph (dip, gnp->text, &gnp->node->l);
    }
}

#if 1
static void
fn_expand(void *p)
{
  char *x = l2_expand(NULL, p, "xmd");
  if (!access(x, R_OK))
    {
      char *e = x + strlen(x);
      e[-1] = 'f';
      e[-2] = 't';
      if (!access(x, R_OK))
	{
	  fnlist[findex] = strdup(x);
	  if (verbose)
	    fprintf(stderr,"selemx: found %s\n",fnlist[findex]);
	  ++findex;
	}
      else if (!quiet)
	fprintf(stderr,"selemx: %s not found\n", x);
    }
  else if (!quiet)
    fprintf(stderr,"selemx: %s not found; skipping XTF file\n", x);
}
#else
static void
fn_expand(void *p)
{
  const char **projects = proxies;
  int found = 0;
  while (*projects)
    {
      fnlist[findex] = strdup(l2 
			      ? l2_expand(*projects, p, "xtf") 
			      : expand(*projects, p, "xtf"));
      if (!access(fnlist[findex],R_OK))
	{
	  if (verbose)
	    fprintf(stderr,"found %s\n",fnlist[findex]);
	  ++findex;
	  found = 1;
	  break;
	}
      ++projects;
    }
  if (!found && !quiet)
    fprintf(stderr,"no input file for %s\n",(char*)p);
}
#endif

static void
set_proxies(const char *pxpath)
{
  const char *s = pxpath;
  static char *pxbuf;
  int n;
  n = (*pxpath ? 1 : 0);
  while (*s)
    if (*s++ == ':')
      ++n;
  pxbuf = strdup(pxpath);
  proxies = malloc((2+n) * sizeof(const char*));
  proxies[0] = curr_project;
  for (n = 1; *pxbuf; ++n)
    {
      proxies[n] = pxbuf;
      while (*pxbuf && ':' != *pxbuf)
	++pxbuf;
      if (*pxbuf)
	*pxbuf++ = '\0';
    }
  proxies[n] = NULL;
}

/********************************************************************/

const char *prog = "sel24x";
int major_version = 5, minor_version = 0, verbose;
const char *usage_string = "[-{acgpqsx}] <input>";

void
help()
{
  ;
}

int
opts(int c, char *arg)
{
  switch (c)
    {
    case '2':
      l2 = 1;
      break;
    case 'a':
      aliases_only = 1;
      break;
    case 'c':
      cache_elements = atoi (arg);
      break;
    case 'g':
      debug_flag = 1;
      break;
    case 'p':
      curr_project = arg;
      break;
    case 'q':
      quiet = 1;
      break;
    case 's':
      one_big_stdin = 1;
      break;
    case 'x':
      proxies_arg = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

#include <psd_base.h>
#include <locale.h>
#include <ctype128.h>
#include <dbi.h>
#include <index.h>
#include <alias.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>

#include "atflocale.h"

#include "fields.h"
#include "property.h"
#include "se.h"
#include "selib.h"
#include "vid.h"

static struct est *estp;

const char *textid;
FILE *f_log;

#ifndef strdup
extern char *strdup(const char *);
#endif

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

int l2 = 1;

FILE *mangletab;

static struct vid_data *vidp;

static int aliases_only = 0;

static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex;
int one_big_stdin = 0;

static char loc_project_buf[_MAX_PATH];

const char *curr_project = "cdli", *curr_index = "tra", *proxies_arg = "";
Four_bytes curr_line;
int debug_flag = 0, index_flag = 1;
static int cache_elements = 16;

struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

int indexing = 0;
int quiet = 1;

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

int trax_inword = 0;

static void
loc_project(const char **atts)
{
  strcpy(loc_project_buf, findAttr(atts, "project"));
}


void
trax_startElement(void *userData, const char *name, const char **atts)
{
  /* Ignore namespace prefixes */
  if (strrchr(name,':'))
    name = strrchr(name,':') + 1;

  switch (*name)
    {
    case 't':
      if (!strcmp(name,"translation"))
	{
	  reset(indexed_mm);
	  if (!l2)
	    vid_new_id(vidp, findAttr(atts,"xml:id"));
	  else
	    loc_project(atts);
	}
      charData_discard();
      break;
    case 'p':
      if (name[1] == '\0')
	{
	  curr_parallel = NULL;
	  begin_indexed();
	  begin_parallels();
	}
      charData_discard();
      break;
    case 's':
      if (!strcmp(name,"span"))
	{
	  const char *class=NULL,*form=NULL,*lem=NULL, *xid=NULL, *xlang=NULL;
	  int i;
	  for (i = 0; atts[i]; i+=2)
	    {
	      if (!strcmp(atts[i],"class"))
		class = atts[i+1];
	      else if (!strcmp(atts[i],"xtr:form"))
		form = atts[i+1];
	      else if (!strcmp(atts[i],"xtr:lem"))
		lem = atts[i+1];
	      else if (!strcmp(atts[i],"xml:id"))
		xid = atts[i+1];
	      else if (!strcmp(atts[i],"xml:lang"))
		xlang = atts[i+1];
	    }
	  if (class && !strcmp(class,"w"))
	    {
	      static char qualified_id[128];		  
	      charData_discard();
	      sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	      wid2loc8(vid_map_id(vidp,qualified_id),xlang,&l8);
	      
	      if (lem)
		{
		  grapheme((char*)lem);
		  est_add(lem, estp);
		}
	      if (form)
		{
		  const unsigned char *kmg = keymangler((unsigned char *)form, 
							rulestab[d_tra].ix_manglerules, 
							NULL, 0,
							estp);
		  est_add(form, estp);
		  grapheme((char*)kmg);
		}
	      else
		trax_inword = 1;
	    }
	}
      else
	charData_discard();
      break;
    default:
      charData_discard();
      break;
    }
}

void
trax_endElement(void *userData, const char *name)
{
  if (strrchr(name,':'))
    name = strrchr(name,':') + 1;

  if (*name == 'p' && name[1] == '\0')
    {
      end_parallels();
      end_indexed();
      charData_discard();
    }
  else if (trax_inword)
    {
      Char *g = (Char*)charData_retrieve();
      const unsigned char *kmg = keymangler(g, rulestab[d_tra].ix_manglerules, NULL, 0, estp);
      grapheme((char*)kmg);
      est_add(g, estp);
      trax_inword = 0;
    }
  else
    charData_discard();
}

int
main (int argc, char **argv)
{
  const char *index_dir, *keys, *vids;
  const unsigned char *key;

  options (argc, argv, "2ac:gp:stx:");
  set_proxies(proxies_arg);
  setlocale(LC_ALL,LOCALE);
  if (l2)
    vidp = vid_load_data(se_file(curr_project,"cat","vid.dat"));
  else
    vidp = vid_init();

  km_use_stemmer();
  index_dir = se_dir (curr_project, curr_index);
  progress ("indexing %s ...\n", index_dir);
  indexed_mm = init_mm (sizeof (struct indexed), 256);
  parallels_mm = init_mm (sizeof (struct parallel), 256);
  grapheme_mm = init_mm (sizeof (struct grapheme), 256);
  node_mm = init_mm (sizeof (struct node), 256);

  dip = dbi_create (curr_index, index_dir, 10000, /* hash_create will adjust */
		    sizeof(struct location16), DBI_ACCRETE);
  dbi_set_user(dip,d_tra);
  if (NULL == dip) 
    error (NULL, "unable to create index for %s", curr_index);
  if (cache_elements > 0)
    dbi_set_cache (dip, cache_elements);
  trie_init ();

  estp = est_init(curr_project, "tra");

  if (one_big_stdin)
    {
      runexpat(i_stdin, NULL, trax_startElement, trax_endElement);
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
      runexpat(i_list, fnlist, trax_startElement, trax_endElement);
    }

  trie_term ();
  dbi_flush (dip);
  dbi_free (dip);

  if (l2)
    {
      vid_free(vidp);
      vidp = NULL;
    }
  else
    {
      vids = se_file (curr_project, curr_index, "vid.dat");
      vid_dump_data(vidp,vids);
      vid_term(vidp);
      vidp = NULL;
    }

  index_dir = se_dir (curr_project, curr_index);
  dip = dbi_open(curr_index, index_dir);
  keys = se_file (curr_project, curr_index, "key.lst");
  keysf = fopen(keys,"w");

  while (NULL != (key = (dbi_each(dip))))
    fprintf(keysf,"%s\n",key);
  fclose(keysf);
  dbi_close(dip);

  est_dump(estp);
  est_term(estp);

  km_end_stemmer();

  ce_cfg(curr_project,curr_index,"xh:p","xtr",ce_byid,proxies);
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
  const char *x = l2_expand(NULL, p, "xmd");
  if (!access(x, R_OK))
    {
      x = l2_expand_xtr(NULL, p, "project", "en");
      if (!access(x, R_OK))
	{
	  fnlist[findex] = strdup(x);
	  if (verbose)
	    fprintf(stderr,"setrax: found %s\n",fnlist[findex]);
	  ++findex;
	}
      else if (verbose) /* ordinarily we don't warn about these */
	fprintf(stderr,"setrax: %s not found\n", x);
    }
  else
    fprintf(stderr,"setrax: %s not found; skipping XTR file\n", x);
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
			      ? l2_expand_xtr(*projects, p, "project", "en")
			      : expand_xtr(*projects, p, "project", "en"));
      if (!access(fnlist[findex],R_OK))
	{
	  if (!quiet)
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

const char *prog = "setrax";
int major_version = 5, minor_version = 0, verbose;
const char *usage_string = "[-{acgpsx}] <input>";

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
    case 's':
      one_big_stdin = 1;
      break;
    case 'x':
      proxies_arg = arg;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

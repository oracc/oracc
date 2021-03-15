#include <unistd.h>
#include <locale.h>
#include <atflocale.h>
#include <psd_base.h>
#include <ctype128.h>
#include <dbi.h>
#include <index.h>
#include <alias.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>
#include "oracclocale.h"

#include "fields.h"
#include "property.h"
#include "se.h"
#include "selib.h"
#include "vid.h"

#include <sys/unistd.h>

int swc_flag = 0;
int l2 = 1;

struct vid_data *vidp;

#ifndef strdup
extern char *strdup(const char *);
#endif

struct est *estp;
extern void grapheme_decr_start_column(void);
extern void grapheme_end_column_logo(void);
extern void grapheme_inherit_preceding_properties(void);
extern void grapheme_reset_start_column(void);

extern void trie_init(void);
extern void trie_term(void);

#undef xmalloc
#undef xrealloc
#undef xcalloc
#undef xfree
#define xmalloc malloc
#define xrealloc realloc
#define xcalloc calloc
#define xfree free

Dbi_index *dip;

static int aliases_only = 0;
static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex;
int one_big_stdin = 0;
char loc_project_buf[_MAX_PATH];
int verbose = 0;

const char *curr_project = "cdli", *curr_index = "txt", *proxies_arg = "";
const char *project = NULL; /* for now we need this for vid.c */
Four_bytes curr_line;
int debug_flag = 0, index_flag = 1;
static int cache_elements = 16;

char curr_id[32];
struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

int indexing = 0;
int quiet = 1;

const char **proxies = NULL;

FILE *f_mangletab = NULL, *signmap_err = NULL;

static FILE *keysf;
/* static void process_cdata(Uchar*); */
static void fn_expand(void *p);

enum pending_boundary_types pending_boundary = pb_none;

extern void grapheme_decr_start_column(void);
extern void signmap_init(void);
extern void signmap_term(Dbi_index *);
static void set_proxies(const char *pxpath);

static const char *
pos(const char **atts)
{
  int i;
  for (i = 0; atts[i] != NULL; ++i)
    if (!strcmp(atts[i],"pos"))
      return atts[i+1];
  return "";
}

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

void
startElement(void *userData, const char *name, const char **atts)
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
    case 's':
      if (!strcmp(name,"score"))
	{
	  reset(indexed_mm);
	  loc_project(atts);
	}
      break;
    case 'l':
      if (name[1] == '\0')
	{
	  curr_properties = 0;
	  curr_parallel = NULL;
	  begin_indexed();
	  begin_parallels();
	}
      break;
    case 'v':
      if (name[1] == '\0')
	{
	  begin_indexed();
	  begin_parallels();
	}
      break;
    case 'g':
      if (curr_node && name[1] == ':' && name[3] == '\0')
	{
	  extern void gdlStartElement(void*userData,const char *name, const char **atts);
	  gdlStartElement(userData,name,atts);
	}
      else
	{
	  if (!strcmp("g:swc",name))
	    {
	      static char qualified_id[128];
	      const char *headref = findAttr(atts,"headref");
	      pos_props(pos(atts));
	      sprintf(qualified_id, "%s:%s", loc_project_buf, headref);
	      /* fprintf(stderr,"setxtx: setting word loc from headref %s\n", headref); */
	      wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	    }
	}
    case 'n':
      if (!strcmp("n:w",name))
	{
	  static char qualified_id[128];
	  const unsigned char *n = NULL;
	  pos_props(pos(atts));
	  sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	  wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	  n = (const unsigned char*)attr_by_name(atts,"form");
	  grapheme((const char *)n);
	  est_add(n, estp);
	  charData_discard();
	}
    }
}

void
endElement(void *userData, const char *name)
{
  if (*name == 'v' && name[1] == '\0')
    {
      end_parallels();
      end_indexed();
    }
  if (*name == 'l' && name[1] == '\0')
    {
      end_parallels();
      end_indexed();
    }
  else if (curr_node && *name == 'g' && name[1] == ':' && name[3] == '\0')
    {
      extern void gdlEndElement(void*userData,const char *name);
      gdlEndElement(userData,name);
    }
  else if ('n' == *name && !strcmp("n:w",name))
    {
      pending_boundary = pb_space;
    }
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

  setlocale(LC_ALL,ORACC_LOCALE);
  options (argc, argv, "2ac:gp:sx:v");
  project = curr_project;
  set_proxies(proxies_arg);
  alias_index_aliases(curr_project,curr_index);
  if (aliases_only)
    exit(0);

  if (l2)
    vidp = vid_load_data(se_file(curr_project,"cat","vid.dat"));

  f_mangletab = create_mangle_tab(curr_project,"txt");

  if (!(signmap_err = fopen("01tmp/signmap.log", "w")))
    {
      fprintf(stderr, "setxtx: unable to write to 01tmp/signmap.log. Stop.\n");
      exit(1);
    }

  signmap_init();

  index_dir = se_dir (curr_project, curr_index);
  progress ("indexing %s ...\n", index_dir);
  indexed_mm = init_mm (sizeof (struct indexed), 256);
  parallels_mm = init_mm (sizeof (struct parallel), 256);
  grapheme_mm = init_mm (sizeof (struct grapheme), 256);
  node_mm = init_mm (sizeof (struct node), 256);

  /*  alias_check_date ("", TRUE); */
  dip = dbi_create (curr_index, index_dir, 10000, /* hash_create will adjust */
		    sizeof(struct location16), DBI_ACCRETE);
  dbi_set_user(dip,d_txt);
  if (NULL == dip) 
    error (NULL, "unable to create index for %s", curr_index);
  if (cache_elements > 0)
    dbi_set_cache (dip, cache_elements);
  trie_init ();

  estp = est_init(curr_project, "txt");

  alias_fast_init (curr_project, curr_index);

  if (one_big_stdin)
    {
      runexpat(i_stdin, NULL, startElement, endElement);
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
      runexpat(i_list, fnlist, startElement, endElement);
    }

  /*  progress (NULL); */
  trie_term ();
  dbi_flush (dip);
  dbi_free (dip);

  est_dump(estp);
  est_term(estp);

  if (l2)
    {
      vid_free(vidp);
      vidp = NULL;
    }

  index_dir = se_dir (curr_project, curr_index);
  dip = dbi_open("txt", index_dir);
  keys = se_file (curr_project, "txt", "key.lst");
  keysf = fopen(keys,"w");

  if (aliases)
    hash_exec2(aliases,(void (*)(const unsigned char *, void *))dumpalias);
  alias_fast_term ();

  while (NULL != (key = (dbi_each(dip))))
    fprintf(keysf,"%s\n",key);
  fclose(keysf);
  dbi_close(dip);

  mapdb = dbi_create ("signmap", se_dir (curr_project, curr_index), 8000, /* hash_create will adjust */
		      PADDED_GRAPHEME_LEN, DBI_BALK);
  signmap_term(mapdb);
  fclose(signmap_err);
  
  dbi_flush(mapdb);
  /*  dbi_close(mapdb);*/

  ce_cfg(curr_project,"txt","tr","txh",ce_byid, proxies);

  progress ("index files written to `%s'\n", se_dir(curr_project,curr_index));

  return 0;
}

/********************************************************************/

static void
fn_expand(void *p)
{
  char *x = (char*)l2_expand(NULL, p, "xmd");
  if (!access(x, R_OK))
    {
      char *e = x + strlen(x);
      e[-1] = 'f';
      e[-2] = 's';
      /* try .xsf files first to index full scores if the .xtf is
	 derived from them */
      if (!access(x, R_OK))
	{
	  fnlist[findex] = strdup(x);
	  if (!quiet)
	    fprintf(stderr,"setxtx: found %s\n",fnlist[findex]);
	  ++findex;
	}
      else
	{
	  e[-2] = 't';
	  if (!access(x, R_OK))
	    {
	      fnlist[findex] = strdup(x);
	      if (!quiet)
		fprintf(stderr,"setxtx: found %s\n",fnlist[findex]);
	      ++findex;
	    }
	  else 
	    fprintf(stderr,"setxtx: %s not found\n", x);
	}
    }
  else
    fprintf(stderr,"setxtx: %s not found; skipping XTF file\n", x);
}

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

const char *prog = "setxtx";
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
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

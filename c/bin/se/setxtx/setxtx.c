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

#include "fields.h"
#include "property.h"
#include "se.h"
#include "selib.h"
#include "vid.h"

#include <sys/unistd.h>

int l2 = 1;

static struct vid_data *vidp;

#ifndef strdup
extern char *strdup(const char *);
#endif

static struct est *estp;

extern void grapheme_decr_start_column(void);
extern void grapheme_end_column_logo(void);
extern void grapheme_inherit_preceding_properties(void);

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

const char *curr_project = "cdli", *curr_index = "txt", *proxies_arg = "";
const char *project = NULL; /* for now we need this for vid.c */
Four_bytes curr_line;
int debug_flag = 0, index_flag = 1;
static int cache_elements = 16;

struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

int indexing = 0;
int quiet = 1;

const char **proxies = NULL;

FILE *f_mangletab = NULL;

static FILE *keysf;
/* static void process_cdata(Uchar*); */
static void fn_expand(void *p);

enum pending_boundary_types { pb_none , pb_space , pb_hyphen };
enum pending_boundary_types pending_boundary = pb_none;

extern void grapheme_decr_start_column(void);
extern void signmap_init(void);
extern void signmap_term(Dbi_index *);
static void set_proxies(const char *pxpath);

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
  const char *v_delim = NULL, *data = NULL;

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
	switch (name[2])
	  {
	  case 'w':
	    {
#if 1
	      static char qualified_id[128];
	      pos_props(pos(atts));
	      sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	      wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	      est_add((const unsigned char*)attr_by_name(atts,"form"), estp);
#else
	      if (l2)
		{
		  static char qualified_id[128];
		  pos_props(pos(atts));
		  sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
		  wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
		}
	      else
		wid2loc8(xml_id(atts),xml_lang(atts),&l8);

	      for (i = 0; atts[i]; i += 2)
		{
		  struct sn_tab *snp = statnames(atts[i],strlen(atts[i]));
		  if (snp)
		    {
		      static struct location16 l16;
		      memcpy(&l16,&l8,sizeof(l8));
		      /* so citeform=a and pos=N will work */
		      l16.properties = curr_properties;
		      l16.start_column = snp->uid;
		      l16.end_column = l16.branch_id = 0;
		      addgraph(dip, , &l16);
		      est_add((unsigned char*)atts[i+1],estp);
		    }
		}
#endif
	    }
	    break;
	  case 'v':
	    v_delim = findAttr(atts, "g:delim");
	    if (v_delim && *v_delim == ' ')
	      pending_boundary = pb_space;
	    else if (v_delim && *v_delim)
	      pending_boundary = pb_hyphen;
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
	    data = (const char*)attr_by_name(atts,"form");
	    grapheme(data);
	    est_add((const unsigned char *)data, estp);
	    break;
	  case 'q':
	    do_boundary();
	    in_qualified = 1;
	    break;
	  }
    case 'n':
      if (!strcmp("n:w",name))
	{
	  static char qualified_id[128];
	  pos_props(pos(atts));
	  sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	  wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	  est_add((const unsigned char*)attr_by_name(atts,"form"), estp);
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
      switch (name[2])
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
	    est_add(g, estp);
	    if (name[2] == 'v')
	      {
		extern Hash_table *signmap;
		Char *s = hash_find(signmap,g);
		if (s)
		  {
		    progress("indexing value %s as sign %s\n",g, s);
		    grapheme_decr_start_column();
		    grapheme((const char *)s);
		    est_add(g, estp);
		    grapheme_inherit_preceding_properties();
		    do_boundary();
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
  else if ('n' == *name && !strcmp("n:w",name))
    {
      const char *n = (const char *)charData_retrieve();
      grapheme(n);
      est_add((const unsigned char *)n, estp);
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

  setlocale(LC_ALL,LOCALE);
  options (argc, argv, "2ac:gp:sx:v");
  project = curr_project;
  set_proxies(proxies_arg);
  alias_index_aliases(curr_project,curr_index);
  if (aliases_only)
    exit(0);

  if (l2)
    vidp = vid_load_data(se_file(curr_project,"cat","vid.dat"));

  f_mangletab = create_mangle_tab(curr_project,"txt");

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
  dbi_flush(mapdb);
  /*  dbi_close(mapdb);*/

  ce_cfg(curr_project,"txt","tr","txh",ce_byid, proxies);

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

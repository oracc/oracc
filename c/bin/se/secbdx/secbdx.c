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
#include "atflocale.h"

#include "fields.h"
#include "property.h"
#include "se.h"
#include "selib.h"
#include "ce.h"

#ifndef strdup
extern char *strdup(const char *);
#endif

FILE *f_idlist;
FILE *f_log;
static struct est *estp;

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

static char curr_id[32];
int one_big_stdin = 0;
extern int branch_id;

const char *curr_project = "cdli", *curr_index = "cbd";
const char *lang;
char *idlist;
Four_bytes curr_line;
int debug_flag = 0, index_flag = 1;
static int cache_elements = 16;

struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

int indexing = 0;

static FILE *keysf;
static void process_cdata(Uchar*);

/*CAUTION: THESE NAMES MUST BE KEPT ALIGNED WITH field_names IN fields.h*/
const char *static_names[] = 
  {
    "cf", "gw", "t", "mng", "mean", "cfgw", "term", "n", "nofield" , "not_in_use", "next_uid"
  };

/* This indexer ignores the fact that senses and bases can have their own
   ids; it treats everything as transliteration, and as a result aliases
   all languages--this is going to come back to bite us in the ass some day.

   At that point we should at least switch aliasing by language.
 */
void
startElement(void *userData, const char *name, const char **atts)
{
  switch (*name)
    {
    case 'e':
      if (name[5] == 'i' && !strcmp(name,"entries"))
	reset(indexed_mm);
      else if (!strcmp(name,"entry"))
	{
	  branch_id = 0;
	  curr_parallel = NULL;
	  begin_indexed();
	  begin_parallels();
	  strcpy(curr_id,xml_id(atts));
	  fprintf(f_idlist, "%s\n", curr_id);
	}
      break;
    case 'b':
    case 'f':
      if (!name[4] && (!strcmp(name,"base") || !strcmp(name, "form")))
	{
	  wid2loc8(curr_id,xml_lang(atts),&l8);
	  process_cdata((unsigned char*)findAttr(atts,"n"));
	}
      break;
    case 'n':
      if (!name[1])
	wid2loc8(curr_id,xml_lang(atts),&l8);
      break;
    default:
      if ((!name[1] && *name == 't')
	  || (!name[2] && (!strcmp(name,"cf")||!strcmp(name,"gw")))
	  || (!name[3] && (!strcmp(name,"mng")))
	  || (!name[4] && (!strcmp(name,"mean")||!strcmp(name,"term"))))
	wid2loc8(curr_id,xml_lang(atts),&l8);
      break;
    }
}

void
endElement(void *userData, const char *name)
{
  static int norm_count = 0;
  if (*name == 'e' && !strcmp(name,"entry"))
    {
      end_parallels();
      end_indexed();
    }
  else if ((!name[1] && *name == 't')
	   || (!name[1] && (!strcmp(name,"n")))
	   || (!name[2] && (!strcmp(name,"cf")||!strcmp(name,"gw")))
	   || (!name[3] && (!strcmp(name,"mng")))
	   || (!name[4] && (!strcmp(name,"mean")||!strcmp(name,"term"))))
    {
      static char cfgw[256];
      struct sn_tab* sntabp;
      char *data = charData_retrieve();
      est_add((unsigned char *)data, estp);
      if (!strcmp(name,"cf"))
	{
	  strcpy(cfgw,data);
	}
      else if (!strcmp(name,"gw"))
	{
	  sprintf(cfgw+strlen(cfgw),"[%s]",data);
	  l8.unit_id = sn_cfgw;
	  grapheme(cfgw);
	}

      if (NULL != (sntabp = statnames(name,strlen(name))))
	l8.unit_id = sntabp->uid;
      else
	fprintf(stderr,"secbdx: indexed field %s not in statnames\n",name);
      if (l8.unit_id == sn_norm)
	++norm_count;
      if (l8.unit_id == sn_mng || l8.unit_id == sn_norm)
	{
	  begin_branch();
	  process_cdata((Uchar*)data);
	  end_branch();
	}
      else
	process_cdata((Uchar*)data);
    }
  else
    charData_discard();
}

/*#define HYPHEN(c) ((c)=='.'||(c)=='-'||(c)==':'||(c)=='+'||(c)=='*') */
#define HYPHEN(c) ((c)=='-')
#define SPACE(c)  ((c <  128)&&(isspace(c)||(c)==','||(c)==';'))
#define JUNK(c)   ((c)=='['||(c)==']'||(c)=='?'||(c)=='!')

static void
process_cdata(Uchar*cdata) {
  unsigned char *s = cdata;
  unsigned char *graph;
  static unsigned char buf[1024];
  int wid = 0;

  if (!*cdata)
    return;

  s = cdata = (unsigned char *)strcpy((char*)buf,(const char *)cdata);  

  while (*s && (HYPHEN(*s) || SPACE(*s)))
    ++s;
  graph = s;
  while (*s) {
    if (HYPHEN(*s)) {
      grapheme_boundary('-');
      do
	++s;
      while (*s && (HYPHEN(*s) || SPACE(*s)));
      graph = s;
    } else if (SPACE(*s)) {
      l8.word_id = ++wid;
      grapheme_boundary(' ');
      do
	++s;
      while (*s && (HYPHEN(*s) || SPACE(*s)));
      graph = s;
    } else if (s[1] == '\0' || HYPHEN(s[1]) || SPACE(s[1])
	       || s[1] == '{' || s[1] == '}') {
      char saved_char = (*s ? *++s : *s);
      if (*graph && graph < s)
	{
#if 1
	  const unsigned char *kmg = NULL;
	  *s = '\0';
	  est_add(graph,estp);
	  kmg = keymangler((unsigned char *)graph, 
			   rulestab[d_cbd].ix_manglerules, NULL, 0,
			   estp);
#if 0
	  if (strcmp(kmg,graph))
	    fprintf(stderr,"mangled %s to %s\n",graph,kmg);
#endif
	  grapheme((char*)kmg);
#endif
	  *s = saved_char;
	}
      graph = s+1;
    } else if (*s == '{') {
      begin_option();
      graph = ++s;
    } else if (*s == '}') {
      end_option();
      graph = ++s;
    } else {
      ++s;
    }
  }
  grapheme_boundary(' ');
}

void
dumpalias(unsigned char *a,void*h)
{
  fprintf(keysf,"%s\n",a);
}

int
main(int argc, char **argv)
{
  char *index_dir;
  const char *keys;
  const unsigned char *key;
  char buf[128];
  char idxlang[1024];
  FILE *fldsf;
  const char *fldsname;
  int i, top;

  f_log = stderr;

  options(argc, argv, "ac:gl:p:s");

  setlocale(LC_ALL,LOCALE);

  alias_index_aliases(curr_project,curr_index);
  if (aliases_only)
    exit(0);

  index_dir = (char*)se_dir(curr_project, curr_index);
  if (lang)
    {
      strcat(index_dir,"/");
      strcat(index_dir,lang);
      sprintf(idxlang, "cbd/%s", lang);
    }
  index_dir = strdup(index_dir);

  progress("indexing %s ...\n", index_dir);
  indexed_mm = init_mm(sizeof (struct indexed), 256);
  parallels_mm = init_mm(sizeof (struct parallel), 256);
  grapheme_mm = init_mm(sizeof (struct grapheme), 256);
  node_mm = init_mm(sizeof (struct node), 256);

  /*  alias_check_date ("", TRUE); */
  dip = dbi_create(curr_index, index_dir, 10000, /* hash_create will adjust */
		    sizeof(struct location16), DBI_ACCRETE);
  dbi_set_user(dip,d_cbd);
  if (NULL == dip) 
    error (NULL, "unable to create index for %s", curr_index);
  if (cache_elements > 0)
    dbi_set_cache(dip, cache_elements);
  trie_init ();

  estp = est_init(curr_project, idxlang);

  alias_fast_init(curr_project, curr_index);

  idlist = malloc(strlen(index_dir)+strlen("entry_ids.lst")+2);
  sprintf(idlist, "%s/%s", index_dir, "entry_ids.lst");
  f_idlist = xfopen(idlist, "w");

  if (one_big_stdin)
    {
      runexpat(i_stdin, NULL, startElement, endElement);
    }
  else
    {
#if 1
      runexpat(i_list, &argv[optind], startElement, endElement);
#else
      char *fn;
      List *files = list_create(LIST_SINGLE);
      while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
	{
	  fn[strlen(fn)-1] = '\0';
	  list_add(files,strdup(fn));
	}
      fnlist = malloc(1+files->count * sizeof(const char *));
      findex = 0;
      list_exec(files,fn_expand);
      fnlist[findex] = NULL;
      runexpat(i_list, fnlist, startElement, endElement);
#endif
    }

  /*  progress (NULL); */
  xfclose(idlist, f_idlist);
  free(idlist);
  idlist = NULL;
  trie_term();
  dbi_flush(dip);
  dbi_free(dip);
  dip = dbi_open("cbd", index_dir);
  
  est_dump(estp);
  est_term(estp);

  if (lang)
    sprintf(buf,"cbd/%s",lang);
  else
    strcpy(buf,"cbd");

  fldsname = strdup(se_file(curr_project,buf,"fieldnames.tab"));
  fldsf = xfopen(fldsname,"w");
  for (i = 0,top=(sizeof(static_names)/sizeof(const char *))-3; 
       i < top; 
       ++i)
    {
      struct sn_tab *s = statnames(static_names[i],strlen(static_names[i]));
      fprintf(fldsf,"%s\t%d\n",static_names[i],s->uid);
    }
  xfclose(fldsname,fldsf);

  keys = se_file(curr_project, buf, "key.lst");
  keysf = xfopen(keys,"w");

  if (aliases)
    hash_exec2(aliases,dumpalias);
  alias_fast_term();

  while (NULL != (key = (dbi_each(dip))))
    fprintf(keysf,"%s\n",key);
  fclose(keysf);
  dbi_close(dip);

  ce_cfg(curr_project, buf, "p", NULL, ce_summary, NULL);

  progress("index files written to `%s'\n", index_dir);

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
#if 0
	  if (tmp && strcmp(tmp,gnp->text))
	    fprintf(stderr,"%s indexed under alias '%s'\n", gnp->text, tmp);
#endif
	  addgraph (dip, tmp ? tmp : gnp->text, &gnp->node->l);
	}
      else
	addgraph (dip, gnp->text, &gnp->node->l);
    }
}

/********************************************************************/

const char *prog = "secbdx", *textid;
int major_version = 5, minor_version = 0, verbose;
const char *usage_string = "[-{acgps}] <input>";

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
    case 'a':
      aliases_only = 1;
      break;
    case 'c':
      cache_elements = atoi(arg);
      break;
    case 'g':
      debug_flag = 1;
      break;
    case 'l':
      lang = arg;
      break;
    case 'p':
      curr_project = arg;
      break;
    case 's':
      one_big_stdin = 1;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

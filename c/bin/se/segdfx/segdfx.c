#include <psd_base.h>
#include <fname.h>
#include <list.h>
#include <dbi.h>
#include <options.h>
#include <ctype128.h>
#include <locale.h>
#include <runexpat.h>
#include "atflocale.h"
#include "oracclocale.h"
#include "../types.h"
#include "../selib.h"
#include "../se.h"
#include "../secatx/fields.h"
#include "../vid.h"

#undef C
#define C(x) #x,

static struct est *estp;

const char *static_names[] = 
  {
    field_names "nofield" , "not_in_use", "next_uid"
  };

#ifndef strdup
extern char *strdup(const char *);
#endif

int l2 = 1;

extern const char *textid;
extern FILE *f_log;
FILE *f_mangletab = NULL;

const char *curr_project = NULL;
static Dbi_index*dp;
static FILE *pqidsf;
static void process_cdata(Uchar*cdata);
static struct location8 l8;
static int indexing = 0;
static FILE *fldsf;
static int cache_size = 16;

const char *curr_index = "cat";

static struct vid_data *vidp;

/*
 * Dynamic field name support
 */
static Hash_table *dyntab;

struct sn_tab *
dynanames(const char *fname)
{
  if (dyntab)
    return hash_find(dyntab,(Uchar *)fname);
  else
    return NULL;
}
int
dn_add_name(const char *fname)
{
  static int dn_next_uid = next_uid;
  struct sn_tab *s = malloc(sizeof(struct sn_tab));
  s->name = strdup(fname);
  s->uid = ++dn_next_uid;
  if (!dyntab)
    dyntab = hash_create(16);
  hash_add(dyntab, (Uchar *)fname, s);
  progress("adding %s with uid %d\n",fname,s->uid);
  fprintf(fldsf,"%s\t%d\n",fname,s->uid);
  return s->uid;
}

/* 
 * INDEXING CODE 
 */
static void
startElement(void *userData, const char *name, const char **atts)
{
  const char *curr_id = NULL;
  if (!strcmp(name,"o:record"))
    {
      curr_id = findAttr(atts, "xml:id");
      vid_new_id(vidp, curr_id);
      fprintf(pqidsf,"%s\n",curr_id);
      loc8(vid_map_id(vidp,curr_id), 0, lang_mask(atts), &l8);
      indexing = 1;
    }
}

static void
endElement(void *userData, const char *tag)
{
  if (!strcmp(tag,"o:record"))
    indexing = 0;
  else if (indexing)
    {
      struct sn_tab* sntabp;
      if (NULL != (sntabp = dynanames(tag)))
	l8.unit_id = sntabp->uid;
      else
	l8.unit_id = dn_add_name(tag);
      process_cdata((Uchar*)xstrdup(charData_retrieve()));
    }
}

static void
process_cdata(Uchar*cdata) {
  unsigned char *s = cdata;
  unsigned char *word;

  if (!*cdata)
    return;

  while (*s && isspace(*s))
    ++s;

  l8.word_id = 0;
  word = s;
  while (*s) {
    if (isspace(*s)) {
      do
	++s;
      while (*s && isspace(*s));
      ++l8.word_id;
      word = s;
    } else if (s[1] == '\0' || isspace(s[1])) {
      char saved_char = (*s ? *++s : *s);
      if (*word && word < s)
	{
	  const unsigned char *kmg = NULL;
	  *s = '\0';
	  kmg = keymangler(word, 
			   rulestab[d_cat].ix_manglerules, NULL, 0,
			   estp, "gdf");
	  if (strlen((char*)kmg) > 63)
	    {
	      char buf[64];
	      strncpy(buf,(const char *)kmg,63);
	      buf[63] = '\0';
	      dbi_add(dp,(unsigned char*)buf,&l8,1);
	    }
	  else
	    dbi_add(dp,(unsigned char*)kmg,&l8,1);
	  *s = saved_char;
	}
      word = s+1;
    } else {
      ++s;
    }
  }
}

/*
 * PROGRAM WRAPPER
 */

int
main(int argc, char * const*argv)
{
  unsigned char *key;
  FILE*keysf;
  const char *keysname, *fldsname, *vids;
  char *gdfpath = NULL;
  extern int vid_obey_dots;

  f_log = stderr;

  options(argc,argv,"p:");

  if (!curr_project)
    usage();

  setlocale(LC_ALL,ORACC_LOCALE);

  vid_obey_dots = 0;
  vidp = vid_init();

  gdfpath = malloc(strlen(curr_project)+1);
  sprintf(gdfpath, "%s", curr_project);

  estp = est_init(gdfpath, "cat");

  dp = dbi_create("cat",
		  se_dir(gdfpath,"cat"),
		  500000,
		  sizeof(struct location8),DBI_ACCRETE);
  dbi_set_cache(dp,cache_size);
  dbi_set_user(dp,d_cat);

  progress("segdfx: creating index %s\n",dp->dir);
  pqidsf = xfopen(se_file(gdfpath,"cat","pqids.lst"),"w");
  fldsname = strdup(se_file(gdfpath,"cat","fieldnames.tab"));
  fldsf = xfopen(fldsname,"w");
  
  runexpat(i_stdin, NULL, startElement, endElement);

  dbi_flush(dp);
  dbi_free(dp);

  est_dump(estp);
  est_term(estp);

  vids = se_file (gdfpath, curr_index, "vid.dat");
  vid_dump_data(vidp,vids);
  vid_term(vidp);
  vidp = NULL;

  progress("segdfx: completed db creation\n");

  dp = dbi_open("cat",se_dir(gdfpath,"cat"));
  if (dp && dp->h.entry_count > 0)
    {
      keysname = strdup(se_file(gdfpath,"cat","key.lst"));
      keysf = xfopen(keysname,"w");
      while (NULL != (key = dbi_each (dp)))
	fprintf(keysf,"%s\n",key);
      xfclose(keysname,keysf);
    }
  else
    {
      fprintf(stderr,"segdfx: no keys in input\n");
    }
  dbi_close(dp);
  xfclose(fldsname,fldsf);

  ce_cfg(gdfpath, "cat", "catalog", "xmd", ce_xmd, NULL);

  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'p':
      curr_project = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "segdfx";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-p [project] [-s]";
void
help ()
{
  printf("  -p [project] Gives the name of the project; required\n");
}

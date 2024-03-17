#include <oraccsys.h>
#include <ctype128.h>
#include <locale.h>
#include "../selib.h"
#include "../se.h"
#include "fields.h"

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

extern FILE *f_log;
FILE *f_mangletab = NULL;

static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex;
const char *curr_project = NULL;
static void fn_expand(void*);
static int one_big_stdin = 0;
static Dbi_index*dp;
static FILE *pqidsf;
static void process_cdata(Uchar*cdata);
static struct location8 l8;
static int indexing = 0;
static FILE *fldsf;
static int cache_size = 16;

static struct vid_data *vidp;

const char *curr_index = "cat";

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
  const char *curr_text_pq, *curr_text_proj;
  if (!strcmp(name,"xmd"/*"key"*/))
    {
      curr_text_pq = findAttr(atts, "xml:id");
      if (l2)
	{
	  static char qualified_id[128];
	  curr_text_proj = findAttr(atts, "project");
	  sprintf(qualified_id, "%s:%s", curr_text_proj, curr_text_pq);	  
	  fprintf(pqidsf,"%s\n",qualified_id);
	  loc8(vid_map_id(vidp,qualified_id), 0, lang_mask(atts), &l8);
	}
      else
	{
	  fprintf(pqidsf,"%s\n",curr_text_pq);
	  loc8(curr_text_pq, 0, lang_mask(atts), &l8);
	}
      indexing = 1;
    }
}

static void
endElement(void *userData, const char *tag)
{
  if (!strcmp(tag,"xmd"/*"key"*/))
    indexing = 0;
  else if (indexing)
    {
      struct sn_tab* sntabp;
      if (NULL != (sntabp = statnames(tag,strlen(tag))))
	l8.unit_id = sntabp->uid;
      else if (NULL != (sntabp = dynanames(tag)))
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
			   estp, "cat");
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
  List *files = list_create(LIST_SINGLE);
  char *fn;
  unsigned char *key;
  FILE*keysf;
  const char *keysname, *fldsname;
  int i,top;

  f_log = stderr;

  options(argc,argv,"2c:p:s");

  if (!curr_project)
    usage();

  setlocale(LC_ALL,ORACC_LOCALE);

  if (l2)
    vidp = vid_load_data(se_file(curr_project,"cat","vid.dat"));

  f_mangletab = create_mangle_tab(curr_project,"cat");
  
  estp = est_init(curr_project, "cat");

  dp = dbi_create("cat",
		  se_dir(curr_project,"cat"),
		  500000,
		  sizeof(struct location8),DBI_ACCRETE);
  dbi_set_cache(dp,cache_size);
  dbi_set_user(dp,d_cat);

  progress("secatx: creating index %s\n",dp->dir);
  pqidsf = xfopen(se_file(curr_project,"cat","pqids.lst"),"w");
  fldsname = strdup(se_file(curr_project,"cat","fieldnames.tab"));
  fldsf = xfopen(fldsname,"w");
  
  for (i = 0,top=(sizeof(static_names)/sizeof(const char *))-3; 
       i < top; 
       ++i)
    {
      struct sn_tab *s = statnames(static_names[i],strlen(static_names[i]));
      fprintf(fldsf,"%s\t%d\n",static_names[i],s->uid);
    }

  if (one_big_stdin)
    {
      runexpat(i_stdin, NULL, startElement, endElement);
    }
  else
    {
      while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
	{
	  fn[strlen(fn)-1] = '\0';
	  list_add(files,strdup(fn));
	}
      fnlist = malloc(1+files->count * sizeof(const char *));
      findex = 0;
      list_exec(files,fn_expand);
      runexpat(i_list, fnlist, startElement, endElement);
    }

  dbi_flush(dp);
  dbi_free(dp);

  est_dump(estp);
  est_term(estp);

  if (l2)
    {
      vid_free(vidp);
      vidp = NULL;
    }

  progress("secatx: completed db creation\n");
  dp = dbi_open("cat",se_dir(curr_project,"cat"));
  if (dp && dp->h.entry_count > 0)
    {
      keysname = strdup(se_file(curr_project,"cat","key.lst"));
      keysf = xfopen(keysname,"w");
      while (NULL != (key = dbi_each (dp)))
#if 1
	fprintf(keysf,"%s\n",key);
#else
      fprintf(keysf,"%lu\t%s\n",(unsigned long)dp->nfound,key);
#endif
      xfclose(keysname,keysf);
    }
  else
    {
      fprintf(stderr,"secatx: no keys in input\n");
    }
  dbi_close(dp);
  xfclose(fldsname,fldsf);

  ce_cfg(curr_project, "cat", "catalog", "xmd", ce_xmd, NULL);

  return 0;
}

static void
fn_expand(void *p)
{
  fnlist[findex++] = strdup(expand(curr_project, p, "key"));
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case '2':
      l2 = 1;
      break;
    case 'c':
      cache_size = atoi(arg);
      break;
    case 'p':
      curr_project = arg;
      break;
    case 's':
      one_big_stdin = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "secatx";
int major_version = 6, minor_version = 0, verbose;
const char *usage_string = "-p [project] [-s]";
void
help ()
{
  printf("  -p [project] Gives the name of the project; required\n");
  printf("  -s           Read input from stdin rather than PQ ids\n");
}

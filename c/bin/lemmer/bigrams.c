#include <ctype128.h>
#include <psd_base.h>
#include <sys/unistd.h>
#include <options.h>
#include <xmem.h>
#include <memblock.h>
#include <fname.h>
#include <xmlparse.h>
#include <runexpat.h>
#include <list.h>
#include <hash.h>
#include <npool.h>
#include <xcl.h>

int verbose = 0;

#define XCL_LOCAL_NAME_OFFSET	27

#define uc(x)  ((const unsigned char *)x)
#define ucc(x) ((const unsigned char *)x)
#define xpool_copy(x) (char*)npool_copy((unsigned char *)x,pool)

#ifndef strdup
extern char *strdup(const char *);
#endif

List *c_list;

int fuzzy_aliasing = 0;
int lem_autolem = 0;

char *tmp_sig = NULL;
const unsigned char *head_bigram = NULL;
static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex = 0;
static const char *ext = "xtf";
static const char *inputfile = NULL;
static const char *outputbase = NULL;
static const char *project = NULL;
static const char *iproject = NULL;

int from_stdin = 0;
static void fn_expand(void*);

#define _MAX_LANG 	32

char curr_sig_lang[_MAX_LANG], *curr_sig;
char last_sig_lang[_MAX_LANG], *last_sig;
Hash_table *bigrams, *bigram_lang_hashes;
struct bgm_tab { int freq; unsigned char *bgm; } *bgms;

struct mb *mb_ints;
struct npool *pool;

int total_bigrams = 0, nbgms = 0;

static void
clear_last_sig(void)
{
  if (last_sig)
    {
      free(last_sig);
      last_sig = NULL;
    }
}

static void
set_lang_buf(const char *sig, char *buf)
{
  const char *s = sig;
  char *lang_end = NULL;

  if ((s = strchr(sig, '%')))
    {
      ++s;
      if ((lang_end = strchr(s, ':')))
	{
	  if (lang_end - s >= (_MAX_LANG-1))
	    {
	      fprintf(stderr, "lang %s too long in bigrams (max=%d)", s, _MAX_LANG-1);
	      *lang_end = '\0';
	    }
	  else
	    {
	      strncpy(buf, s, lang_end - s);
	      buf[lang_end-s] = '\0';
	    }
	}
      else
	{
	  fprintf(stderr, "malformed lang in bigrams\n");
	  buf[0] = '\0';
	}
      /* fprintf(stderr, "%s\n", buf); */
    }
  else
    buf[0] = '\0';
}

static void
set_last_sig(const char *s)
{
  clear_last_sig();
  if (s)
    {
      last_sig = malloc(strlen(s)+1);
      strcpy(last_sig, s);
      strcpy(last_sig_lang, curr_sig_lang);
      if (verbose)
	fprintf(stderr, "setting last_sig=%s; last_sig_lang=%s\n", s, last_sig_lang);
    }
}

static int
use_this_form(const char **atts)
{
  char *square = NULL, *inst = NULL;;
  tmp_sig = NULL;
  while (*atts)
    {
      if (!strcmp(atts[0],"sig"))
	tmp_sig = (char*)atts[1];
      else if (!strcmp(atts[0],"exosig"))
	tmp_sig = (char*)atts[1];
      else if (!strcmp(atts[0],"newsig"))
	tmp_sig = (char*)atts[1];
      else if (!strcmp(atts[0],"inst"))
	inst = (char*)atts[1];
      atts += 2;
    }
  
  if (!tmp_sig || *tmp_sig == '{')
    {
      if (inst && !strcmp(inst,"n"))
	{
	  tmp_sig = inst;
	  return 1;
	}
      else
	return 0;
    }

  square = strchr(tmp_sig,']');
  if (!square)
    return 0;

  ++square;
  return isalpha(square[1]) 
    || ('u' != square[0] && 'X' != square[0] && 'L' != square[0]);
}

int *
new_int(void)
{
  return mb_new(mb_ints);
}

static void
shift_sig(char *tmp_sig)
{
  char *norm_end = NULL;

  set_lang_buf(tmp_sig, curr_sig_lang);

  if (!(curr_sig = strchr(tmp_sig, '=')))
    curr_sig = tmp_sig;
  else
    ++curr_sig;

  if ((norm_end = strchr(curr_sig, '$')))
    {
      size_t n = strcspn((char*)norm_end+1,F2_FIELD_ENDS_STR); /*'*/
      norm_end += n + 1;
      *norm_end = '\0';
    }

  if (last_sig && !strcmp(last_sig_lang, curr_sig_lang))
    {
      Hash_table *last_sig_hash;
      if (!(last_sig_hash = hash_find(bigrams,ucc(last_sig))))
	{
	  int *intp = new_int();
	  last_sig_hash = hash_create(1);
	  hash_add(bigrams,ucc(xpool_copy(last_sig)),last_sig_hash);
	  hash_add(last_sig_hash,ucc(xpool_copy(curr_sig)),intp);
	  if (verbose)
	    {
	      fprintf(stderr,"created last_sig_hash for head %s\n",last_sig);
	      fprintf(stderr,"registering bigram %s %s\n",last_sig, curr_sig);
	    }
	  ++total_bigrams;
	}
      else
	{
	  int *intp;
	  if (!(intp = hash_find(last_sig_hash,ucc(curr_sig))))
	    {
	      intp = new_int();
	      hash_add(last_sig_hash,ucc(xpool_copy(curr_sig)),intp);
	      
	      if (verbose)
		{
		  fprintf(stderr,"using last_sig_hash for head %s\n",last_sig);
		  fprintf(stderr,"registering bigram %s %s\n",last_sig, curr_sig);
		}
	      ++total_bigrams;
	    }
	  else
	    ++(*intp);
	}
      set_last_sig(curr_sig);
    }
  else if (*curr_sig_lang)
    {
      set_last_sig(curr_sig);
      strcpy(last_sig_lang, curr_sig_lang);
      if (!(bigrams = hash_find(bigram_lang_hashes, last_sig_lang)))
	{
	  bigrams = hash_create(1024);
	  hash_add(bigram_lang_hashes, xpool_copy(last_sig_lang), bigrams);
	}
    }
}

static void
shift_cof(char *tmp_sig)
{
  char *amps = strstr(tmp_sig,"&&");
  while (amps)
    {
      *amps = '\0';
      shift_sig(tmp_sig);
      tmp_sig = amps+1;
      amps = strstr(tmp_sig,"&&");
    }
  shift_sig(tmp_sig);
}

static void
startHandler(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|l"))
    {
      if (use_this_form(atts))
	{
	  if (verbose)
	    fprintf(stderr, "using sig %s\n", tmp_sig);
	  if (strstr(tmp_sig, "&&"))
	    shift_cof(tmp_sig);
	  else
	    shift_sig(tmp_sig);
	}
      else
	{
	  if (verbose)
	    fprintf(stderr, "ignoring sig %s\n", tmp_sig);
	  clear_last_sig();
	}
    }
  else if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|c"))
    {
      const char *type = findAttr(atts, "type");
      if (!strcmp(type, "sentence"))
	list_push(c_list, "1");
      else
	list_push(c_list, "0");
    }
}

/* Not clearing last_sig except at unit boundaries
   means that the bigrams are fuzzy about phrase 
   boundaries and discontinuities, but we
   probably do want that */
static void
endHandler(void *userData, const char *name)
{
  if (!strcmp(name,"http://oracc.org/ns/xcl/1.0|c"))
    {
      char *c = list_pop(c_list);
      if (c && '1' == *c)
	clear_last_sig();
    }
}

static void
fn_expand(void *p)
{
  fnlist[findex++] = strdup(l2_expand(iproject, p, ext));
}

static void
marshall_bigrams_2(unsigned char *key, int *data)
{
  bgms[nbgms].bgm = malloc(strlen(head_bigram) + strlen(key) + 2);
  sprintf(bgms[nbgms].bgm, "%s %s", head_bigram, key);
  bgms[nbgms].freq = *data;
  ++nbgms;
}

static void
marshall_bigrams(unsigned char *key, Hash_table*data)
{
  head_bigram = key;
  hash_exec2(data, (hash_exec2_func*)marshall_bigrams_2);
}

int
bgm_tab_cmp(struct bgm_tab *a, struct bgm_tab *b)
{
  return b->freq - a->freq;
}

static void
dump_bigrams(char *lang, Hash_table *bhash)
{
  static char obuf[1024];
  FILE *ofp = NULL;
  int i;

  sprintf(obuf,"02pub/bgrm-%s.ngm",lang);
  if (!(ofp = xfopen(obuf, "w")))
    {
      fprintf(stderr, "bigrams: can't write bigrams to %s\n", obuf);
      return;
    }

  /* sort bigrams by frequency */
  nbgms = 0;
  hash_exec2(bhash, (hash_exec2_func*)marshall_bigrams);
  qsort(bgms,nbgms,sizeof(struct bgm_tab),
	(int(*)(const void*,const void*))bgm_tab_cmp);

  /* dump bigrams */
  for (i = 0; i < nbgms; ++i)
    fprintf(ofp,"%s\n",bgms[i].bgm);

  xfclose(obuf,ofp);
}

int
main(int argc, char * const*argv)
{
  List *files;
  char *fn;
  int i;
  FILE *ofp = NULL;

  options(argc,argv,"f:i:o:p:sv");

  if (!project)
    {
      fprintf(stderr, "bigrams: must use -p PROJECT option to name project. Stop.\n");
      exit(1);
    }

  *last_sig_lang = *curr_sig_lang = '\0';

  if (!iproject)
    iproject = project;

  c_list = list_create(LIST_DOUBLE);

  if (!from_stdin)
    {
      files = list_create(LIST_SINGLE);
      if (inputfile)
	{
	  list_add(files,strdup(inputfile));
	}
      else
	{
	  while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
	    {
	      fn[strlen(fn)-1] = '\0';
	      list_add(files,strdup(fn));
	    }
	}
      fnlist = xmalloc((1+files->count) * sizeof(const char *));
      findex = 0;
    }

  pool = npool_init();
  bigram_lang_hashes = hash_create(10);
  mb_ints = mb_init(sizeof(int), 32);

  if (from_stdin)
    runexpatNS(i_stdin, NULL, startHandler, endHandler,"|");
  else
    {
      list_exec(files,fn_expand);
      fnlist[findex] = NULL;
      runexpatNS(i_list, fnlist, startHandler, endHandler,"|");
    }

  list_free(c_list, NULL);

  bgms = calloc(total_bigrams, sizeof(struct bgm_tab));
  hash_exec2(bigram_lang_hashes, (hash_exec2_func*)dump_bigrams);
  free(bgms);

  /* Clean up */
  if (!from_stdin)
    {
      list_free(files,list_xfree);
      for (i = 0; fnlist[i]; ++i)
	free(fnlist[i]);
      free(fnlist);
      fnlist = NULL;
    }
  npool_term(pool);
  hash_free(bigrams,NULL);
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'f':
      inputfile = arg;
      break;
    case 'i':
      iproject = arg;
      break;
    case 'o':
      outputbase = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 's':
      from_stdin = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "bigrams";
int major_version = 1, minor_version = 0;
const char *usage_string = "-p [project]";
void
help ()
{
  printf("  -f [file]\n    Gives the name of the XTF file to process\n");
  printf("  -i [inputs project]\n    Gives the name of the project to harvest\n");
  printf("  -o [output base]\n    Gives the name of outputs; NO EXTENSION\n");
  printf("  -p [project]\n    Gives the name of the project to harvest for; REQUIRED\n");
  printf("  -s\n    Read XML input from <stdin> (not Pnums)\n");
}

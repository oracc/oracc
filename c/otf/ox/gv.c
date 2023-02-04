#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include <graphemes.h>
#include <gdl.h>
#include <lang.h>
#include <tree.h>
#include <sllib.h>
#include <gvl.h>
#include <oracclocale.h>

static char *project = NULL, *name = NULL;
static unsigned char *key;
static int human_readable = 0;
static int utf8 = 0;
static int uhex = 0;

const char *errmsg_fn = NULL;
extern const char *file;

static int tsv = 1;

int sparse_lem = 0;
int trans_inline = 0;
int xcl_is_sparse_field = 0;

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

extern FILE *f_log;

unsigned char *
scan_comment_sub(unsigned char **lines, int *nlinesp, int badcolon)
{
  lines = NULL;
  nlinesp = NULL;
  badcolon = 0;
  return NULL;
}

static void
run_gv(unsigned char *key)
{
  gvl_g *gv = NULL;
  gv = gvl_validate((uccp)key);
  if (gv)
    {
      if (gv->mess)
	fprintf(stderr, "%s\n", gv->mess);
      else
	fprintf(stderr, "gv: g=%s; oid=%s; sn=%s\n", key, gv->oid, gv->sign);
    }
  else
    fprintf(stderr, "gvl_validate failed on %s\n", key);
}

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,ORACC_LOCALE);
  f_log = stderr;

  tree_init();
  gdl_init();
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);

  options(argc, argv, "hk:p:n:u8");

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";
  if (!name)
    name = "ogsl";

  gvl_setup(project, name, tsv);
  if (key)
    run_gv((uccp)key);
  else
    {
      char keybuf[256];
      while ((key = (ucp)fgets(keybuf, 256, stdin)))
	{
	  key[strlen((ccp)key)-1] = '\0';
	  if (*key == 0x04) {
	    goto quit;
	  } else {
	    run_gv(key);
	  }
	}
    }
  gvl_wrapup(name);
  
 quit:
  return 0;
}

const char *prog = "gv";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "PROJECT SLDB_NAME KEY";
int opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'h':
      human_readable = 1;
      break;
    case 'k':
      key = (ucp)arg;
      break;
    case 'n':
      name = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'u':
      uhex = 1;
      break;
    case '8':
      utf8 = 1;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}

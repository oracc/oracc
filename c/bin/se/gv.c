#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include <sllib.h>
#include <gvl.h>
#include <oracclocale.h>

static char *project = NULL, *name = NULL;
static char *key;
static int human_readable = 0;
static int utf8 = 0;
static int uhex = 0;

const char *errmsg_fn = NULL;
extern const char *file;

static int tsv = 1;

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,ORACC_LOCALE);
  
  options(argc, argv, "hk:p:n:u8");

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";
  if (!name)
    name = "ogsl";

  gvl_setup(project, name, tsv);
  if (key)
    gvl_validate(key);
  else
    {
      char keybuf[256];
      while ((key = fgets(keybuf, 256, stdin)))
	{
	  key[strlen(key)-1] = '\0';
	  if (*key == 0x04) {
	    goto quit;
	  } else {
	    gvl_validate(key);
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
      key = arg;
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

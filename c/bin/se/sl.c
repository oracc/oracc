#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <ctype128.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include <oracclocale.h>
#include <gvl.h>
#include <gdl.h>
#include <lang.h>
#include <tree.h>

extern FILE *f_log;
const char *project;
static char *db = NULL, *name = NULL;
static char *key;
static int human_readable = 0;
static const char *oracc = NULL;
static int oid = 0;
static int utf8 = 0;
static int uhex = 0;
static int gv_mode = 0;

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

static void
gv(unsigned const char *key)
{
  unsigned char tmp[32];
  gvl_g *gvp = NULL;
  if ('o' == key[0] && isdigit(key[1]) && !strchr((ccp)key, ';'))
    {
      unsigned const char *sn = gvl_lookup(key);
      if (sn)
	{
	  if (strlen((ccp)key) < 26) {
	    sprintf((char*)tmp, "%s;name", key);
	    key = tmp;
	  } else {
	    fputc('\n',stdout);
	    goto RET;
	  }
	}
    }
  if (strchr((ccp)key, ';'))
    {
      unsigned const char *res = gvl_lookup(key);
      if (res)
	fprintf(stdout, "%s\n", res);
      else
	fputc('\n',stdout);
    }
  else
    {
      gvp = gvl_validate((uccp)key);
      if (gvp && gvp->sign)
	{
	  if (human_readable)
	    fprintf(stdout, "%s\n", (char*)gvp->sign);
	  else if (utf8)
	    {
	      if (gvl_cuneify_gv(gvp))
		fprintf(stdout, "%s\n", (char*)gvp->utf8);
	      else
		fputc('\n',stdout);
	    }
	  else if (uhex)
	    {
	      if (gvl_ucode(gvp))
		fprintf(stdout, "%s\n", (char*)gvp->uhex);
	      else
		fputc('\n',stdout);
	    }
	  else
	    fprintf(stdout, "%s\n", gvp->oid);
	}
      else
	fputc('\n',stdout);
    }
 RET:
  fflush(stdout);
}

static void
sl(Dbi_index *dbi, char *key)
{
  char *v = NULL;
  char *k2 = NULL;
  char tmp[32];

  if ('|' == *key && !strchr(key, ';'))
    {
      k2 = malloc(strlen(key));
      strcpy(k2, key+1);
      k2[strlen(k2)-1] = '\0';
      key = k2;
    }
  else if (key[0] == 'o' && isdigit(key[1]) && !strchr(key, ';'))
    {
      if (strlen(key) < 26) {
	sprintf(tmp, "%s;name", key);
	key = tmp;
	/*fprintf(stderr, "key = %s\n", key);*/
      } else {
	fputc('\n',stdout);
	goto RET;
      }
    }
  
  dbi_find(dbi,(unsigned char *)key);
  if (dbi->data)
    {
      v = dbi->data;
      if (human_readable)
	{
	  unsigned char tmp[128];	  
	  sprintf((char*)tmp, "%s;name", v);
	  dbi_find(dbi, tmp);
	  fprintf(stdout, "%s\n", (char*)dbi->data);
	}
      else if (utf8)
	{
	  unsigned char tmp[128];
	  sprintf((char*)tmp, "%s;uchar", v);
	  dbi_find(dbi, tmp);
	  if (dbi->data)
	    fprintf(stdout, "%s\n", (char*)dbi->data);
	  else
	    fputc('\n',stdout);
	}
      else if (uhex)
	{
	  unsigned char tmp[128];
	  sprintf((char*)tmp, "%s;ucode", v);
	  dbi_find(dbi, tmp);
	  if (dbi->data)
	    fprintf(stdout, "%s\n", (char*)dbi->data);
	  else
	    fputc('\n',stdout);
	}
      else
	fprintf(stdout, "%s\n", v);
    }
  else
    fputc('\n',stdout);
 RET:
  fflush(stdout);
  if (k2)
    free(k2);
}

int
main(int argc, char **argv)
{
  Dbi_index *dbi = NULL;

  setlocale(LC_ALL,ORACC_LOCALE);
  f_log = stderr;
  
  options(argc, argv, "ghk:p:n:ou8");

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";
  if (!name)
    name = "ogsl";

  if (gv_mode)
    {
      /* Figure out the db and open it */
      if (!project)
	project = "ogsl";
      if (!name)
	name = "ogsl";

      tree_init();
      gdl_init();
      curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
 
      gvl_setup(project, name, 1);
      if (key)
	gv((uccp)key);
      else
	{
	  char keybuf[256];
	  while ((key = fgets(keybuf, 256, stdin)))
	    {
	      key[strlen((ccp)key)-1] = '\0';
	      if (*key == 0x04) {
		break;
	      } else {
		gv((uccp)key);
	      }
	    }
	}
      gvl_wrapup(name);
    }
  else
    {
      oracc = oracc_home();
      db = malloc(strlen(oracc)+strlen("/pub/sl/") + strlen(project) + 1);
      sprintf(db, "%s/pub/%s/sl", oracc, project);
      if ((dbi = dbi_open(name, db)))
	{
	  /* do the look up or sit or enter stdin_mode */
	  if (key)
	    sl(dbi, key);
	  else
	    {
	      char keybuf[256];
	      while ((key = fgets(keybuf, 256, stdin)))
		{
		  key[strlen(key)-1] = '\0';
		  if (*key == 0x04) {
		    break;
		  } else {
		    sl(dbi, key);
		  }
		}
	    }
	  dbi_close(dbi);
	}
      else
	{
	  fprintf(stderr, "sl: failed to open %s/%s\n", (char *)project, (char*)db);
	  exit(1);
	}
    }
  return 0;
}

const char *prog = "sl";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = "PROJECT SLDB_NAME KEY";
int opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'g':
      gv_mode = 1;
      break;
    case 'h':
      human_readable = 1;
      break;
    case 'k':
      key = arg;
      break;
    case 'n':
      name = arg;
      break;
    case 'o':
      oid = 1;
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

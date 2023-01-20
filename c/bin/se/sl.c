#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>
#include <oracclocale.h>

static char *db = NULL, *project = NULL, *name = NULL;
static char *key;
static int human_readable = 0;
static const char *oracc = NULL;
static int utf8 = 0;
static int uhex = 0;

void
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
  
  options(argc, argv, "hk:p:n:u8");

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";
  if (!name)
    name = "ogsl";

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
		goto quit;
	      } else {
		sl(dbi, key);
	      }
	    }
	}
    }
  else
    {
      fprintf(stderr, "sl: failed to open %s/%s\n", (char *)project, (char*)db);
      exit(1);
    }
 quit:
  dbi_close(dbi);
  return 0;
}

const char *prog = "sl";
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

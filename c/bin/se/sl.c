#include <stdio.h>
#include <stdlib.h>
#include <psd_base.h>
#include <fname.h>
#include <options.h>
#include <dbi.h>

static char *db = NULL, *project = NULL;
static char *key;
static int human_readable = 0;
static const char *oracc = NULL;

void
sl(Dbi_index *dbi, char *key)
{
  char *v = NULL;
  char *k2 = NULL;

  if ('|' == *key)
    {
      k2 = malloc(strlen(key));
      strcpy(k2, key+1);
      k2[strlen(k2)-1] = '\0';
      key = k2;
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
      else
	fprintf(stdout, "%s\n", v);
    }
  else
    fputc('\n',stdout);
  fflush(stdout);
  if (k2)
    free(k2);
}

int
main(int argc, char **argv)
{
  Dbi_index *dbi = NULL;

  options(argc, argv, "hk:p:");

  /* Figure out the db and open it */
  if (!project)
    project = "ogsl";

  oracc = oracc_home();
  db = malloc(strlen(oracc)+strlen("/pub/") + strlen(project) + 1);
  sprintf(db, "%s/pub/%s", oracc, project);
  if ((dbi = dbi_open(project, db)))
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
    case 'p':
      project = arg;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}

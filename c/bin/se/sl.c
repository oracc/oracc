#include <stdio.h>
#include <stdlib.h>
#include <psdtypes.h>
#include <dbi.h>

static const char *arg_db, *arg_project;
static char *arg_key;
static int human_readable = 0;

void
sl(const char *project, const char *index, char *key)
{
  Dbi_index *dbi = NULL;
  char *v = NULL;

  if ((dbi = dbi_open(project,index)))
    {
      dbi_find(dbi,(unsigned char *)key);
      if (dbi->data)
	{
	  v = dbi->data;
	  if (human_readable)
	    {
	      char tmp[128];
	      sprintf(tmp, "%s;name", v);
	      dbi_find(dbi, tmp);
	      fputs(dbi->data,stdout);
	    }
	  else
	    fputs(v,stdout);
	}
      dbi_close(dbi);
    }
  else
    {
      fprintf(stderr, "sl: failed to open %s/%s\n", (char *)project, (char*)index);
      exit(1);
    }
}

int
main(int argc, char **argv)
{
  options(argc, argv, "d:hk:p:");
  if (arg_project && arg_db && arg_key)
    sl(arg_project, arg_db, arg_key);
  else
    {
      fprintf(stderr, "sl: must give PROJECT SLDB KEY on command line\n");
      exit(1);
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
    case 'd':
      arg_db = arg;
      break;
    case 'h':
      human_readable = 1;
      break;
    case 'k':
      arg_key = arg;
      break;
    case 'p':
      arg_project = arg;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}

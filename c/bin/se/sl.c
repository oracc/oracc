#include <stdio.h>
#include <stdlib.h>
#include <psdtypes.h>
#include <dbi.h>

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
  if (argc == 4)
    sl(argv[1], argv[2], argv[3]);
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
int opts(int argc, char *arg){return 0;}
void help (void){}

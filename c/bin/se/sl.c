#include <stdio.h>
#include <stdlib.h>
#include <psdtypes.h>
#include <dbi.h>

void
sl(const char *project, const char *index, unsigned char *key)
{
  Dbi_index *dbi = NULL;
  unsigned char *v = NULL;
  dbi = dbi_open(project,index);
  dbi_find(dbi,key);
  if (dbi->data)
    {
      v = dbi->data;
      fputs(v,stdout);
    }
  dbi_close(dbi);
}

int
main(int argc, char **argv)
{
  sl(argv[1], argv[2], argv[3]);
  return 0;
}

const char *prog = "sl";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = " SLDB_NAME KEY [EXT]";
int opts(int argc, char *arg){return 0;}
void help (void){}

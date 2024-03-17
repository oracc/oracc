#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "loadfile.h"
#include "vector.h"

extern char *return_index;

const char **
xmldir_toks(const char *xmldir)
{
  char *fname = malloc(strlen(xmldir)+16);
  static const char *txt[2];
  sprintf(fname,"%s/search.txt",xmldir);
  txt[0] = (char*)loadfile((unsigned char *)fname, NULL);
  txt[1] = NULL;
  free(fname);
  return vec_from_str(txt[0], NULL, NULL);
}

void
xmldir_results(const char *xmldir, int count)
{
  char *fname = malloc(strlen(xmldir)+16), *dirbase;
  FILE *ret;
  extern FILE *out_f;
  if (!(dirbase = strrchr(xmldir,'/')))
    dirbase = xmldir;
  else
    ++dirbase;
  sprintf(fname,"%s/results.xml",xmldir);
  ret = fopen(fname,"w");
  fprintf(ret,"<ret><index>%s</index><dir>%s</dir><count>%d</count></ret>",return_index,dirbase,count);
  fclose(ret);
  sprintf(fname,"%s/results.lst",xmldir);
  out_f = fopen(fname,"w");
}

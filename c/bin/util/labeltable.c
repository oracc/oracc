#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <pool.h>
#include "./warning.h"


extern char *strdup(const char *);
extern FILE *f_log;

static const char *current_PQ = NULL;
static FILE *tab = NULL;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (name[0] == 'l' && name[1] == '\0')
    {
      const char *xmlid = NULL;
      const char *label = NULL;
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    xmlid = atts[i+1];
	  else if (!strcmp(atts[i],"label"))
	    label = atts[i+1];
	  if (xmlid && label)
	    break;
	}
#if 0
      fprintf(stderr,"loading label %s:%s = %s\n", current_PQ, label, xmlid);
#endif
      if (xmlid && label) /* lgs doesn't have id/label */
	{
	  fprintf(tab,"%s\t%s\n",xmlid,label);
	}
    }
}

static void
eH(void *userData, const char *name)
{
}

int
main(int argc, char **argv)
{
  char PQ[512];
  tab = stdout;
  while (fgets(PQ,512,stdin))
    {
      const char *fname[2];
      PQ[strlen(PQ)-1] = '\0';
      current_PQ = PQ;
      fname[0] = l2_expand(NULL, PQ, "xtf");
      fname[1] = NULL;
      runexpat(i_list, fname, sH, eH);
    }
  return 1;
}

const char *prog = "labeltable";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "labeltable <XTF >TAB";
void help () { }
int opts(int arg,char*str){ return 1; }

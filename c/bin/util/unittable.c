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

static int doing = 0;

static char discourse_id[2048];
static char discourse_type[2048];
static char sentence_id[2048];
static char sentence_label[2048];

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"xcl"))
    doing = 1;
  else if (doing && name[0] == 'c' && name[1] == '\0')
    {
      const char *xmlid = NULL;
      const char *type = NULL;
      const char *subtype = NULL;
      const char *label = NULL;
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    xmlid = atts[i+1];
	  else if (!strcmp(atts[i],"label"))
	    label = atts[i+1];
	  else if (!strcmp(atts[i],"type"))
	    type = atts[i+1];
	  else if (!strcmp(atts[i],"subtype"))
	    subtype = atts[i+1];
	}
      if (type)
	{
	  if (!strcmp(type, "discourse"))
	    {
	      strcpy(discourse_id,xmlid);
	      strcpy(discourse_type,subtype);
	    }
	  else if (!strcmp(type, "sentence"))
	    {
	      strcpy(sentence_id, xmlid);
	      strcpy(sentence_label,label);
	    }
	}
    }
  else if (doing && name[0] == 'l' && name[1] == '\0')
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
          if (!strcmp(atts[i],"ref"))
	    {
               fprintf(tab,"%s\t%s\t%s\t%s\t%s\n",atts[i+1],discourse_id,discourse_type,sentence_id,sentence_label);
	       break;
            }
	}
    }
}

static void
eH(void *userData, const char *name)
{
  if (!strcmp(name,"xcl"))
    doing = 0;
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

#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <pool.h>
#include "./warning.h"

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static const char *current_PQ = NULL;
static FILE *tab = NULL;

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "g:w")) /* name[0] == 'w' && name[1] == '\0') */
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    fprintf(tab,"%s\t%s\t%d\n",atts[i+1],pi_file,pi_line);
	}
    }
  else if (name[0] == 'l' && !name[1])
    {
      int i, ref = -1, inst = -1, sig = -1;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"inst"))
	    inst = i+1;
	  else if (!strcmp(atts[i], "ref"))
	    ref = i+1;
	  else if (!strcmp(atts[i], "sig"))
	    sig = i+1;
	  else if (!strcmp(atts[i], "exosig"))
	    sig = i+1;
	  else if (!strcmp(atts[i], "newsig"))
	    sig = i+1;
	}
      if (inst > 0)
	fprintf(tab, "%s\t%s\t%s\n", atts[ref], atts[inst], (sig > 0) ? atts[sig] : "");
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
      char *dot;
      PQ[strlen(PQ)-1] = '\0';
      if ((dot = strchr(PQ,'.')))
	*dot = '\0';
      current_PQ = PQ;
      fname[0] = l2_expand(NULL, PQ, "xtf");
      fname[1] = NULL;
      runexpat(i_list, fname, sH, eH);
    }
  return 1;
}

const char *prog = "wid2err";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "labeltable <XTF >TAB";
void help () { }
int opts(int arg,char*str){ return 1; }

#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <pool.h>
#include "./warning.h"

/* Emulate ox -G output but do it from the XTF not from the ATF */

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static FILE *tab = NULL;

int printing = 0;
int xcl = 0;

static char pqx[128];
static char tlabel[128];
static char lid[128];
static char llabel[128];
static char project[1024];

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "transliteration") || !strcmp(name, "composite"))
    {
      strcpy(pqx, get_xml_id(atts));
      strcpy(tlabel, findAttr(atts, "n"));
      strcpy(project, findAttr(atts, "project"));
      printing = 1;
      xcl = 0;
    }
  else if (!xcl && !strcmp(name, "l"))
    {
      strcpy(lid, get_xml_id(atts));
      strcpy(llabel, findAttr(atts, "label"));
      fprintf(tab,"%s/%s\t%d\t%s\t%s\t%s\t%s\t",project,pi_file,pi_line,pqx,tlabel,lid,llabel);
      printing = 1;
    }
  else if (printing && (!strcmp(name, "g:c") || !strcmp(name, "g:q")))
    {
      fprintf(tab, "%s ", findAttr(atts, "form"));
      --printing;
    }
  else if (!strcmp(name, "xcl"))
    {
      printing = 0;
      xcl = 1;
    }  
}

static void
eH(void *userData, const char *name)
{
  if (printing && !strcmp(name, "l"))
    {
      fprintf(tab, "\n");
    }
  else if (printing && (!strcmp(name, "g:v") || !strcmp(name, "g:s")))
    {
      fprintf(tab, "%s ", (char*)charData_retrieve());
    }
  else if (!strcmp(name, "g:c") || !strcmp(name, "g:q"))
    ++printing;
  else
    charData_discard();
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

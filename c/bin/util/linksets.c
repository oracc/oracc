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

static int printing_ls = 0;
static int in_xcl = 0;

static void
sH(void *userData, const char *name, const char **atts)
{
  const char *title = NULL, *role = NULL, *xmlid = NULL;
  if (!strcmp(name, "linkset")) /* name[0] == 'w' && name[1] == '\0') */
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i], "xl:title"))
	    title = atts[i+1];
	  else if (!strcmp(atts[i], "xl:role"))
	    role = atts[i+1];
	  else if (!strcmp(atts[i], "xml:id"))
	    xmlid = atts[i+1];
	}
      if (title)
	{
	  printf("%s\t%s\t%s\t", role, title, xmlid);
	  printing_ls = 1;
	}
    }
  else if (!strcmp(name,"link") && printing_ls)
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i], "xl:href"))
	    printf("%s ", atts[i+1]);
	}
    }
  else if (!strcmp(name,"xcl"))
    {
      fprintf(stderr, "in xcl\n");
      in_xcl = 1;
    }
  else if (!strcmp(name,"l") && in_xcl)
    {
      const char *l_id = NULL, *w_id = NULL;
      int i;
      fprintf(stderr, "found xcl:l\n");
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  fprintf(stderr,"xcl:l @ %s\n", atts[i]);
	  if (!strcmp(name, "xml:id"))
	    l_id = atts[i+1];
	  else if (!strcmp(name, "ref"))
	    w_id = atts[i+1];
	}
      if (l_id && w_id)
	{
	  fprintf(stderr, "%s => %s\n", l_id, w_id);
	  printf("%s\t%s\n",l_id,w_id);
	}
    }
  else if (!strcmp(name, "g:w")) /* name[0] == 'w' && name[1] == '\0') */
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    printf("%s\t%s\t%d\n",atts[i+1],pi_file,pi_line);
	}
    }

}

static void
eH(void *userData, const char *name)
{
  if (!strcmp(name, "linkset") && printing_ls)
    {
      printing_ls = 0;
      printf("\n");
    }
  else if (!strcmp(name,"xcl"))
    {
      fprintf(stderr, "out xcl\n");
      in_xcl = 0;
    }
}

int
main(int argc, char **argv)
{
  char PQ[512];
  const char *fname[2];
  tab = stdout;
  printf("File format:\n2 fields = LEM_ID\tWORD_ID\n3 fields = WORD_ID\tFILE\tLINE\n4 fields = LS\tROLE\tTITLE\tWORD_IDS\n");
  if (argc > 1)
    {
      if (!strcmp(argv[1], "-s"))
	{
	  runexpat(i_stdin, NULL, sH, eH);
	}
      else
	{
	  fname[0] = argv[1];
	  fname[1] = NULL;
	  runexpat(i_list, fname, sH, eH);
	}
    }
  else
    {
      while (fgets(PQ,512,stdin))
	{
	  char *dot;
	  PQ[strlen(PQ)-1] = '\0';
	  if ((dot = strchr(PQ,'.')))
	    *dot = '\0';
	  current_PQ = PQ;
	  fname[0] = l2_expand(NULL, PQ, "xtf");
	  fname[1] = NULL;
	  runexpat(i_list, fname, sH, eH);
	}
    }
  return 0;
}

const char *prog = "linkset";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "linkset <XTF >TAB";
void help () { }
int opts(int arg,char*str){ return 1; }

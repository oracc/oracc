#include <unistd.h>
#include <string.h>
#include <psd_base.h>
#include <runexpat.h>
#include <fname.h>
#include <hash.h>
#include <npool.h>
#include "./warning.h"

#undef strdup
extern char *strdup(const char *);
extern FILE *f_log;

static int long_output = 1;

static Hash_table *errh;
static struct npool *errp;
static const char *current_PQ = NULL;
static FILE *tab = NULL;

static void
sH(void *userData, const char *name, const char **atts)
{
  static char *saved_inst = NULL, *saved_ref = NULL;
  if (!strcmp(name, "g:w")) /* name[0] == 'w' && name[1] == '\0') */
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"xml:id"))
	    {
	      char errstr[512], *p, *id;
	      sprintf(errstr, "%s:%d", pi_file, pi_line);
	      id = (char *)npool_copy((unsigned char *)atts[i+1], errp);
	      p = (char*)npool_copy((unsigned char *)errstr, errp);
	      /*fprintf(stderr, "add %s => %s to errh\n", id, p);*/
	      hash_add(errh, (const unsigned char *)id, p);
	      /*fprintf(tab,"%s\t%s\t%d\n",atts[i+1],pi_file,pi_line);*/
	    }
	}
    }
  else if (name[0] == 'l' && !name[1])
    {
      int i, ref = -1, inst = -1, sig = -1, prj = -1;
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
	  else if (!strcmp(atts[i], "exoprj"))
	    prj = i+1;
	}
      if (inst > 0)
	{
	  if (sig > 0)
	    {
	      char *err = hash_find(errh,(const unsigned char *)atts[ref]);
	      /*fprintf(stderr,"find %s => %s in errh\n",atts[ref],err);*/
	      if (long_output)
		fprintf(tab, "%s\t%s\t%s\t%s\t%s\n", err, atts[ref], atts[inst], (sig > 0) ? atts[sig] : "", (prj > 0) ? atts[prj] : "");
	      else if (atts[sig])
		fprintf(tab, "%s\t%s\n", atts[sig], atts[ref]);
	    }
	  else if (ref > 0 && inst > 0)
	    {
	      saved_ref = strdup(atts[ref]);
	      saved_inst = strdup(atts[inst]);
	    }
	}
    }
  else if (saved_inst && !strcmp(name, "xff:f"))
    {
      int i;
      for (i = 0; atts[i] != NULL; i+=2)
	{
	  if (!strcmp(atts[i],"form"))
	    break;
	}
#if 0
      fprintf(tab, "%s\t%s=%s\tBAD\n", saved_ref, atts[i+1], saved_inst);
#endif
      free(saved_inst);
      free(saved_ref);
      saved_inst = saved_ref = NULL;
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
  const char *fname[2];
  tab = stdout;

  if (argc > 1)
    {
      if (!strcmp(argv[1], "-s"))
	{
	  errp = npool_init();
	  errh = hash_create(1024);
	  runexpat(i_stdin, NULL, sH, eH);
	  hash_free(errh,NULL);
	  npool_term(errp);
	}
      else
	{
	  fname[0] = argv[1];
	  fname[1] = NULL;
	  errp = npool_init();
	  errh = hash_create(1024);
	  runexpat(i_list, fname, sH, eH);
	  hash_free(errh,NULL);
	  npool_term(errp);
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
	  errh = hash_create(1024);
	  errp = npool_init();
	  runexpat(i_list, fname, sH, eH);
	  hash_free(errh,NULL);
	  npool_term(errp);
	}
    }
  return 0;
}

const char *prog = "xtfsigs";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "xtfsigs <XTF >TAB";
void help () { }
int opts(int arg,char*str){ return 1; }

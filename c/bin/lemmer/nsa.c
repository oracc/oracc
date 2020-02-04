#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "psd_base.h"
#include "options.h"
#include "nsa.h"
#include "list.h"

extern const char *file,*project;

/* symbols that are really external but aren't provided by the linked libraries */
int cbd_lem_sigs = 0, fuzzy_aliasing = 0, lem_dynalem = 0, pretty = 0;

int bootstrap_mode = 0;
int do_lem = 0;
int ignore_plus = 0;
int lem_autolem = 1;
int lem_standalone = 0;
int lem_system = 1;
int atf = 0;
int v2 = 0;
extern int checkpoints;
extern int label;
extern int printres;
int shadow_lem;
int verbose = 0;
int use_unicode = 0;
extern const char *input_file;
static struct nsa_context *context;

const char *phase;
FILE *f_log;

static void
my_nsa_xml_input(struct nsa_context *cp, const char *infile)
{
  struct xcl_context *xcp;
  struct nsa_result *res;
  extern const char *textid;
  FILE *xfp;
  xcp = xcl_load(infile, 0);
  textid = xcp->textid;
  if (!xcp->linkbase)
    xcp->linkbase = new_linkbase();
  nsa_xcl_input(xcp, cp);
  res = (struct nsa_result *)xcp->user;
  if (res->success)
    {
      char *outname = malloc(strlen(infile+1));
      strcpy(outname,infile);
      outname[strlen(outname)-3] = 'n';
      outname[strlen(outname)-2] = 's';
      outname[strlen(outname)-1] = 'a';
      res->label = xcp->textid;

#if 0
      if (printres)
	nsa_print(res, stdout);
#endif
      
      xfp = fopen(outname,"w");
      x2_serialize(xcp,xfp,0);
      fclose(xfp);
    }
  else
    fprintf(stderr,"nsa: parse failed in %s\n",infile);
  nsa_del_result(res);
  return;
}

int
main(int argc, char*const*argv)
{
#if 0
  long long int ll = 0x7fffffffffffffffLL; /* 9223372036854775807 */
  fprintf(stderr,"int = %lu; long = %lu; long long = %lu\n",
	  sizeof(int),sizeof(long),sizeof(long long));
  fprintf(stderr,"ll = %lld\n",ll);
#endif
  options(argc,argv,"aci:lpvx");
  context = nsa_init("nm.xml");
  f_log = stderr;
  if (input_file)
    {
      if (strstr(input_file,".xtf") || strstr(input_file,".xcl"))
	{
	  my_nsa_xml_input(context, input_file);
	}
      else if (strstr(input_file,".txt") || strstr(input_file,".atf")
	        || strstr(input_file,".in"))
	{
	  FILE *in = fopen(input_file,"r");
	  if (!in)
	    {
	      fprintf(stderr,"nsa: can't open %s for input\n",input_file);
	      exit(1);
	    }
	  lnum = 0;
	  nsa_text_input(context, in);
	  fclose(in);
	}
    }
  else
    {
      nsa_text_input(context, stdin);
    }
  nsa_term();
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'a':
      atf = 1;
      label = 1;
      break;
    case 'c':
      checkpoints = 1;
      break;
    case 'i':
      input_file = arg;
      break;
    case 'l':
      label = 1;
      break;
    case 'p':
      printres = 1;
      break;
    case 'v':
      ++verbose;
      break;
    case 'x':
      xlink_results = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "nsa";
int major_version = 0;
int minor_version = 1;
const char *usage_string = "nsa [-v[v]] [-aclpx] [-i [FILE]]";
void help() {}

#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include "xpd2.h"
#include "lemmer.h"
#include "ilem.h"
#include "sigs.h"
#include "xcl.h"

extern void dsa_exec(struct xcl_context *xc);
extern void psa_exec(struct xcl_context *xc);

int lem_props_strict;
const char *config_file;
const char *out_file;
const char *project;
const char *sig_file;
const char *textid;
const char *xcl_file;
/*const char *xcl_project;*/

FILE*f_log;
FILE*outfp = NULL;

int bootstrap_mode = 0;
int cbd_lem_sigs = 0;
int do_DSA = 0, do_NSA = 0, do_PSA = 0;
int fuzzy_aliasing = 0;
int inplace = 0;
int lem_autolem = 0;
int lem_dynalem = 0;
int new_lem = 0;
int pretty = 0;
int psu = 0;
int sig_dump = 0;
int textmode = 0;
int verbose = 0;

extern int ng_debug;

int
main(int argc, char **argv)
{
  int optch = -1;
  struct sig_context *scp = NULL;
  struct xcl_context *xcp = NULL;
  const char *sigs_opt = NULL;

  f_log = stderr;
  lem_do_wrapup = 1;

  if (verbose)
    ng_debug = 1;

  scp = sig_context_init();

  while (-1 != (optch = getopt(argc, argv, "c:DdiNno::Pps:tvx:")))
    {
      switch (optch)
	{
	case 'c':
	  config_file = optarg;
	  break;
	case 'D':
	  do_DSA = 1;
	  break;
	case 'd':
	  sig_dump = 1;
	  break;
	case 'i':
	  inplace = 1;
	  break;
	case 'N':
	  do_NSA = 1;
	  break;
	case 'n':
	  new_lem = 1;
	  break;
	case 'o':
	  if (optarg)
	    {
	      if ((outfp = fopen(optarg,"wb")))
		out_file = optarg;
	      else
		{
		  fprintf(stderr, "lemmer: can't write to `%s'\n", optarg);
		  exit(1);
		}
	    }
	  else
	    outfp = stdout;
	  break;
	case 'P':
	  do_PSA = 1;
	  break;
	case 'p':
	  psu = 1;
	  break;
	case 's':
	  sigs_opt = optarg;
	  break;
	case 't':
	  textmode = 1;
	  break;
	case 'x':
	  xcl_file = optarg;
	  break;
	case 'v':
	  ++verbose;
	  break;
	case 'w':
	  lem_do_wrapup = 0;
	  break;
	default:
	  break;
	}
    }

  nl_init();
  ngramify_init();

  if (textmode)
    {
      char buf[128];
      while (NULL != fgets(buf, 128, stdin))
	{
	  if ('\n' == buf[strlen(buf)-1])
	    {
	      
	    }
	  else
	    {
	      fprintf(stderr, "lemmer: buf overflow at %s\n", buf);
	      exit(1);
	    }
	}
    }
  else if (xcl_file)
    {
      xcp = xcl_load(xcl_file, XCL_LOAD_SANS_SIGSETS);
      xcp->sigs = scp;
      scp->xcp = xcp;
      textid = xcp->textid;
      scp->xpd = xpd_init(xcp->project,xcp->pool);

      if (sigs_opt)
	sig_context_register(scp,sigs_opt,NULL,0);
      else
	sig_context_langs(scp,xcp->langs);

      if (new_lem)
	sig_new(xcp);
      else
	sig_check(xcp);

      if (psu || do_DSA || do_NSA || do_PSA)
	{
	  xcp->linkbase = new_linkbase();
	  xcp->linkbase->textid = textid;
	  if (psu)
	    psus2(xcp);
	  else if (do_DSA)
	    dsa_exec(xcp);
	  else if (do_PSA)
	    psa_exec(xcp);
	  else if (do_NSA)
	    /*nsa_exec(xcp)*/;
	}

      if (lem_do_wrapup)
	xcl_map(xcp,NULL,NULL,NULL,ilem_wrapup);

      if (outfp)
	{
	  x2_serialize(xcp,outfp,1);
	  fclose(outfp);
	}
      xcl_destroy(&xcp);
    }

  sig_context_term();
  xcl_final_term();

  return 0;
}

/* This should be removed when -lpsd isn't used */
const char *prog="lemmer";
const char *usage_string = "";
void help(void) { }
int opts() { return 0; }
int major_version, minor_version;

/* Compile the sort info from sortinfo.tab into 
   an optimized form for runtime use */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psdtypes.h"
#include "messages.h"
#include "options.h"
#include "sortinfo.h"

int csi_debug = 0;
int verbose = 0;
const char *project;
struct sortinfo *sip;
extern unsigned char *loadfile(unsigned const char*,size_t*);

int
main(int argc, char **argv)
{
  unsigned char*tab = NULL;
  size_t tabsize;
  char *ext;

  options(argc,argv,"d");
  if (argv[optind])
    tab = loadfile((unsigned char*)argv[optind],&tabsize);
  else
    usage();
  sip = si_load_tab(tab,tabsize);
  if (NULL == sip)
    exit(1);
  ext = argv[optind] + strlen(argv[optind]) - 3;
  if (!strcmp(ext,"tab"))
    {
      strcpy(ext,"csi");
      si_dump_csi((unsigned char *)argv[optind],sip);
      if (csi_debug)
	{
	  sip = si_load_csi((const char*)argv[optind]);
	  fprintf(stderr,"%ld\n",(long)sip->nmember);
	}
      exit(0);
    }
  else
    {
      fprintf(stderr,"pgcsi: %s: require .tab extension\n", tab);
      exit(1);
    }
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'd':
      csi_debug = 1;
      break;
    case 'p':
      project = arg;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "pgcsi";
int major_version = 1, minor_version = 0;
const char *usage_string = "[file]";
void
help ()
{

}

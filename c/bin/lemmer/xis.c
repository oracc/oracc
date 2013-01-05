#include <psd_base.h>
#include "runexpat.h"
#include "options.h"

const char *file,*id,*outfile;
int want_this = 0;

static void
sH(void *userData, const char *name, const char **atts)
{
  /* Determine quickly if there an xml:id which matches target_id. */
  while (*atts)
    {
      if (**atts == 'x' && !strcmp(&atts[0][1],"ml:id") && !strcmp(atts[1],id))
	{
	  want_this = 1;
	  return;
	}
      atts += 2;
    }
}

static void
eH(void *userData, const char *name)
{
  static FILE *o = NULL;

  if (want_this && *name == 'r')
    {
      XML_Char *c = charData_retrieve();
      if (outfile)
	{
	  if (!o)
	    o = fopen(outfile,"w");
	  if (o)
	    {
	      fwrite(c,1,strlen(c),o);
	      fputc('\n',o);
	    }
	  else
	    fprintf(stderr,"xis: open failed on %s\n",outfile);
	}
      else
	{
	  fwrite(c,1,strlen(c),stdout);
	  fputc('\n',stdout);
	}
    }
  else if (want_this)
    {
      if (o)
	{
	  fclose(o);
	  o = NULL;
	}
      exit(1);
    }
  else
    charData_discard();
}

int
main(int argc, char * const*argv)
{
  char const *fnlist[2];
  options(argc,argv,"f:i:o:");

  if (!file || !id)
    usage();
  
  fnlist[0] = file;
  fnlist[1] = NULL;
  runexpat(i_list, fnlist, sH, eH);
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'f':
      file = arg;
      break;
    case 'i':
      id = arg;
      break;
    case 'o':
      outfile = arg;
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

const char *prog = "xis";
int major_version = 1, minor_version = 0, verbose = 0;
const char *usage_string = "-f [file] -i [id]";
void help () { }

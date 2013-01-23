#include <stdio.h>
#include <xpd2.h>
#include <npool.h>

int
main(int argc, char **argv)
{
  struct npool *pool = NULL;
  if ((pool = npool_init()))
    {
      struct xpd *x = NULL;
      const char *project = NULL;
      const char *option = NULL;
      const char *deflt = NULL;

      if (argc == 3 || argc == 4)
	{
	  project = argv[1];
	  option = argv[2];
	  if (argc == 4)
	    deflt = argv[3];
	  else
	    deflt = "";
	}
      else if (argc == 1)
	{
	  project = ".";
	  option = "project";
	}
      else
	{
	  void help(void);
	  help();
	  exit(1);
	}

      if ((x = xpd_init(project,pool)))
	{
	  const char *val = xpd_option(x,option);
	  if (!val && deflt)
	    val = deflt;
	  if (val)
	    {
	      fputs(val,stdout);
	      return 0;
	    }
	}
    }
  return 1;
}

int verbose;
const char *prog = "oraccopt";
int major_version = 1, minor_version = 0;
const char *usage_string = "[PROJ] [OPTION]";
void help()
{
  fprintf(stderr, 
	  "oraccopt takes 0, 2 or 3 arguments.\n\n"
	  "With 0 arguments it prints the name of the current project.\n\n"
	  "With 2 arguments, the first must be a known project (or a period, ., to mean the current project).\n"
	  "The second option must be a config option.  The value of the option in 02xml/config.xml\n"
	  "is printed and may be empty.\n"
	  "A third argument may be given which is the default value to print if\n"
	  "the config option is empty.\n");
}
int opts(int argc, char *arg) { return 0; }

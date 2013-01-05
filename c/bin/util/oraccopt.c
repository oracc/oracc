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

      if (argc == 3)
	{
	  project = argv[1];
	  option = argv[2];
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
	  const char *val = NULL;
	  if ((val = xpd_option(x,option)))
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
	  "oraccopt takes 0 or 2 arguments.  With 0 arguments it prints\n"
	  "the name of the current project.  With 2 arguments, the first\n"
	  "must be a known project, the second a config option.  The value\n"
	  "of the config option in the project is printed and may be empty\n");
}
int opts(int argc, char *arg) { return 0; }

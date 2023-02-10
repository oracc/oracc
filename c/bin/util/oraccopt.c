#include <stdio.h>
#include <xpd2.h>
#include <npool.h>

extern FILE *f_log;

static int looks_like_cfgname(char *arg)
{
  return !strncmp(arg,"@cfg=",5);
}

int
main(int argc, char **argv)
{
  struct npool *pool = NULL;
  extern const char *file, *textid;
  f_log = stderr;
  if ((pool = npool_init()))
    {
      struct xpd *x = NULL;
      const char *project = NULL;
      const char *option = NULL;
      const char *deflt = NULL;
      const char *cfgname = NULL;

      if (argc == 3 || argc == 4 || argc == 5)
	{
	  project = argv[1];
	  option = argv[2];
	  switch (argc)
	    {	      
	    case 5:
	      deflt = argv[3];
	      cfgname = argv[4];
	      break;
	    case 4:
	      if (looks_like_cfgname(argv[3]))
		{
		  cfgname = argv[3]+5;
		  deflt = "";
		}
	      else
		{
		  deflt = argv[3];		  
		}
	      break;
	    case 3:
	      deflt = "";
	      break;
	    }
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

      if (!file)
	file = "<none>";
      if (!textid)
	textid = "<none>";

      if (cfgname)
	xpd_set_configname(cfgname);
      
      if ((x = xpd_init(project,pool)))
	{
	  const char *val = xpd_option(x,option);
	  if (!val)
	    {
	      if (*option == '%')
		++option;
	      val = xpd_lang(x,option); /* look up %sux opts as sux */;
	    }
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

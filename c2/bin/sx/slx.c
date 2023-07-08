#include <stdio.h>
#include <stdlib.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include <sll.h>

const char *file = NULL;
int lnum = 0;
char *key = NULL;
const char *name = NULL;
const char *project = NULL;

int dbi_mode = 0;
int tsv_mode = 1;

int
main(int argc, char **argv)
{
  Dbi_index *d = NULL;
  Hash *h = NULL;

  program_values("slx", 1, 0, "[-p PROJECT] [key]", NULL);

  setlocale(LC_ALL,ORACC_LOCALE);
  
  options(argc, argv, "C:E:G:P:edghk:p:n:otu8");

  if (wcaller)
    {
      sll_web_handler(wcaller, wproject, wgrapheme, wextension);
      /* sll_web_handler should exit in normal circumstances */
      fprintf(stderr, "slx: strange--sll_web_handler did not exit\n");
      return 1;
    }
  else
    {
      if (dbi_mode)
	d = sll_init_d(project, name);
      else
	h = sll_init_t(project, name);
      
      if (key)
	sll_cli_handler((uccp)key);
      else
	{
	  char keybuf[256];
	  file = "<stdin>";
	  lnum = 0;
	  while ((key = fgets(keybuf, 256, stdin)))
	    {
	      key[strlen((ccp)key)-1] = '\0';
	      ++lnum;
	      if (*key)
		{
		  if (*key == 0x04) {
		    break;
		  } else {
		    sll_cli_handler((uccp)key);
		  }
		}
	    }
	}
    }
  if (h)
    sll_term_t(h);
  else
    sll_term_d(d);
  
  return 0;
}

extern int verbose;

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'C':
      wcaller = arg;
      break;
    case 'E':
      wextension = arg;
      break;
    case 'G':
      wgrapheme = arg;
      break;
    case 'P':
      wproject = arg;
      break;
    case 'k':
      key = arg;
      break;
    case 'n':
      name = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'r':
      sll_raw_output = 1;
      break;
    case 't':
      sll_trace = 1;
      break;
    default:
      usage();
      exit(1);
      break;
    }
  return 0;
}
void help (void){}

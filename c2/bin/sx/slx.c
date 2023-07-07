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
  
  options(argc, argv, "edghk:p:n:otu8");

  if (dbi_mode)
    d = sll_init_d(project, name);
  else
    h = sll_init_t(project, name);
  
  if (key)
    sll_handle((uccp)key);
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
		sll_handle((uccp)key);
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

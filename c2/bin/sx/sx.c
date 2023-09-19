#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <mesg.h>
#include <tree.h>
#include <json.h>
#include <xml.h>
#include <../../nss/asl/asl.h>
#include <gdl.h>
#include <oraccsys.h>
#include <oracclocale.h>
#include "sx.h"
#include "help.h"

Mloc *xo_loc;
FILE *f_xml;
const char *file;
int verbose;
int asltrace,rnvtrace;

int status = 0; /* for rnc; should be in library there */

int asl_output = 0;
int identity_mode = 0;
int jsn_output = 0;
int list_dump = 0;
int list_names_mode = 0;
int listdef_check = 0;
int sll_output = 0;
int sortcode_output = 0;
int tree_output = 0;
int unicode_table = 0;
int xml_output = 0;

extern int asl_raw_tokens; /* ask asl to produce list of @sign/@form/@v tokens */
extern int ctrace;

int boot_mode = 0;
int check_mode = 0;
int trace_mode = 0;
extern int asl_flex_debug, gdl_flex_debug, gdl_unicode;

const char *missing_lists = NULL;

int
main(int argc, char * const*argv)
{
  struct sl_signlist *sl;
  FILE *sllout = stdout;
  
  xo_loc = malloc(sizeof(Mloc));
  mesg_init();
  asl_flex_debug = gdl_flex_debug = 0;
  gdl_unicode = 1;

  gsort_init();
  
  options(argc, argv, "abcCijlm:nMsStTux?");
  asltrace = asl_flex_debug = trace_mode;

  if (boot_mode)
    {
      sll_output = 1;
      file = "00lib/ogsl.asl";
      if (!freopen(file, "r", stdin))
	{
	  fprintf(stderr, "sx: unable to read from %s\n", file);
	  exit(1);
	}
      const char *outfile = "02pub/sl/sl.tsv"; /* FIXME: should be @@ORACC@@/ogsl ... */
      if (!(sllout = fopen(outfile, "w")))
	{
	  fprintf(stderr, "sx: unable to write to %s\n", outfile);
	  exit(1);
	}
    }
  else
    {
      if (argv[optind])
	{
	  file = argv[optind];
	  if (!freopen(file, "r", stdin))
	    {
	      fprintf(stderr, "sx: unable to read from %s\n", file);
	      exit(1);
	    }
	}
    }
  
  mesg_init();
  nodeh_register(treexml_o_handlers, NS_SL, treexml_o_generic);
  nodeh_register(treexml_c_handlers, NS_SL, treexml_c_generic);
  gdl_init();
  asl_init();
  sl = aslyacc(file);

  if (sl)
    {
      if (list_names_mode)
	{
	  sx_list_dump(stdout, sl);
	  exit(0);
	}

      sx_marshall(sl);

      if (unicode_table)
	{
	  FILE *f = stdout; /*fopen("sx-unicode.tab","w");*/
	  if (f)
	    {
	      sx_unicode_table(f, sl);
	      fclose(f);
	    }
	  else
	    fprintf(stderr, "sx: unable to dump list data; can't write sx-listdata.out\n");
	}
      
      if (list_dump)
	{
	  FILE *lfp = stdout; /*fopen("sx-listdata.out","w");*/
	  if (lfp)
	    {
	      sx_list_dump(lfp, sl);
	      fclose(lfp);
	    }
	  else
	    fprintf(stderr, "sx: unable to dump list data; can't write sx-listdata.out\n");
	}
      
      if (listdef_check)
	sx_listdefs(sl, missing_lists);
      
      if (asl_output)
	sx_walk(sx_w_asl_init(stdout, "-"), sl);

      if (jsn_output)
	sx_walk(sx_w_jsn_init(stdout, "-"), sl);
	
      if (sll_output)
	sx_s_sll(sllout, sl);
      
      if (xml_output)
	sx_walk(sx_w_xml_init(stdout, "_"), sl);
    }
  
  gdl_term();
  asl_term();
  asl_bld_term(sl);
  mesg_print(stderr);
  return mesg_status();
}

int
opts(int opt, char *arg)
{
  switch (opt)
    {
    case 'a':
      asl_output = 1;
      break;
    case 'b':
      boot_mode = 1; /* write sll output to 02pub/sl/sl.tsv */
      break;
    case 'c':
      check_mode = 1;
      break;
    case 'C':
      ctrace = 1;
      break;
    case 'i':
      asl_output = identity_mode = 1;
      break;
    case 'j':
      jsn_output = 1;
      break;
    case 'l':
      list_dump = 1;
      break;
    case 'n':
      list_names_mode = 1;
      break;
    case 'm':
      listdef_check = 1;
      missing_lists = arg;
      break;
    case 'M':
      listdef_check = 1;
      missing_lists = NULL;
      break;
#if 0
    case 'r':
      asl_raw_tokens = 1;
      break;
#endif
    case 's':
      sll_output = 1;
      break;
    case 'S':
      sortcode_output = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
#if 0
    case 'T':
      tree_output = 1;
      break;
#endif
    case 'u':
      unicode_table = 1;
      break;
    case 'x':
      xml_output = 1;
      break;
    case '?':
      help();
      exit(1);
      break;
    default:
      return 1;
      break;
    }
  return 0;
}

void
help(void)
{
  help_longest_opt("m [LIST]");
  help_title("sx: The Oracc signlist processor for .asl files");
  help_usage("  Usage: sx [OPTIONS] [ASL-FILE]");

  help_heading("Mode Options");
  help_option("b", "boot-mode: write signlist data output to 02pub/sl/sl.tsv");
  help_option("c", "check-mode: check the signlist and exit");
  help_option("C", "ctrace-mode: trace compound processing for debugging purposes");
  help_option("i", "identity-mode: produce 'identity' .asl output.\n"
	      	   "\t\tOutput may be re-sorted and/or have new @ucun tags.");
  help_option("t", "trace-mode: turn on tracing for debugging purposes");

  help_heading("Output Options");
  help_option("", "(All the following outputs are written to stdout)\n");
  help_option("a", "asl-output: this adds @letter and @group tags. See also -i.");
  help_option("j", "json-output: a JSON version of the signlist; beta");
  help_option("s", "sll-output: data for the Sign-List-Library, sll, also used by GVL");
  help_option("S", "Sortcode-output: show a list of OIDs and sort-codes");
  help_option("x", "xml-output: an XML version of the signlist");
  
  help_heading("List and Coverage Options");
  help_option("", "(All the following outputs are written to stdout)\n");
  help_option("l", "list-dump: show all list entries in signlist");
  help_option("M", "missing-all: show missing entry information for all lists");
  help_option("m [LIST]", "missing [LIST]: show missing entry information for the LIST, e.g., -m MZL");
  help_option("n", "names-of-lists: show list -names defined in signlist");
  help_option("u", "unicode: show a Unicode coverage data");
  help_str("",0);
}

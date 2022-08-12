#include <stdio.h>
#include <unistd.h>
#include <psd_base.h>
#include "options.h"
#include "runexpat.h"
#include "xmlutil.h"
#include "atf.h"
#include "gdl.h"
#include "lang.h"
#include "pool.h"
#include "npool.h"
#include "gx.h"
#include "rnvif.h"
#include "../lib/rnv/rnl.h"

const char *errmsg_fn = NULL;

int acdstyle_xml = 0;
int flextrace = 0;
int identity_output = 0;
int json_output = 0;
int keepgoing = 0;
int output = 0;
int stdin_input = 0;
int xml_output = 0;

extern int yyparse(void);
struct cbd* curr_cbd;
struct entry*curr_entry;
const char *efile = NULL;
const char *rncfile = NULL;
extern void yyrestart(FILE*);
extern char *cbdrnc(void);

extern int parser_status;

extern int yydebug;
extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

static void cbd_rnc_init(void);

int
main(int argc, char **argv)
{
  options(argc,argv,"acdefikr:stvx");

#if 1
  file = argv[optind];
#else
  const char *fname[2];

  /* no way to set stdin_input atm */
  if (!stdin_input)
    {
      fname[0] = file = argv[optind];
      fname[1] = NULL;
    }

  if (outfile)
    f_xml = outfp = xfopen(outfile,"w");
  else
    f_xml = stdout;
#endif

  f_log = stderr;
  math_mode = no_pi = do_cuneify = use_unicode = 1;
  
  common_init();
  galloc_init();
  pool_init();
  tree_init();
  gdl_init();
  msglist_init();
  curr_lang = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  cbds = hash_create(1);
  with_textid = 0;

  if (rncfile)
    {
      FILE *f_rnc = fopen(rncfile,"w");
      if (!f_rnc)
	{
	  fprintf(stderr,"ox: unable to write RNC file %s\n",rncfile);
	  exit(2);
	}
      else
	{
	  fputs(cbdrnc(),f_rnc);
	  fclose(f_rnc);
	  exit(0);
	}
    }
  
  if (file)
    {
      FILE *fp;
      efile = file;
      if ((fp = xfopen(efile, "r")))
	yyrestart(fp);
    }
  curr_cbd = bld_cbd();
  phase = "syn";
  if (yyparse() || parser_status)
    {
      if (!keepgoing)
	{
	  msglist_print(stderr);
	  fprintf(stderr, "gx: exiting after syntax errors\n");
	  exit(1);
	}
    }

  if (check || output)
    validator(curr_cbd);
  
  if (identity_output)
    identity(curr_cbd);

  if (xml_output)
    {
      extern void rnvxml_rnvif_init(void);
      int rnvok = -1;
      rnvxml_rnvif_init();
      rnvif_init();
      cbd_rnc_init();
      rnv_validate_start();      
      xmloutput(curr_cbd);
      rnvok = rnv_validate_finish();
      rnvif_term();
      if (verbose)
	fprintf(stderr, "rnv returned %d\n", rnvok);
    }
  
  msglist_print(stderr);
  
  lang_term();
  gdl_term();
  msglist_term();
  pool_term();
  tree_term(1);
  galloc_term();
  common_term();
  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "gx";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'a':
      acdstyle_xml = 1;
      break;
    case 'b':
      break;
    case 'c':
      check = 1;
      break;
    case 'd':
      yydebug = 1;
      break;
    case 'e':
      entries = 1;
      break;
    case 'f':     
      break;
    case 'g':
      break;
    case 'i':
      output = identity_output = 1;
      break;
    case 'j':
      output = json_output = 1;
      break;
    case 'k':
      keepgoing = 1;
      break;
    case 'n':
      break;
    case 'o':
      break;
    case 'p':
      break;
    case 'r':
      rncfile = optarg;
      break;
    case 's':
      sigs = 1;
      break;
    case 't':
      flextrace = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      acdstyle_xml = output = xml_output = 1; /* in future we will move away from acdstyle */
      break;
    default:
      return 1;
    }
  return 0;
}

void
cbd_rnc_init(void)
{
  extern int cbdrnc_len;
  if (xml_output)
    {
      char *cbd = cbdrnc();
      rnc_start = rnl_s("ORACC_SCHEMA/cbd.rnc",cbd,cbdrnc_len);
      status = !rnc_start;
    }
}

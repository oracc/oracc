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

extern struct iom *iomethod(const char *str, size_t len);
struct iom *input_method, *output_method;

const char *errmsg_fn = NULL;

int flextrace = 0;
int identity_output = 0;
int json_output = 0;
int keepgoing = 0;
int output = 0;
int stdin_input = 0;
int xml_output = 0;

struct cbd* curr_cbd;
struct entry*curr_entry;
const char *efile = NULL;
const char *rncfile = NULL;
extern char *cbdrnc(void);

extern int parser_status;

extern int tg1debug, tg2debug;
extern int tg1parse(void);
/*extern int tg2parse(void);*/
extern void tg1restart(FILE*);
/*extern void tg2restart(FILE*);*/

extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

static void cbd_rnc_init(void);

static void gx_init(void);
static void gx_run(void);
static void gx_term(void);
static void io_init(void);

static void
gx_init()
{
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
}

static void
gx_run()
{
  if (file)
    {
      FILE *fp;
      efile = file;
      if ((fp = xfopen(efile, "r")))
	tg1restart(fp);
    }
  curr_cbd = bld_cbd();
  phase = "syn";
  if (tg1parse() || parser_status)
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
      rnv_validate_start();      
      xmloutput(curr_cbd);
      rnvok = rnv_validate_finish();
      rnvif_term();
      if (verbose)
	fprintf(stderr, "rnv returned %d\n", rnvok);
    }
  
  msglist_print(stderr);
}

static void
gx_term()
{
  lang_term();
  gdl_term();
  msglist_term();
  pool_term();
  tree_term(1);
  galloc_term();
  common_term();
}

static void
io_init(void)
{
  ;
}

int
main(int argc, char **argv)
{
  options(argc,argv,"A:I:O:i:o:v");

  gx_init();

  io_init();

  gx_run();
  
  gx_term();

  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "gx";
const char *usage_string = "[-I input-type] [-O output-type] [-A action] -i <FILE|-> -o <FILE|->";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'I':
      input_method = iomethod(optarg, strlen(optarg));
      break;
    case 'O':
      output_method = iomethod(optarg, strlen(optarg));
      break;
    case 'a':
      break;
    case 'b':
      break;
    case 'c':
      check = 1;
      break;
    case 'd':
      tg1debug = 1; /* = tg2debug */
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
      output = xml_output = 1;
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
  else if (output_method)
    {
      char fn[12];
      sprintf(fn, "cbd-%s.rnc", output_method->name);
      if (!xaccess(fn, R_OK, 0))
	{
	  fprintf(stderr, "gx: found output method schema %s\n", fn);
	  rnc_start = rnl_fn(fn);
	  status = !rnc_start;
	}
      else
	{
	  fprintf(stderr, "gx: no such output method schema %s\n", fn);
	}
    }
}

#include <stdio.h>
#include <unistd.h>
#include <oraccsys.h>
#include <c2types.h>
#include <iome.h>
#include "pool.h"
#include "tree.h"
#if 0
#include "options.h"
#include "runexpat.h"
#include "xmlutil.h"
#include "atf.h"
#endif
#include <lng.h>
#include <gdl.h>
#include <cbd.h>
#include <cbdyacc.h>
#include "gx.h"

#if 0
#include "rnvif.h"
#include "../lib/rnv/rnl.h"
#include "rnvtgi.h"
#include "rnvxml.h"
#endif

extern int cbd_flex_debug, cbddebug;

static const char *default_input_method = "tg1";

extern Iome *iomethod(const char *str, size_t len);
static Iome *input_method, *output_method;
static Iome_io  input_io, output_io;
const char *input_file, *output_file;

Pool *cbd_pool = NULL;
Tree *cbd_tree = NULL;

/* Different error mechanisms in libraries need to be unified */
const char *errmsg_fn = NULL;
const char *efile = NULL;

Hash *cbds = NULL;

int check = 0;
int trace_mode = 0;
int rnvtrace = 0;
int keepgoing = 0;
int input_validation = 0;
int lnum = 0;
int math_mode = 0;
int output_validation = 0;
int parser_status = 0;
int sigs = 0;
int status = 0;
int stdin_input = 0;
int stdin_output = 0;
int verbose = 0;

const char *file = NULL;
FILE *f_log, *f_xml;
int no_pi = 0;
int do_cuneify = 0;
int use_unicode = 1;
int with_textid = 0;

struct cbd* curr_cbd;
struct entry*curr_entry;

extern int parser_status;

int cbddebug;
extern int cbdparse(void);
extern void cbdrestart(FILE*);

extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

static void gx_init(void);
static void gx_term(void);
static void io_init(void);

static void
gx_init(void)
{
  f_log = stderr;
  math_mode = no_pi = do_cuneify = use_unicode = 1;
  common_init();
  cbd_pool = pool_init();
  tree_init();
  mesg_init();
  gvl_setup("ogsl","ogsl");
  gdlparse_init();
  lng_init();
  curr_lang_ctxt = global_lang = lang_switch(NULL,"sux",NULL,NULL,0);
  cbds = hash_create(1);
  with_textid = 0;
}

#if 0
static void
gx_run()
{
  if (file)
    {
      FILE *fp;
      efile = errmsg_fn = file;
      if ((fp = xfopen(efile, "r")))
	tg1restart(fp);
    }
  curr_cbd = bld_cbd();
  phase = "syn";
  if (tg1parse() || parser_status)
    {
      if (!keepgoing)
	{
	  mesg_print(stderr);
	  fprintf(stderr, "cx: exiting after syntax errors\n");
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
  
  mesg_print(stderr);
}
#endif

static void
gx_term(void)
{
  lang_term();
  gdlparse_term();
  mesg_term();
  pool_term(cbd_pool);
  tree_term(cbd_tree);
  common_term();
}

static void
io_init(void)
{
  if (!input_method)
    input_method = iomethod(default_input_method, strlen(default_input_method));

  if (!check && !output_method)
    output_method = iomethod(default_input_method, strlen(default_input_method));

  if (output_method)
    {
      input_validation = 0;
      output_validation = 1;
    }
  else
    {
      input_validation = 1;
      output_validation = 0;
    }
  
  memset(&input_io, '\0', sizeof(Iome_io ));
  memset(&output_io, '\0', sizeof(Iome_io ));

  if (!input_file)
    input_file = "-";
  if (!check && !output_file)
    output_file = "-";

  input_io.fn = input_file;
  if (!efile)
    efile = errmsg_fn = input_file;
  if (!strcmp(input_file, "-"))
    input_io.use_stdio = 1;

  if (output_file)
    {
      output_io.fn = output_file;
      if (!strcmp(output_file, "-"))
	output_io.use_stdio = 1;
    }
}

static void
io_run(void)
{
#if 0
  extern struct xnn_data cbd_tg1_data, cbd_tg2_data, cbd_xc2_data;  
  extern void rnvtgi_init(struct xnn_data *xdp, const char *rncbase);
  extern void rnvtgi_term(void);
#endif
  extern void cbd_l_init(Iome_io  *ip);
  extern void cbd_l_term(void);
  
  int parse_return = 0;
  
  switch (input_method->type)
    {
    case iome_cbd:
#if 0
      rnvtgi_init_err();
      rnvif_init();
      rnvtgi_init(&cbd_tg1_data, input_method->name);
#endif
      cbd_l_init(&input_io);
      curr_cbd = cbd_bld_cbd();
      phase = "syn";
      parse_return = cbdparse();
      /*rnvtgi_term();*/
      if (parse_return || parser_status)
	{
	  mesg_print(stderr);
	  if (!keepgoing)
	    {
	      fprintf(stderr, "cx: exiting after syntax errors\n");
	      exit(1);
	    }
	}

      if (check)
	validator(curr_cbd);

      cbd_l_term();
      break;
#if 0
    case iome_tg2:
      rnvtgi_init_err();
      rnvif_init();
      rnvtgi_init(&cbd_tg2_data, input_method->name);
      tg2_l_init(&input_io);
      curr_cbd = bld_cbd();
      phase = "syn";
      parse_return = tg2parse();
      rnvtgi_term();
      if (parse_return || parser_status)
	{
	  mesg_print(stderr);
	  if (!keepgoing)
	    {
	      fprintf(stderr, "cx: exiting after syntax errors\n");
	      exit(1);
	    }
	}
      tg1_l_term();
      break;
    case iome_xc1:
    case iome_xc2:
    case iome_x11:
    case iome_x12:
    case iome_x21:
    case iome_x22:
#endif
    default:
      fprintf(stderr, "cx: %s input not supported\n", input_method->name);
      exit(1);
      break;
    }

  /* THIS IS WHERE ACTIONS WILL BE EXECUTED */

  if (output_method)
    {
      switch (output_method->type)
	{
	case iome_cbd:
	  /*identity(curr_cbd);*/
	  fprintf(stderr, "cx: tg1 output request should use tg2\n");
	  exit(1);
	  break;
#if 0
	case iome_tg2:
	  o_tg2(curr_cbd);
	  break;
	case iome_xc1:
	  fprintf(stderr, "cx: xg1 output request should use xc2\n");
	  exit(1);
	case iome_xc2:
	  rnvxml_init_err();
	  rnvif_init();
	  rnvxml_init(&cbd_xc2_data, output_method->name);
#endif
	  o_xc2(curr_cbd);
	  break;
	case iome_x11:
	case iome_x12:
	case iome_x21:
	case iome_x22:
	default:
	  fprintf(stderr, "cx: %s output not supported\n", output_method->name);
	  break;
	}
    }
}

int
main(int argc, char **argv)
{
  extern void cbdset_debug(int);
  extern int gdl_flex_debug, gdldebug;
  
  status = 0;
  options(argc,argv,"A:I:O:i:o:ckrtTv");

  if (status)
    {
      fprintf(stderr, "cx: quitting after errors in option processing\n");
      exit(1);
    }

  gdl_flex_debug = gdldebug = cbd_flex_debug = cbddebug = trace_mode;
  cbdset_debug(trace_mode);
  
  gx_init();

  io_init();

  if (!input_method)
    input_method = iomethod("cbd", 3);

  io_run();

  gx_term();

  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "cx";
const char *usage_string = "[OPTIONS] [-I input-type] [-O output-type] -i <FILE|-> -o <FILE|->";
void help(void)
{
  fprintf(stderr, "OPTIONS:\n\n");
  fprintf(stderr, "\t-c\tcheck input and exit without creating output\n");
  fprintf(stderr, "\t-d\tdebug mode\n");
  fprintf(stderr, "\t-e\tset file name to use in errors\n");
  fprintf(stderr, "\t-k\tkeep going despite errors\n");
  fprintf(stderr, "\t-t\ttrace tokenizing\n");
  fprintf(stderr, "\t-T\ttrace TGI validation\n");
  fprintf(stderr, "\t-v\tverbose mode\n");
  fprintf(stderr, "\n\t-A [ACTIONS]\n\n");
  fprintf(stderr, "ACTIONS:\n\n");
  fprintf(stderr, "\t(none yet)\n\n");
}
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'I':
      if (!(input_method = iomethod(optarg, strlen(optarg))))
	{
	  fprintf(stderr, "cx: unknown input method: %s\n", optarg);
	  status = 1;
	}
      break;
    case 'O':
      if (!(output_method = iomethod(optarg, strlen(optarg))))
	{
	  fprintf(stderr, "cx: unknown output method: %s\n", optarg);
	  status = 1;
	}	
      break;
    case 'a':
      break;
    case 'b':
      break;
    case 'c':
      check = 1;
      break;
    case 'd':
      cbddebug = 1; /* = tg2debug */
      break;
    case 'e':
      efile = errmsg_fn = optarg;
      break;
    case 'f':
      break;
    case 'g':
      break;
    case 'i':
      input_file = optarg;
      break;
    case 'j':
      break;
    case 'k':
      keepgoing = 1;
      break;
    case 'n':
      break;
    case 'o':
      output_file = optarg;
      break;
    case 'p':
      break;
    case 'r':
      rnvtrace = 1;
      break;
    case 's':
      sigs = 1;
      break;
    case 't':
      trace_mode = 1;
      break;
    case 'T':
      rnvtrace = 1;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'x':
      break;
    default:
      return 1;
    }
  return 0;
}

#if 0
void
cbd_rnc_init(void)
{
  if (xml_output)
    {
#if 0      
      extern int cbdrnc_len;
      char *cbd = cbdrnc();
      rnc_start = rnl_s("ORACC_SCHEMA/cbd.rnc",cbd,cbdrnc_len);
      status = !rnc_start;
#endif
    }
  else if (output_method)
    {
      char fn[12];
      sprintf(fn, "%s.rnc", output_method->name);
      if (!xaccess(fn, R_OK, 0))
	{
	  fprintf(stderr, "cx: found output method schema %s\n", fn);
	  rnc_start = rnl_fn(fn);
	  status = !rnc_start;
	}
      else
	{
	  fprintf(stderr, "cx: no such output method schema %s\n", fn);
	}
    }
}
#endif

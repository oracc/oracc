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

static const char *default_input_method = "tg1";

extern struct iom *iomethod(const char *str, size_t len);
static struct iom *input_method, *output_method;
static struct iom_io input_io, output_io;
const char *input_file, *output_file;

/* Different error mechanisms in libraries need to be unified */
const char *errmsg_fn = NULL;
const char *efile = NULL;

int flextrace = 0;
int rnvtrace = 0;
int keepgoing = 0;
int input_validation = 1;
int stdin_input = 0;
int stdin_output = 0;

struct cbd* curr_cbd;
struct entry*curr_entry;

extern int parser_status;

extern int tg1debug, tg2debug;
extern int tg1parse(void);
/*extern int tg2parse(void);*/
extern void tg1restart(FILE*);
/*extern void tg2restart(FILE*);*/

extern int math_mode;
extern int cbd(const char *fname);
extern int flex(const char *fname);

static void gx_init(void);
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
#endif

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
  if (!input_method)
    input_method = iomethod(default_input_method, strlen(default_input_method));

  if (!check && !output_method)
    output_method = iomethod(default_input_method, strlen(default_input_method));
  
  memset(&input_io, '\0', sizeof(struct iom_io));
  memset(&output_io, '\0', sizeof(struct iom_io));

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
  extern struct xnn_data cbd_tg1_data;
  extern void rnvtgi_init(struct xnn_data *xdp, const char *rncbase);
  extern void rnvtgi_term(void);
  extern void tg1_l_init(struct iom_io *ip);
  extern void tg1_l_term(void);
  
  switch (input_method->type)
    {
    case iom_tg1:
      rnvtgi_init_err();
      rnvif_init();
      rnvtgi_init(&cbd_tg1_data, input_method->name);
      tg1_l_init(&input_io);
      curr_cbd = bld_cbd();
      phase = "syn";
      tg1parse();
      rnvtgi_term();
      if (tg1parse() || parser_status)
	{
	  msglist_print(stderr);
	  if (!keepgoing)
	    {
	      fprintf(stderr, "gx: exiting after syntax errors\n");
	      exit(1);
	    }
	}
      tg1_l_term();
      break;
    case iom_tg2:
      break;
    case iom_xg1:
    case iom_xg2:
    case iom_x11:
    case iom_x12:
    case iom_x21:
    case iom_x22:
    default:
      fprintf(stderr, "gx: %s input not supported\n", input_method->name);
      exit(1);
      break;
    }

  /* THIS IS WHERE ACTIONS WILL BE EXECUTED */

  if (output_method)
    {
      switch (output_method->type)
	{
	case iom_tg1:
	  identity(curr_cbd);
	  break;
	case iom_tg2:
	case iom_xg1:
	case iom_xg2:
	case iom_x11:
	case iom_x12:
	case iom_x21:
	case iom_x22:
	default:
	  fprintf(stderr, "gx: %s output not supported\n", output_method->name);
	  break;
	}
    }
}

int
main(int argc, char **argv)
{
  status = 0;
  options(argc,argv,"A:I:O:i:o:krtv");
  if (status)
    {
      fprintf(stderr, "gx: quitting after errors in option processing\n");
      exit(1);
    }

  gx_init();

  io_init();

  io_run();

  gx_term();

  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "gx";
const char *usage_string = "[OPTIONS] [-I input-type] [-O output-type] -i <FILE|-> -o <FILE|->";
void help()
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
	  fprintf(stderr, "gx: unknown input method: %s\n", optarg);
	  status = 1;
	}
      break;
    case 'O':
      if (!(output_method = iomethod(optarg, strlen(optarg))))
	{
	  fprintf(stderr, "gx: unknown output method: %s\n", optarg);
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
      tg1debug = 1; /* = tg2debug */
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
      flextrace = 1;
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
#endif

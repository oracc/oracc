#define OX_VERSION 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <getopt.h>
#include <string.h>
#include <ctype128.h>
#include <stdarg.h>
#include "warning.h"
#include "atffile.h"
#include "loadfile.h"
#include "pool.h"
#include "proj_context.h"
#include "rnvif.h"
#include "run.h"
#include "cdf_rnc.h"
#include "list.h"
#include "globals.h"
#include <signal.h>

#include "ox.h"

static char *crashbuf = NULL;

static void 
clean_exit_on_sig(int sig_num)
{
  (void)write(STDERR_FILENO, crashbuf, strlen(crashbuf));
  exit(128+sig_num);
}

static void
signals(int argc, char **argv)
{
  int i, len;
  for (i = len = 0; i < argc; ++i)
    len += strlen(argv[i]);
  crashbuf = malloc(len + argc + 128);
  strcpy(crashbuf, "ox: emergency stop, program crash. Invoked as:\n\t");
  for (i = 0; i < argc; ++i)
    {
      if (i)
	strcat(crashbuf, " ");
      strcat(crashbuf, argv[i]);
    }
  strcat(crashbuf, "\n");
  signal(SIGINT  , clean_exit_on_sig);
  signal(SIGABRT , clean_exit_on_sig);
  signal(SIGILL  , clean_exit_on_sig);
  signal(SIGFPE  , clean_exit_on_sig);
  signal(SIGSEGV , clean_exit_on_sig);
  signal(SIGTERM , clean_exit_on_sig);
}

const char *automatic_styles = NULL,
  *cdtfile = NULL, 
  *master_styles = NULL,
  *project_dir = NULL,
  *styles_styles = NULL;

char project_base[_MAX_PATH];

const char *output_dir;
List *cdt_master_list, *cdt_meta_list, *cdt_styles_list;

#define xstr(s) str(s)
#define str(s)  #s

const char*atffile = NULL;
FILE*f_atf = NULL;
const char *badfile = NULL;
FILE*f_bad = NULL;
const char *formsfile = NULL;
const char *goodfile = NULL;
FILE*f_good = NULL;
const char *graphemesfile = NULL;
/*FILE*f_graphemes = NULL;*/
const char *lemmafile = NULL;
FILE*f_lemma = NULL;
const char*logfile = NULL;
/*FILE*f_log = NULL;*/
const char*norm1file = NULL;
const char *rncfile = NULL;
FILE*f_rnc = NULL;
const char *xmlfile = NULL;
/*FILE*f_xml = NULL;*/
const char *unlemmfile = NULL;
FILE *f_unlemm;

const char *inputs_from_file = NULL;

int bootstrap_mode = 0;
/*int check_lem = 0;*/
int check_links = 0;
int check_only = 0;
int do_cdl = 0;
int force = 0;
int harvest_notices = 0;
int lem_autolem = 0;
int lem_forms_raw = 0;
int lem_standalone = 0;
int lem_system = 1;
int named_ents = 0;
int no_xcl_map = 0;
int non_xtf_output = 0;
/*int no_pi = 0;*/
int one_big_file = 1; /* per-file outputs not supported yet */

int perform_dsa = 0;
/*N.B.: it is wrong for callers to check perform_lem for anything
  but serialization; various checking and annotation routines require
  the lemmatization subsystems even when perform_lem == 0, so callers
  should always use need_lemm rather than perform_lem
 */
int perform_lem = 0;
int perform_nsa = 0;
int process_detchars = 0;
int psu = 0;
int check_pnames = 0;
int shadow_lem = 0;
int with_noform = 0;

int global_psa_context = 0;
int psa_nents = 0;
int psa_props = 0;

const char *project = "";
const char *arg_project = NULL;
const char *xcl_project = NULL;
extern const char *system_project;

/*int show_toks = 0;*/
/*int sparse_lem = 0;*/

int validate = 1;
extern int verbose;
int warn_unlemmatized = 0;

/*char sparse_lem_fields[32];*/

extern struct nsa_context *global_nsa_context;

static void finish_outputs(void);

int
main(int argc, char **argv)
{
  struct run_context *run = NULL;
  verbose = 0;
  signals(argc, argv);
  run = run_init();
  pool_init();
  no_gdl_xids = 0;
  if (!ox_options(argc,argv))
    {
      ox_init();

      post_option_setup(run);

      if (arg_project)
	proj_init(run, arg_project);
      else
	*project_base = '\0';

      process_inputs(run, argc, argv);

      finish_outputs();

      ox_term();
      free(crashbuf);
    }

  if (!exit_status && lem_status)
    exit_status = 3; /* clean other than lemm errors */

  if (verbose)
    fprintf(stderr,"ox: exiting with status=%d\n",exit_status);

  print_error_count(f_log,0);
  run_term(run);
  return exit_status;
}

static void
finish_outputs(void)
{
  if (fp_forms)
    {
      fputc('\n',fp_forms);
      fclose(fp_forms);
    }
  if (f_graphemes)
    {
      fputc('\n',f_graphemes);
      fclose(f_graphemes);
    }
  if (f_lemma)
    {
      fclose(f_lemma);
    }
}

const char *prog = "ox";
int major_version = OX_VERSION;
int minor_version = 1;
const char *usage_string = "";
int opts() {return 0;}

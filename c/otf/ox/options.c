#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "initterm.h"
#include "catvec.h"
#include "ox.h"
#include "globals.h"

int v_flag = 0;
int
ox_options(int argc, char **argv)
{
  while (1)
    {
      int optch = getopt(argc,argv,
			 "$+a:Ab:BcCd:De:EfFg:G:hHiI:jJkKl:L:mMnNo:OpP:qQr:Rs:StT:uU:vVwWx:XyYz:Z?012348");
      if (optch < 0)
	break;
      switch (optch)
	{
	case 'a':
	  atffile = input_file = optarg;
	  break;
	case 'A':
	  lem_autolem = perform_lem = need_lemm = 1;
	  break;
	case 'b':
	  badfile = optarg;
	  break;
	case 'B':
	  bootstrap_mode = 1;
	  break;
	case 'c':
	  check_only = 1;
	  validate = 0;
	  break;
	case 'C':
	  do_cdl = need_lemm = 1;
	  break;
	case 'd':
	  output_dir = optarg;
	  break;
	case 'D':
	  ++lem_extended;
	  break;
	case '$':
	  no_strict_dollar = 1;
	  break;
	case 'e':
	  errmsg_fn = optarg;
	  break;
	case 'E': /* Emacs error mode */
	  with_textid = 0;
	  break;
	case 'f':
	  force = 1;
	  break;
	case 'F':
	  /* formsfile = optarg; */
	  fuzzy_aliasing = 1;
	  break;
	case 'g':
	  goodfile = optarg;
	  break;
	case 'G':
	  graphemesfile = optarg;
	  break;
	case 'h':
	  harvest_notices = 1;
	  break;
	case 'H':
	  shadow_lem = 1;
	  break;
	case 'i':
	  ignore_plus = 1;
	  break;
	case 'I':
	  inputs_from_file = optarg;
	  break;
	case 'j':
	  pretty = 1;
	  break;
	case 'J':
	  perform_dsa = 1;
	  break;
	case 'k': /* show 'known' P-nums */
	  catvec_init();
	  catvec_dump();
	  exit(1);
	  break;
	case 'K':
	  compound_warnings = 1;
	  break;
	case 'l':
	  logfile = optarg;
	  break;
	case 'L':
	  lemmafile = optarg;
	  break;
	case 'm':
	  do_cdl = check_lem = need_lemm = 1;
	  break;
	case 'M':
	  lem_autolem = do_cdl = perform_lem = need_lemm = 1;
	  no_destfile = 1;
	  break;
	case 'n':
	  no_pi = 1;
	  break;
	case 'N':
	  need_lemm = do_cdl = 1;
	  perform_lem = 0;
	  no_xcl_map = 1;
	  break;
	case 'o':
	  cdtfile = input_file = optarg;
	  break;
	case 'O':
	  do_cdl = need_lemm = perform_nsa = 1;
	  break;
	case 'p':
	  check_pnames = 1;
	  init_pnames();
	  break;
	case 'P':
	  /*set_project(optarg);*/
	  arg_project = optarg;
	  break;
	case 'q':
	  cbd_lem_sigs = 1;
	  break;
	case 'Q':
	  atf_cbd_err = 1;
	  break;
	case 'r':
	  rncfile = optarg;
	  break;
	case 'R':
	  lem_forms_raw = 1;
	  break;
	case 's':
	  system_project = optarg;
	  break;
	case 'S':
	  lem_standalone = 1;
	  break;
	case 't':
	  show_toks = 1;
	  break;
	case 'T':
	  lem_percent_threshold = atoi(optarg);
	  break;
	case 'x':
	  /* Note that this may be xmlfile or autolem_fn because if -A is specified no XML is output so 
	     we overload the option */
	  xmlfile = optarg;
	  break;
	case 'X':
	  check_links = 1;
	  break;
	case 'u':
	  psu = 1;
	  break;
	case 'U':
	  warn_unlemmatized = 1;
	  unlemmfile = optarg;
	  break;
	case 'v':
	  v_flag = 1;
	  break;
	case 'V':
	  ++verbose;
	  if (verbose > 1)
	    ng_debug = 1;
	  break;
	case 'w':
	  do_cuneify = 1;
	  break;
	case 'W':
	  lem_do_wrapup = 0;
	  break;
	case 'y':
	  named_ents = 1;
	  break;
	case 'Y':
	  named_ents = 2;
	  break;
	case 'z':
	  fallback_project = optarg;
	  break;
	case 'Z':
	  process_detchars = 1;
	  break;
	case '?':
	  help();
	case '0':
	  odt_serial = 2;
	  break;
	case '1':
	  odt_serial = 1;
	  break;
	case '2':
	  report_all_label_errors = 1;
	  break;
	case '3':
	  cuneify_notices = 1;
	  break;
	case '4':
	  dump_sigs = 1;
	  check_only = 1;
	  break;
	case '8':
	  arg_use_unicode = 1;
	  break;
	default:
	  fprintf(stderr,"unhandled option %c",optch);
	  help();
	  break;
	}
    }
  return status;
}

/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: options.c,v 0.5 1997/09/08 14:50:06 sjt Exp sjt $
*/

/* Module to build customized option processing over top of Gnu
 * getopt
 *
 * The usual way to use these routines is to write a routine 'opts',
 * which takes the option and arg string, does stuff with it and 
 * returns 0 if OK, 1 on error (such as a bad option), and another
 * routine 'help' which is called by the 'usage' function if an
 * unknown option is supplied. Then near the start of the program
 * make the call:
 *
 *  options (argc, argv, "a string of opts as in getopt");
 */
#include <psd_base.h>
#include <ctype128.h>
#include <options.h>

static void def_opts (int argc, char *const *argv);
int _internal_argc;
const char *V_opt = "";
char *const*_internal_argv;
Boolean doing_debug;
const char *copyright_string = "Copyright(c) 1997-2022 Steve Tinney";
extern Unsigned16 _msg_invoke_flag;
static Boolean banner_done = FALSE;
static Boolean show_command_line;
static Boolean V_seen = FALSE;
static char *all_opts = NULL;
static void set_all_opts (const char *u_opts);
static void bad_option (Boolean give_error_message);

/**All programs have access to the help option (?), which should print a
 * screen or two of information to stdout. If it's more than a screenful
 * users can then pipe it through less, more or whatever.
 *
 * Users can see what they are using with the -B option. This prints
 * the banner which includes argv[0], the full version number info and
 * the copyright notice and informal name of the program. The banner is
 * automatically printed after option processing if verbose mode is on.
 *
 * Program-User interaction is controlled by two sets of options. The
 * first controls which message levels are generated:
 *	Q = quiet = only errors and fatals
 *	N = normal = message/warning/error/fatal
 *	V = verbose = progress + default
 *	VV = very verbose = command line + progress + default
 *	D = debug = verbose + debug
 *
 * The second set of options controls where output goes.
 *	S = no screen
 *	L = log file, with file name
 *	L+ = append to log file, with file name
 *
 */
static const char *default_opts = "?QNVDSL:";

void
default_options (int argc, char *const *argv, const char *u_opts)
{
  _internal_argc = argc;
  _internal_argv = argv;
  if (NULL == all_opts)
    set_all_opts (u_opts);
  def_opts (argc, argv);
  if (!_message_flags)
    _message_flags = MSG_DEFAULTS;
  if (MSG_ISSET(MSG_PROGRESS))
    banner ();
}

static void
def_opts (int argc, char *const*argv)
{
  int saved_optind = optind;
  int saved_opterr = opterr;
  int c;
  const char *opts_to_use = all_opts == NULL ? default_opts : all_opts;
  optind = opterr = 0;
  doing_debug = FALSE;
  while (EOF != (c = getopt (argc, argv, opts_to_use)))
    {
      switch (c)
	{
	case '?':
	  _message_flags = 0;
	  MSG_SET (MSG_PROGRESS | MSG_DEFAULTS);
	  bad_option (FALSE);
	  break;
	case 'Q':
	  MSG_CLR (MSG_PROGRESS | MSG_MESSAGE | MSG_WARNING);
	  MSG_SET (MSG_ERROR | MSG_FATAL);
	  V_opt = "-Q";
	  break;
	case 'N':
	  MSG_SET (MSG_DEFAULTS);
	  break;
	case 'V':
	  MSG_SET (MSG_PROGRESS | MSG_DEFAULTS);
	  V_opt = "-V";
	  if (V_seen)
	    show_command_line = TRUE;
	  else
	    V_seen = TRUE;
	  break;
	case 'D':
#ifdef DEBUG
	  MSG_SET (MSG_DEBUG | MSG_PROGRESS | MSG_DEFAULTS);
#else
	  mwarning (NULL, "default debugging information not available");
#endif
	  doing_debug = TRUE;
	  break;
	case 'S':
	  MSG_CLR (MSG_STDERR);
	  break;
	case 'L':
	  if ('+' == *optarg)
	    {
	      ++optarg;
	      if ('\0' == *optarg)
	        optarg = argv[optind++];
	      _msg_log_file = xfopen (optarg, "a");
	    }
	  else
	    _msg_log_file = xfopen (optarg, "w");
	  setvbuf (_msg_log_file, NULL, _IONBF, 0);
	  break;
	default:
	  /*DO NOTHING*/
	  break;
	}
    }
  optind = saved_optind;
  opterr = saved_opterr;
}

static void
bad_option (Boolean give_error_message)
{
  exit_on_error = FALSE;
  if (give_error_message)
    error (NULL, "unrecognized option\n");
  usage ();
}

void
user_options (int argc, char *const *argv, const char *u_opts)
{
  int c;

  if (NULL == all_opts)
    set_all_opts (u_opts);
  while (EOF != (c = getopt (argc, argv, all_opts)))
    {
      if ('?' == c)
        bad_option (FALSE);
      else if (opts (c, optarg) && NULL == strchr (default_opts, c))
	bad_option (TRUE);
    }
  free (all_opts);
}

static void 
set_all_opts (const char *u_opts)
{
  all_opts = malloc (strlen (u_opts) + strlen (default_opts) + 1);
  (void) sprintf (all_opts, "%s%s", u_opts, default_opts);
}

void
options (int argc, char *const *argv, const char *u_opts)
{
  default_options (argc, argv, u_opts);
  user_options (argc, argv, u_opts);
}

/**print the program name, version and copyright info
 */
void
banner ()
{
  if (banner_done)
    return;
  else
    banner_done = TRUE;
   
  message ("This is %-10s %2d.%02d %50s", 
	   prog, major_version, minor_version, copyright_string);
  if (show_command_line)
    {
      int i;
      message ("\nCommand line: %s", _internal_argv[0]);
      for (i = 1; i < _internal_argc; ++i)
	message (" %s", _internal_argv[i]);
    }
  message ("\n");
}

/**print the banner and the caller's help text and then exit.
 */
void
usage ()
{
  banner ();
  message ("Usage:\n      %s %s\n\n", prog, usage_string);
  help ();
  exit (EXIT_ERROR);
}

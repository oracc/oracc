/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: messages.c,v 0.5 1997/09/08 14:50:06 sjt Exp s $
*/

/* This module defines routines to simplify printing messages of various
 * kinds.
 */
#include <psd_base.h>
#include <options.h>
/**Callers can set this FALSE if they don't want the error function
 * to exit.
 */
Boolean exit_on_error = TRUE;

/* send all messages to stdout instead of stderr */
static Boolean messages_on_stdout = FALSE;

/**This is used to maintain a note of the highest level of warning/error
 * that has been perpetrated
 */
int history = 0;

/**Callers should set this through the macros provided
 */
Unsigned16 _message_flags = MSG_DEFAULTS;
Unsigned16 _msg_invoke_flag;

/**Callers should make this point to an error log file if they want to
 * output messages there as well as the screen. This is independent of
 * the MSG_SCREEN_OUTPUT bit in _message_flags.
 */
FILE *_msg_log_file;

/**Internal variable to prettify any mix of progress and warning/error
 * messages. If TRUE there's an unclosed progress line; callers should
 * use progress (NULL) to force a newline rather than progress("\n").
 */
static Boolean progress_newline_open = FALSE;

void
messages_to_stdout ()
{
  messages_on_stdout = TRUE;
  xsetvbuf (stdout, NULL, _IONBF, 0);
}

void
_message_kernel_nofile (const char * fmt,...)
{
  char buf[1024];
  va_list va;

  if (MSG_TYPE(MSG_DEBUG) && !MSG_ISSET(MSG_DEBUG))
    return;

  if (MSG_TYPE(MSG_PROGRESS))
    {
      if (!MSG_ISSET(MSG_PROGRESS))
	return;
      if (NULL == fmt)
	{
          fmt = "\n";
          progress_newline_open = FALSE;
	}
      else
	{
	  if ('\n' == fmt[strlen(fmt) - 1])
  	    progress_newline_open = FALSE;
	  else
	    progress_newline_open = TRUE;
	}
    }

/*lint -e737*/
  va_start (va, fmt);
/*lint +e737*/
  (void) vsprintf (buf, fmt, va);
  va_end (va);

  _message_kernel (NULL, buf);
}

Boolean err_newline = TRUE;
void
_message_kernel (File * file, const char * fmt,...)
{
  FILE *this_pass_file = _msg_log_file;
  Boolean second_pass = FALSE;

  /* don't do anything if this type of output is not wanted */
  if (MSG_ISSET (_msg_invoke_flag))
    {
    PASS_START:
      /* first log, then stderr; progress messages don't go to log */
      if (NULL != this_pass_file && (second_pass || !MSG_TYPE(MSG_PROGRESS)))
	{
	  va_list va;
	  if (!MSG_TYPE(MSG_PROGRESS) && progress_newline_open)
	    {
	      (void) fprintf (this_pass_file, "\n");
	      progress_newline_open = FALSE;
	    }
	  if (NULL == file && !MSG_TYPE (MSG_DEBUG) && !MSG_TYPE (MSG_PROGRESS) && !MSG_TYPE (MSG_MESSAGE))
	    (void) fprintf (this_pass_file, "%s: ", prog);
	  else if (NULL != file)
	    (void) fprintf (this_pass_file, "%s:%lu: ", file->name, (unsigned long)file->line);
/*lint -e737*/
	  va_start (va, fmt);
/*lint +e737*/
	  (void) vfprintf (this_pass_file, fmt, va);
	  va_end (va);
	  if (!MSG_TYPE (MSG_DEBUG) 
		&& !MSG_TYPE (MSG_PROGRESS) 
		&& !MSG_TYPE (MSG_MESSAGE)
		&& err_newline)
	    (void) fprintf (this_pass_file, "\n");
	}
      if (FALSE == second_pass)
	{
	  if (MSG_ISSET (MSG_STDERR))
	    {
	      second_pass = TRUE;
	      this_pass_file = messages_on_stdout ? stdout : stderr;
	      goto PASS_START;
	    }
	}
    }
  if (MSG_TYPE (MSG_FATAL))
    abort ();
  else if (MSG_TYPE (MSG_WARNING))
    {
      if (HISTORY_CLEAN == history)
        history = HISTORY_WARNING;
    }
  else if (MSG_TYPE (MSG_ERROR))
    {
      if (history < HISTORY_ERROR)
        history = HISTORY_ERROR;
    }
  if (exit_on_error)
    {
      if (MSG_TYPE (MSG_ERROR))
	exit (EXIT_ERROR);
      else if (MSG_TYPE (MSG_UERROR))
	usage();
    }
}

File *
ewfile (const char *fn, long ln)
{
  static File f;
  f.name = (char *)fn; /* this loss of const is safe: f.name will never be modified */
  f.line = ln;
  return &f;
}

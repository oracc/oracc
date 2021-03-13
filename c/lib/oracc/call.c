/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: call.c,v 0.6 1997/09/08 14:50:03 sjt Exp $
*/

#include <unistd.h>
#include <psd_base.h>
#include <vector.h>
#include <call.h>

int
call (const char *obligatory_arg, ...)
{
  int status, wait_res;
  char **call_vec = vec_create (), *tmp, *prog;
  va_list ap;
  char buf[1024];

  prog = (char*) obligatory_arg;
  if (!strcmp (prog, "SHELL"))
    {
      prog = getenv (CALL_OS_SPEC);
      if (NULL == prog)
	prog = CALL_OS_SHELL;
      call_vec = vec_add (call_vec, prog);
      call_vec = vec_add (call_vec, CALL_OS_MINUSC);
      *buf = '\0';
      va_start(ap, obligatory_arg);
      while (NULL != (tmp = va_arg(ap, char*)))
	{
	  strcat (buf, tmp);
	  strcat (buf, " ");
	}
      va_end (ap);
      call_vec = vec_add (call_vec, buf);
    }
  else
    {
      call_vec = vec_add (call_vec, prog);
      va_start(ap, obligatory_arg);
      while (NULL != (tmp = va_arg(ap, char*)))
	call_vec = vec_add (call_vec, tmp);
      va_end (ap);
    }

  if (MSG_ISSET(MSG_PROGRESS))
    {
      char **pp = call_vec;
      progress ("calling: ");
      while (NULL != *pp)
	{
	  progress ("%s ", *pp);
	  ++pp;
	}
      progress ("\n");
    }

#if CALL_WITH_FORK
  if (fork () == 0)
    execv (prog, (char *const*)call_vec);
  wait_res = wait (&status);
  /*  fprintf (stdout, "call: wait_res = %d; status = %d; SYS=%d; PRG=%d\n", 
	   wait_res, status, CALL_SYS_RET(status), CALL_PRG_RET(status)); */
#elif CALL_WITH_SPAWN
  status = spawnv (P_WAIT, prog, (const char *const*)call_vec);
  wait_res = 0;
#elif CALL_WITH_SYSTEM
  status = system (vec_to_str (call_vec, vec_len(call_vec), " "));
  wait_res = 0;
#endif

  if (-1 == wait_res || !WIFEXITED(status))
    return -1;
  else
    return WEXITSTATUS(status);
}

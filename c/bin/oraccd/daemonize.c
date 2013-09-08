/* Adapted from APUE 3rd ed. 427-428 */
#include <stdlib.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include "oraccd.h"

static void setlock(pid_t pid, struct oraccd_config *cfg);

void
daemonize(const char *cmd, struct oraccd_config *cfg)
{
  int			i, fd0, fd1, fd2;
  pid_t			pid;
  struct rlimit		rl;
  struct sigaction	sa;


  /*
   * Clear file creation mask.
   */
  umask(0);

  /*
   * Get maximum number of file descriptors.
   */
  if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    err_quit("%s: can't get file limit", cmd);

  /*
   * Become a session leader to lose controlling TTY.
   */
  if ((pid = fork()) < 0)
    err_quit("%s: can't fork", cmd);
  else if (pid != 0) /* parent */
    exit(0);
  setsid();

  /*
   * Ensure future opens won't allocate controlling TTYs.
   */
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGHUP, &sa, NULL) < 0)
    err_quit("%s: can't ignore SIGHUP");
  if ((pid = fork()) < 0)
    err_quit("%s: can't fork", cmd);
  else if (pid != 0) /* parent */
    exit(0);

  /*
   * Set the lock file
   */
  setlock(pid, cfg);

  /*
   * Change the current working directory to oracc home.
   */
  if (chdir(cfg->oracc_home) < 0)
    err_quit("%s: can't change directory to /");
	
  /*
   * Close all open file descriptors.
   */
  if (rl.rlim_max == RLIM_INFINITY)
    rl.rlim_max = 1024;
  for (i = 0; i < rl.rlim_max; i++)
    close(i);

  /*
   * Attach file descriptors 0, 1, and 2 to /dev/null.
   */
  fd0 = open("/dev/null", O_RDWR);
  fd1 = dup(0);
  fd2 = dup(0);

  /*
   * Initialize the log file.
   */
  openlog(cmd, LOG_CONS, LOG_DAEMON);
  if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
	   fd0, fd1, fd2);
    exit(1);
  }
}

static void
setlock(pid_t pid, struct oraccd_config *cfg)
{
  int lockfd = -1;
  const char *lockfile = NULL;
 
  if (cfg->oraccd_mode == ORACCD_BUILD)
    lockfile = "/tmp/oraccd-build.lock";
  else
    lockfile = "/tmp/oraccd-serve.lock";
  /* the lock file can only be accessed by the user who created it, normally the oracc user */
  lockfd = open(lockfile, O_RDWR|O_CREAT, 400);
  
  if (-1 != lockfd)
    {
      struct flock lock = cfg->lock_params;
      lock.l_pid = pid;
      if (-1 == fcntl(lockfd, F_SETLK, &lock))
	{
	  perror("oraccd");
	  exit(1);
	}
      else
	printf("oraccd: process %ld acquired on lockfile %s\n", (long)getpid(), lockfile);
    }
  else
    {
      printf("can't save lock to %s\n", lockfile);
      exit(1);
    }
}

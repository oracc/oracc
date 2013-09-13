/* Adapted from APUE 3rd ed. 427-428 */
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>

#include "oraccd.h"

static int lockfile(int);

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
#if 1
  /* Open a log file in write mode. */
  cfg->logfp = fopen("oraccd.log", "w+");
#else
  openlog(cmd, LOG_CONS, LOG_DAEMON);
#endif

  if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    fprintf(cfg->logfp, "unexpected file descriptors %d %d %d",
	   fd0, fd1, fd2);
    exit(1);
  }
}

#define LOCKFILE ((cfg->oraccd_mode == ORACCD_BUILD)?"/tmp/oraccd-build.pid":"/tmp/oraccd-serve.pid")
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int
already_running(struct oraccd_config *cfg)
{
  int   fd;
  char	buf[16];

  fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
  if (fd < 0) {
    fprintf(cfg->logfp, "can't open %s: %s", LOCKFILE, strerror(errno));
    exit(1);
  }
  if (lockfile(fd) < 0) {
    if (errno == EACCES || errno == EAGAIN) {
      close(fd);
      return(1);
    }
    fprintf(cfg->logfp, "can't lock %s: %s", LOCKFILE, strerror(errno));
    exit(1);
  }
  ftruncate(fd, 0);
  sprintf(buf, "%ld", (long)getpid());
  write(fd, buf, strlen(buf)+1);
  return(0);
}

static int
lockfile(int fd)
{
  struct flock fl;

  fl.l_type = F_WRLCK;
  fl.l_start = 0;
  fl.l_whence = SEEK_SET;
  fl.l_len = 0;
  return(fcntl(fd, F_SETLK, &fl));
}

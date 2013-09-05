#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */

enum e_oraccd_mode { ORACCD_NONE, ORACCD_BUILD, ORACCD_CLEAN, ORACCD_SERVE, ORACCD_STATUS };
enum e_oraccd_state { ORACCD_NOT , ORACCD_RUNNING, ORACCD_STALE_LOCK, ORACCD_NOPERMS, ORACCD_ERR };

static enum e_oraccd_mode oraccd_mode = ORACCD_NONE;

static void q(void);
static void setlock(pid_t pid);
static void status(void);
static void usage(void);

int
main(int argc, char* argv[])
{
  FILE *fp= NULL;
  lock_t process_id = 0, lock;
  lock_t sid = 0;
  unsigned long iteration = 0;
  struct sigaction sa;

  if (argv[1])
    {
      if (!strcmp(argv[1], "build"))
	{
	  switch (check_oraccd(ORACCD_BUILD))
	    {
	    case ORACDD_NOT:
	      oraccd_mode = ORACCD_BUILD;
	      build();
	      break;
	    case ORACCD_RUNNING:
	      fprintf(stderr, "oraccd is already running in BUILD mode\n");
	      exit(1);
	    case ORACCD_STALE_LOCK:
	      fprintf(stderr, "oraccd has a stale lock on BUILD mode--run oraccd clean\n");
	      exit(1);
	    case ORACCD_NOPERMS:
	      fprintf(stderr, "oraccd: you don't have permission to access oraccd lock files\n");
	      exit(1);
	    case ORACCD_ERR:
	      fprintf(stderr, "oraccd: error reading /tmp/oraccd-build.lock\n");
	      exit(1);
	    default:
	      fprintf(stderr, "oraccd: unknown result from check_oraccd()\n");
	      exit(1);
	      break;
	    }
	}
      else if (!strcmp(argv[1], "clean"))
	{
	  oraccd_mode = ORACCD_CLEAN;
	  clean();
	  exit();
	}
      else if (!strcmp(argv[1], "serve"))
	{
	  switch (check_oraccd(ORACCD_SERVE))
	    {
	    case ORACDD_NOT:
	      oraccd_mode = ORACCD_SERVE;
	      serve();
	      break;
	    case ORACCD_RUNNING:
	      fprintf(stderr, "oraccd is already running in BUILD mode\n");
	      exit(1);
	    case ORACCD_STALE_LOCK:
	      fprintf(stderr, "oraccd has a stale lock on SERVE mode--run oraccd clean\n");
	      exit(1);
	    case ORACCD_NOPERMS:
	      fprintf(stderr, "oraccd: you don't have permission to access oraccd lock files\n");
	      exit(1);
	    case ORACCD_ERR:
	      fprintf(stderr, "oraccd: error reading /tmp/oraccd-serve.lock\n");
	      exit(1);
	    default:
	      fprintf(stderr, "oraccd: unknown result from check_oraccd()\n");
	      exit(1);
	      break;
	    }
	}
      else if (!strcmp(argv[1], "status"))
	{
	  oraccd_mode = ORACCD_STATUS;
	  status();
	  exit(0);
	}
      else
	{
	  usage();
	  exit(1);
	}
    }
  else
    {
      usage();
      exit(1);
    }

  /* Create child process */
  process_id = fork();

  /* Indication of fork() failure */
  if (process_id < 0)
    {
      printf("fork failed!\n");
      /* Return failure in exit status */
      exit(1);
    }

  /* PARENT PROCESS. Need to kill it. */
  if (process_id > 0)
    {
      setlock(process_id);
      printf("process_id of child process %d \n", process_id);
      /* return success in exit status */
      exit(0);
    }

  /* unmask the file mode */
  umask(0);

  /* set new session */
  sid = setsid();
  if(sid < 0)
    {
      /* Return failure */
      exit(1);
    }

  /* Change the current working directory to Oracc home. */
  if (chdir("/home/oracc"))
    {
      if (chdir("/Users/oracc"))
	{
	  printf("can't chdir to /home/oracc or /Users/oracc\n");
	  exit(1);
	}
    }

  /* Close stdin, stdout and stderr. */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  /* Open a log file in write mode. */
  fp = fopen ("oraccd.log", "w+");
  while (1)
    {
      /* Don't block context switches, let the process sleep for some time. */
      sleep(1);
      fprintf(fp, "oraccd iteration %lu ...\n", iteration++);
      fflush(fp);
      /* Implement and call some function that does core work for this daemon. */
      q();
    }
  fclose(fp);
  return (0);
}

static enum e_oraccd_status
check_oraccd(enum e_oraccd_mode m)
{
  FILE *lockfp = NULL;
  const char *lockfile = NULL;
  struct stat sb;
  if (m == ORACCD_BUILD)
    {
      lockfile = "/tmp/oraccd-build.lock";
    }
  else if (m == ORACCD_SERVE)
    {
      lockfile = "/tmp/oraccd-serve.lock";
    }
  else
    {
      fprintf(stderr, "oraccd: unknown mode in check request\n");
      exit(1);
    }
  if ((lockfp = fopen(lockfile, "r")))
    {
      enum e_oraccd_status s;
      if (flock(fileno(lockfp)))
	s = ORACCD_RUNNING;
      else
	s = ORACCD_STALE_LOCK;
      fclose(lockfp);
      return s;
    }
  else
    {
      if (!stat(lockfile, &sb))
	return ORACCD_NOPERMS;
      else
	return ORACCD_NOT;
    }
}

static void
q(void)
{
  /* read new messages into list */
  read_q();

  /* check to see if any current builds have finished */
  check_builds();

  /* process list to see if requirements are met */
  process_q();
}

static void
read_q(void)
{
  
}

static void
setlock(pid_t pid)
{
  FILE *lockfp = NULL;
  const char *lockfile = NULL;
  if (oraccd_mode == ORACCD_BUILD)
    lockfile = "/tmp/oraccd-build.lock";
  else
    lockfile = "/tmp/oraccd-serve.lock";
  lockfp = fopen(lockfile, "w");
  if (lockfp)
    {
      fprintf(lockfp, "%lu", pid);
      fflush(lockfp);
      if (flock(fileno(lockfp), LOCK_EX|LOCK_NB))
	{
	  perror("oraccd");
	  exit(1);
	}
    }
  else
    {
      printf("can't save lock to %s\n", lockfile);
      exit(1);
    }
}

static void
status(void)
{
  switch(check_oraccd(ORACCD_BUILD))
    {
    case ORACDD_NOT:
      fprintf(stderr, "oraccd is NOT running in BUILD mode\n");
      break;
    case ORACCD_RUNNING:
      fprintf(stderr, "oraccd is running in BUILD mode\n");
      break;
    case ORACCD_STALE_LOCK:
      fprintf(stderr, "oraccd has a stale lock on BUILD mode--run oraccd clean\n");
      break;
    default:
      fprintf(stderr, "oraccd: unknown result from check_oraccd()\n");
      exit(1);
      break;
   }
  switch(check_oraccd(ORACCD_SERVE))
    {
    case ORACDD_NOT:
      fprintf(stderr, "oraccd is NOT running in SERVE mode\n");
      break;
    case ORACCD_RUNNING:
      fprintf(stderr, "oraccd is running in SERVE mode\n");
      break;
    case ORACCD_STALE_LOCK:
      fprintf(stderr, "oraccd has a stale lock on SERVE mode--run oraccd clean\n");
      break;
    default:
      fprintf(stderr, "oraccd: unknown result from check_oraccd()\n");
      exit(1);
      break;
    }
}

static void
usage(void)
{
  fprintf(stderr, 
	  "oraccd usage:\n"
	  "  oraccd status : query oraccd status\n"
	  "  oraccd build  : run oraccd in build mode\n"
	  "  oraccd serve  : run oraccd in serve mode\n");
}

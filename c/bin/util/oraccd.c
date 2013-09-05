#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

enum e_oraccd_mode { ORACCD_NONE, ORACCD_BUILD, ORACCD_CLEAN, ORACCD_SERVE, ORACCD_STATUS };
enum e_oraccd_status { ORACCD_NOT , ORACCD_RUNNING, ORACCD_STALE_LOCK, ORACCD_NOPERMS, ORACCD_ERR };

static enum e_oraccd_mode oraccd_mode = ORACCD_NONE;
static enum e_oraccd_status check_oraccd(enum e_oraccd_mode m);

static struct flock lock_params;

static void build(void);
static void clean(void);
static const char *lowercase(const char *s);
static void q(void);
static void read_q(void);
static void serve(void);
static void setlock(pid_t pid);
static void status(void);
static enum e_oraccd_status status_build(int verbose);
static enum e_oraccd_status status_generic(int verbose, enum e_oraccd_status estate, const char *sstate);
static enum e_oraccd_status status_serve(int verbose);
static void usage(void);

int
main(int argc, char* argv[])
{
  FILE *fp= NULL;
  pid_t process_id = 0;
  pid_t sid = 0;
  unsigned long iteration = 0;

  lock_params.l_type = F_WRLCK; /* exclusive lock */
  lock_params.l_whence = SEEK_SET;
  lock_params.l_start = 0; /* on entire file */
  lock_params.l_len = 0;

  if (argv[1])
    {
      if (!strcmp(argv[1], "build"))
	{
	  if (ORACCD_NOT == status_build(0))
	    build();
	  else
	    exit(1);
	}
      else if (!strcmp(argv[1], "clean"))
	{
	  oraccd_mode = ORACCD_CLEAN;
	  clean();
	  exit(0);
	}
      else if (!strcmp(argv[1], "serve"))
	{
	  if (ORACCD_NOT == status_serve(0))
	    serve();
	  else
	    exit(1);
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
      printf("process_id of child process %d \n", process_id);
      /* return success in exit status */
      exit(0);
    }

  setlock(process_id);

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
  fp = fopen("oraccd.log", "w+");
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

static void
build(void)
{
  oraccd_mode = ORACCD_BUILD;
}

static void
check_builds(void)
{
}

static enum e_oraccd_status
check_oraccd(enum e_oraccd_mode m)
{
  int lockfd = -1;
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
  printf("oraccd: attempting to acquire lock on lockfile %s\n", lockfile);
  lockfd = open(lockfile, O_RDWR);
  if (-1 != lockfd)
    {
      enum e_oraccd_status s;
      struct flock lock = lock_params;
      lock.l_pid = -1;
      
      if (-1 == fcntl(lockfd, F_GETLK, &lock))
	{
	  perror("oraccd error checking lockfile");
	  s = ORACCD_ERR;
	}
      else
	{
	  printf("oraccd: pid %lu checked %s and found pid %lu\n", (unsigned long)getpid(), lockfile, (unsigned long)lock.l_pid);
	  if (-1 != lock.l_pid)
	    {
	      lock_params.l_pid = lock.l_pid;
	      s = ORACCD_RUNNING;
	    }
	  else
	    {
	      printf("oraccd: check_oraccd successfully locked lockfile %s\n", lockfile);
	      s = ORACCD_STALE_LOCK;
	    }
	}
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
clean(void)
{
  unlink("/tmp/oraccd-build.lock");
  unlink("/tmp/oraccd-serve.lock");
}

static const char *
lowercase(const char *s)
{
  char *l = malloc(strlen(s)+1), *e;
  e = l;
  while (*s)
    *e++ = *s++;
  *e = '\0';
  return l;
}

static void
process_q(void)
{
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
serve(void)
{
  oraccd_mode = ORACCD_SERVE;
}

static void
setlock(pid_t pid)
{
  int lockfd = -1;
  const char *lockfile = NULL;
 
  if (oraccd_mode == ORACCD_BUILD)
    lockfile = "/tmp/oraccd-build.lock";
  else
    lockfile = "/tmp/oraccd-serve.lock";
  /* the lock file can only be accessed by the user who created it, normally the oracc user */
  lockfd = open(lockfile, O_RDWR|O_CREAT, 400);
  
  if (-1 != lockfd)
    {
      struct flock lock = lock_params;
      lock.l_pid = pid;
      if (-1 == fcntl(lockfd, F_SETLK, &lock))
	{
	  perror("oraccd");
	  exit(1);
	}
      else
	printf("oraccd: process %lu acquired on lockfile %s\n", (unsigned long)getpid(), lockfile);
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
  status_build(1);
  status_serve(1);
}

static enum e_oraccd_status
status_build(int verbose)
{
  return status_generic(verbose, ORACCD_BUILD, "BUILD");
}

static enum e_oraccd_status
status_serve(int verbose)
{
  return status_generic(verbose, ORACCD_SERVE, "SERVE");
}

static enum e_oraccd_status
status_generic(int verbose, enum e_oraccd_status estate, const char *sstate)
{
  enum e_oraccd_status s = check_oraccd(estate);
  const char *slower = lowercase(sstate);
  printf("oraccd: check_oraccd() == %d\n", s);
  switch (s)
    {
    case ORACCD_NOT:
      if (verbose)
	fprintf(stderr, "oraccd is not running in %s mode\n", sstate);
      break;
    case ORACCD_RUNNING:
      fprintf(stderr, "oraccd is already running in %s mode\n", sstate);
      break;
    case ORACCD_STALE_LOCK:
      fprintf(stderr, "oraccd has a stale lock on %s mode--run oraccd clean\n", sstate);
      break;
    case ORACCD_NOPERMS:
      fprintf(stderr, "oraccd: you don't have permission to access oraccd lock files\n");
      break;
    case ORACCD_ERR:
      fprintf(stderr, "oraccd: error reading /tmp/oraccd-%s.lock\n", slower);
      break;
    default:
      fprintf(stderr, "oraccd: unknown result from check_oraccd()\n");
      break;
    }
  return s;
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

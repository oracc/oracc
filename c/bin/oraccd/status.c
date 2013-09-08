#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#if 0
#include <dirent.h>
#include <errno.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include "oraccd.h"

static enum e_oraccd_status status_generic(int verbose, enum e_oraccd_status estate, const char *sstate, struct oraccd_config *cfg);

enum e_oraccd_status
status(struct oraccd_config *cfg)
{
  status_build(1, cfg);
  status_serve(1, cfg);
}

enum e_oraccd_status
status_build(int verbose, struct oraccd_config *cfg)
{
  return status_generic(verbose, ORACCD_BUILD, "BUILD", cfg);
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

static enum e_oraccd_status
status_check_lock(enum e_oraccd_mode m, struct oraccd_config *cfg)
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
      struct flock lock = cfg->lock_params;
      lock.l_pid = -1;
      
      if (-1 == fcntl(lockfd, F_GETLK, &lock))
	{
	  perror("oraccd error checking lockfile");
	  s = ORACCD_ERR;
	}
      else
	{
	  printf("oraccd: pid %ld checked %s and found pid %ld\n", (long)getpid(), lockfile, (long)lock.l_pid);
	  if (-1 != lock.l_pid)
	    {
	      cfg->lock_params.l_pid = lock.l_pid;
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

enum e_oraccd_status
status_serve(int verbose, struct oraccd_config *cfg)
{
  return status_generic(verbose, ORACCD_SERVE, "SERVE", cfg);
}

static enum e_oraccd_status
status_generic(int verbose, enum e_oraccd_status estate, const char *sstate, struct oraccd_config *cfg)
{
  enum e_oraccd_status s = status_check_lock(estate, cfg);
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
  free((char *)slower);
  return s;
}


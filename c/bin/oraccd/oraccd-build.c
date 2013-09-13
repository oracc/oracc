#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "oraccd.h"

struct oraccd_config config_info;
struct q *qhead = NULL, *qlast = NULL;

static void a_preview(struct q *qp);
static void a_publish(struct q *qp);
static void args(int argc, char **argv);
static void create_message(const char *path);
static void q(void);
static void ospawn(enum e_oraccd_proc proc, struct q *qp);
static int project_built(struct q *qp);
static int project_unpacked(struct q *qp);
static void usage(void);


#if 0
static enum e_oraccd_status check_oraccd(enum e_oraccd_mode m);
static void load_oraxxen(void);
static const char *lowercase(const char *s);
#endif

int
main(int argc, char* argv[])
{
  FILE *fp= NULL;
  pid_t process_id = 0;
  pid_t sid = 0;
  unsigned long iteration = 0;

  configure(&config_info);

  daemonize("oraccd", &config_info);

  if (already_running(&config_info)) {
    fprintf(config_info.logfp, "oraccd-build already running\n");
    exit(1);
  }

  while (1)
    {
      /* Don't block context switches, let the process sleep for some time. */
      sleep(1);
      fprintf(config_info.logfp, "oraccd iteration %lu ...\n", iteration++);
      fflush(config_info.logfp);
      /* Implement and call some function that does core work for this daemon. */
      q();
    }
  fclose(fp);

  return (0);
}

static void
a_preview(struct q *qp)
{
  if (!project_unpacked(qp))
    ospawn(ORACCD_PROC_UNPACK, qp);
  if (!project_built(qp))
    ospawn(ORACCD_PROC_BUILD, qp);
  else
    qp->status = Q_STATUS_DONE;
}

static void
a_publish(struct q *qp)
{
  char *action = NULL, *message = NULL;
  int i;
  if (!project_built(qp))
    {
      a_preview(qp);
      qp->status = Q_STATUS_PENDING;
    }
  ospawn(ORACCD_PROC_PACK, qp);
  qp->status = Q_STATUS_PENDING;
  action = malloc(8 + strlen(qp->project) + strlen(qp->version));
  sprintf(action, "serve-%s.%s", qp->project, qp->version);
  message = malloc(1 + config_info.oraxxen_max + strlen(action) + strlen(config_info.messages_dir));
  for (i = 0; config_info.oraxxen[i]; ++i)
    {
      sprintf(message, "%s/%s:%s", config_info.messages_dir, config_info.oraxxen[i], action);
      create_message(message);
    }
}

static void
create_message(const char *path)
{
  int touch = open(path, O_CREAT, 600);
  close(touch);
}

static void
ospawn(enum e_oraccd_proc proc, struct q *qp)
{
  pid_t pid;

  if ((pid = fork()) < 0)
    {
      perror("oraccd: fork error");
    }
  else if (pid == 0)
    {
      /* child */
      const char *argv[4];
      argv[0] = qp->action;
      argv[1] = qp->project;
      argv[2] = qp->version;
      argv[3] = NULL;
      execve("./oraccd-helper.sh", (char *const*)argv, NULL);
    }
  else
    {
      /* parent */
      /* do nothing ? */
    }
}

static void
process_q(void)
{
  struct q *qp = NULL;
  for (qp = qhead; qp; qp = qp->next)
    {
      /* only valid messages for this oraccd instance have been added to the queue */
      if (!strcmp(qp->action, "preview"))
	{
	  printf("oraccd: action=preview; project=%s; version=%s\n", qp->project, qp->version);
	  a_preview(qp);
	}
      else if (!strcmp(qp->action, "publish"))
	{
	  printf("oraccd: action=publish; project=%s; version=%s\n", qp->project, qp->version);
	  a_publish(qp);
	}
      else
	{
	  printf("oraccd: unknown action %s\n", qp->action);
	}
    }
}

static int
project_built(struct q *qp)
{
  return 0;
}

static int
project_unpacked(struct q *qp)
{
  return 0;
}

static void
q(void)
{
  read_q(&config_info);
  process_q();
}

static void
usage(void)
{
  fprintf(stderr, 
	  "oraccd usage:\n"
	  "  oraccd build  : run oraccd as daemon in build mode\n"
	  "  oraccd clean  : clean up stale lock files and exit\n"
	  "  oraccd serve  : run oraccd as daemon in serve mode\n"
	  "  oraccd status : query oraccd status and exit\n"
	  );
}

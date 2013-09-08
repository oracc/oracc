#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "oraccd.h"

static int q_add(const char *message, struct oraccd_config *cfg);
static void q_remove(struct q *rem);

static int
q_add(const char *message, struct oraccd_config *cfg)
{
  struct q *qp = NULL;
  char *ptr;

  /* if this message isn't for us, leave it */
  if ((ptr = strchr(message, ':')))
    {
      if (strncmp(message, cfg->hostname, ptr - message))
	return 0;
    }
  else
    {
      if (strcmp(cfg->hostname, "build"))
	return 0;
    }

  qp = malloc(sizeof(struct q));
  qp->message = malloc(strlen(message)+1);
  qp->splits = malloc(strlen(message)+1);
  strcpy(qp->message, message);
  strcpy(qp->splits, message);
  if ((ptr = strchr(qp->splits, ':')))
    {
      qp->host = qp->splits;
      *ptr++ = '\0';
      qp->action = ptr;
    }
  else
    {
      qp->host = "build";
      qp->action = qp->splits;
    }

  if ((ptr = strchr(qp->action, '-')))
    {
      *ptr++ = '\0';
      qp->project = ptr;
      if ((ptr = strchr(ptr, '.')))
	{
	  *ptr++ = '\0';
	  qp->version = ptr;
	  qlast->next = qp;
	  qp->prev = qlast;
	  if (!qhead)
	    qhead = qp;
	  qlast = qp;
	  qp->status = Q_STATUS_PENDING;
	  return 1;
	}
      else
	{
	  printf("malformed message--no period (expect [HOST:]ACTION-PROJECT.VERSION): %s\n", message);
	  free(qp->message);
	  free(qp);
	}
    }
  else
    {
      printf("malformed message--no hyphen (expect [HOST:]ACTION-PROJECT.VERSION) %s\n", message);
      free(qp->message);
      free(qp);
    }
  return 0;
}

static void
q_remove(struct q *rem)
{
  if (!rem->prev && !rem->next)
    {
      /* removing the only item in the list */
      qhead = qlast = NULL;
    }
  else
    {
      if (rem->prev)
	rem->prev->next = rem->next;
      if (rem->next)
	rem->next->prev = rem->prev;
    }
  free(rem->message);
  free(rem->splits);
  free(rem);
}

void
read_q(struct oraccd_config *cfg)
{
  DIR *dirp = NULL;
  struct dirent *entp = NULL;
  struct strlist *delenda, *last;

  if ((dirp = opendir(cfg->messages_dir)))
    {
      while (1)
	{
	  errno = 0;
	  if ((entp = readdir(dirp)))
	    {
	      printf("oraccd: readdir %s\n", entp->d_name);
	      if (q_add(entp->d_name, cfg))
		{
		  struct strlist *tmp = malloc(sizeof(struct strlist));
		  tmp->str = qlast->message;
		  tmp->next = NULL;
		  if (last)
		    {
		      last->next = tmp;
		      last = last->next;
		    }
		  else
		    delenda = last = tmp;
		}
	    }
	  else
	    {
	      if (!errno)
		{
		  closedir(dirp);
		  dirp = NULL;
		  return;
		}
	      else
		{
		  perror("oraccd readdir");
		  return;
		}
	    }
	}
      if (delenda)
	{
	  for (last = delenda; last; last = last->next)
	    unlink(last->str);
	}
    }
  else
    {
      perror("oraccd opendir");
      return;
    }
}

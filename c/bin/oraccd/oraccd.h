#ifndef ORACCD_H_
#define ORACCD_H_

#include <stdio.h>
#include <sys/file.h>

#define HOSTLEN		255
#define MAXLINE 4096                    /* max line length */
#define Q_STATUS_PENDING 	0
#define Q_STATUS_DONE 	 	1

enum e_oraccd_proc { ORACCD_PROC_BUILD , ORACCD_PROC_PACK, ORACCD_PROC_SERVE, ORACCD_PROC_UNPACK };
enum e_oraccd_mode { ORACCD_NONE, ORACCD_BUILD, ORACCD_CLEAN, ORACCD_SERVE, ORACCD_STATUS };
enum e_oraccd_status { ORACCD_NOT , ORACCD_RUNNING, ORACCD_STALE_LOCK, ORACCD_NOPERMS, ORACCD_ERR };

struct oraccd_config
{
  enum e_oraccd_mode oraccd_mode;
  char *oracc_home;
  const char *messages_dir;
  char hostname[HOSTLEN];
  struct flock lock_params;
  int max_concurrent_builds;
  char **oraxxen;
  int oraxxen_max;
  FILE *logfp;
};

extern struct oraccd_config config_info;

struct q
{
  char *message;
  char *splits;
  char *host;
  const char *action;
  const char *project;
  const char *version;
  int status;
  struct q *prev;
  struct q *next;
};

extern struct q *qhead, *qlast;

struct strlist
{
  char *str;
  struct strlist *next;
};

extern char hostname[];

int	already_running(struct oraccd_config *cfg);
void	clean(struct oraccd_config *cfg);
void	configure(struct oraccd_config *cfg);
void    daemonize(const char *, struct oraccd_config *);
void    err_dump(const char *, ...);
void    err_msg(const char *, ...);
void    err_quit(const char *, ...);
void    err_exit(int, const char *, ...);
void    err_ret(const char *, ...);
void    err_sys(const char *, ...);
void 	read_q(struct oraccd_config*);

#endif/*ORACCD_H*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ilem_form.h"
#include "lang.h"
#include "xli.h"
#include "xli_sux.c"

extern int verbose;
int xli_debug = 0;

struct xli xli_data[] = 
{
  { "sux" , "/usr/bin/perl", xli_sux_argp, NULL, NULL, xli_sux_handler, -1, -1 },
  { NULL, NULL, NULL, NULL, NULL, NULL, -1, -1 }
};

struct xli *xlem[c_count];
int xlem_tried[c_count];

struct xli*
xli_system(const char *lang)
{
  int i;
  for (i = 0; xli_data[i].lang; ++i)
    if (!strcmp(xli_data[i].lang, lang))
      return &xli_data[i];
  return NULL;
}

void
xli_init_external(struct xli*xlip)
{
  pid_t nPid;
  int pipeto[2];
  int pipefrom[2];
  if (verbose)
    xli_debug = 1;
  if (pipe(pipeto))
    {
      perror("xli_init: pipe() failed for output to xli");
      exit(255);
    }
  if (pipe(pipefrom))
    {
      perror("xli_init: pipe() failed for input from xli");
      perror( "pipe() from" );
      exit(255);
    }

  nPid = fork();
  if (nPid < 0)
    {
      perror("xli_init: fork() for xli exec failed");
      exit(255);
    }
  else if (nPid == 0)
    {
      dup2(pipeto[0], STDIN_FILENO);
      dup2(pipefrom[1], STDOUT_FILENO);
      close(pipeto[0]);
      close(pipeto[1]);
      close(pipefrom[0]);
      close(pipefrom[1]);
      execv(xlip->prog, (char **)xlip->argp);
      perror( "xli_init: execlp() failed to start xli");
      exit(255);
    }
  else
    {
      xlip->send_fd = pipeto[1];
      xlip->recv_fd = pipefrom[0];
      close(pipeto[0]);
      close(pipefrom[1]);
    }
}

void
xli_term_external(struct xli*xlip)
{
  xli_send(xlip,(unsigned char *)"");
  close(xlip->send_fd);
  close(xlip->recv_fd);
}

void
xli_term_internal(struct xli*xlip)
{
  xlip->term(xlip);
}

#define RDBUF_SIZ  256
unsigned char *
xli_recv(struct xli*xlip)
{
  unsigned char ch[1];
  static unsigned char buf[RDBUF_SIZ];
  int n = 0;
  while (read(xlip->recv_fd,ch,1))
    {
      if (*ch == '\n' || n == RDBUF_SIZ-2)
	break;
      else
	buf[n++] = ch[0];
    }
  buf[n] = '\0';
  if (xli_debug)
    fprintf(stderr,"recv: %s\n",*buf?buf:buf+1);
  return buf;
}

void
xli_send(struct xli*xlip, unsigned char *s)
{
  if (xli_debug)
    fprintf(stderr,"send: %s",s);
  write(xlip->send_fd, s, strlen((const char *)s));
}

void
xli_mapper(struct xcl_context *xcp, struct xcl_l *lp)
{
  enum langcode c = c_none;

  if (!xcp || !lp || !lp->f)
    return;

  if (!BIT_ISSET(lp->f->f2.flags /*lp->f->instance_flags*/, F2_FLAGS_LEM_NEW))
    return;

  if (!BIT_ISSET(lp->f->f2.flags, F2_FLAGS_NO_FORM)
      && lp->f->fcount > 0 
      && lp->f->finds[0]->f2.base && lp->f->finds[0]->f2.morph)
    return;

  c = lp->f->f2.core->code;
  
  if (c != c_none && xlem[c] && !lp->f->f2.morph)
    {
      struct xli *xlip = xlem[c];
      xlip->handler(xlip, lp, lp->f);
      if (lp->f->acount > 0)
	{
	  struct ilem_form *rover;
	  for (rover = lp->f->ambig; rover; rover = rover->ambig)
	    xlip->handler(xlip, lp, rover);
	}
    }
  
  if (!lp->f->f2.norm)
    lp->f->f2.norm = lp->f->f2.cf;
}

#ifdef MAIN
int
main(int argc, char **argv)
{
  unsigned char x[128];
  xli_init(&xli_data[0]);
  while (fgets((char *)x,126,stdin))
    {
      unsigned char *res;
      if (!*x)
	break;
      strcat((char*)x,"\n");
      xli_send(&xli_data[0], x);
      res = xli_recv(&xli_data[0], x);
      fprintf(stderr,"recv: %s\n",res);
    }
  xli_term(&xli_data[0]);
  return 0;
}
#endif

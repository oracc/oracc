#ifndef _XLI_H
#define _XLI_H

#include "lang.h"
#include "f2.h"
#include "xcl.h"

struct xli;
extern void xli_init_external(struct xli *xlip);
extern void xli_term_external(struct xli *xlip);
extern void xli_send(struct xli *xlip, unsigned char *s);
extern struct xli* xli_system(const char *lang);
extern unsigned char *xli_recv(struct xli *xlip);

typedef void xli_init(struct xli *xlip);
typedef void xli_term(struct xli *xlip);
typedef void xli_handler(struct xcl_context *xcp, struct xli *xlip, struct ilem_form *fp, struct f2 *f2p);

struct xli 
{
  const char *lang;
  const char *prog;
  const char * const*argp;
  xli_init *init;
  xli_term *term;
  xli_handler *handler;
  int send_fd, recv_fd;
};

extern struct xli *xlem[c_count];
extern int xlem_tried[c_count];
extern int lem_extended;
extern void xli_ilem(struct xcl_context *xcp, struct ilem_form *f, struct f2 *fp);
extern void xli_mapper(struct xcl_context *xcp, struct xcl_l *lp);
#endif/*_XLI_H*/

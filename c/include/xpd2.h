#ifndef XPD_H_
#define XPD_H_

#include "hash.h"
struct xpd
{
  const char *project;
  const char *file;
  struct npool *pool;
  Hash_table *opts; /* general options */
  Hash_table *lang; /* lang-options for lemmer */
};

extern const char*xpd_option(struct xpd *, const char*);
extern const char*xpd_lang(struct xpd *, const char*);
extern int xpd_option_int(struct xpd *, const char*);
extern struct xpd *xpd_init(const char *project, struct npool *pool);
extern void xpd_term(struct xpd *);
extern void xpd_echo(const char *xpd_proj,FILE *fp);

#endif/*XPD_H_*/

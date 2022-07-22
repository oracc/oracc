#ifndef _SIGOPS_H
#define _SIGOPS_H

#include <f2.h>
#include <sigs.h>

struct sigfile {
  const char *file;
  int lnum;
  unsigned char *ftext;
  struct sigdata *sigs;
  int nsigs;
  int sdp_next;
  int sdp_alloc;
  Hash_table *index;
  unsigned char **keys;
  int nkeys;
  struct npool *pool;
  struct sig_context *scp;
};

struct sigdata {
  struct f2 f2;
  unsigned char *sig; /* this points into the loaded file; use this to print original sig (without rank/count/insts) */
  unsigned char *sig_copy; /* and this is pool copied for f2_parse */
  unsigned const char *cgp_closed; /* should be freed after sigdata is done with */
  unsigned char *oid;
  int rank;
  int count;
  char *insts;
  int psu;
  int cof;
  struct sigfile *owner;
};

extern int sig_cgp;
extern int sig_group;

extern struct sig_context *global_scp;
extern void sigdump(struct sigfile *ssp);
extern void sigindex(struct sigfile *ssp);
extern struct sigfile * sigload(const char *file);

#endif

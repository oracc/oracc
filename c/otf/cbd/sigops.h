#ifndef _SIGOPS_H
#define _SIGOPT_H

#include <sigs.h>

struct sig {
  const char *file;
  int lnum;
  unsigned char *ftext;
  struct sigdata *sigs;
  ssize_t nsigs;
  int sdp_next;
  int sdp_alloc;
  Hash_table *index;
  struct sig_context *scp;
};

struct sigdata {
  struct f2 f2;
  unsigned char *sig;
  unsigned char *copy;
  unsigned char *closed;
  unsigned char *oid;
  int rank;
  int count;
  char *insts;
  int psu;
  int cof;
};

#endif

#ifndef __GX_H__
#define __GX_H__ 1

#undef warning
#include "warning.h"
#include "globals.h"
#undef ucc
#define ccp const char *
#define ucc unsigned const char *
#define ucp unsigned char *

struct header {
  unsigned const char *project;
  unsigned const char *lang;
  unsigned const char *name;
  unsigned const char *trans;
};

struct cbdtag {
  const char *name;
  int val;
};

extern const char *errmsg_fn;

extern struct cbdtag *cbdtags (const char *str, size_t len);
extern unsigned char **entry(unsigned char **ll);
extern unsigned char **header(unsigned char **ll);
extern int process_file(const char *fname);
extern unsigned char *tok(unsigned char *s, unsigned char *end);

#endif

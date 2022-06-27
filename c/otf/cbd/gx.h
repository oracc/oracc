#ifndef __GX_H__
#define __GX_H__ 1

#undef warning
#include "warning.h"
#include "globals.h"
#include "list.h"
#include "hash.h"
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

struct entry {
  unsigned const char *cf;
  unsigned const char *gw;
  unsigned const char *pos;
  unsigned const char *dcf;
  Hash_table *bases;
  List *forms;
  List *senses;
  List *aliases;
  List *bib;
  List *isslp;
  int plus;
  int usage;
  int compound;
  unsigned char *id;
};

struct sense {
  unsigned char *gw;
  unsigned char *pos;
  unsigned char *mng;
  unsigned char *lng;
  unsigned char *num;
  unsigned char *id;
};

struct isslp {
  unsigned char *year;
  unsigned char *text;
};

struct cbdpos {
  const char *name;
  int val;
};

struct cbdtag {
  const char *name;
  void (*parser)(unsigned char *s);
};

extern const char *errmsg_fn;

extern struct cbdtag *cbdtags (const char *str, size_t len);
extern unsigned char **entry(unsigned char **ll);
extern unsigned char **header(unsigned char **ll);
extern int process_file(const char *fname);
extern unsigned char *tok(unsigned char *s, unsigned char *end);

void parse_entry(unsigned char *s);
void parse_bases(unsigned char *s);
void parse_form(unsigned char *s);
void parse_sense(unsigned char *s);
void parse_alias(unsigned char *s);
void parse_allow(unsigned char *s);
void parse_inote(unsigned char *s);
void parse_isslp(unsigned char *s);
void parse_bff(unsigned char *s);
void parse_parts(unsigned char *s);
/*
void parse_(unsigned char *s);
*/
#endif

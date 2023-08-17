#ifndef PCRE2IF_H_
#define PCRE2IF_H_

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

struct pcre2if_m
{
  unsigned char *mstr;
  size_t off;
  size_t len;
};

extern pcre2_code *pcre2if_set_pattern(unsigned const char *pattern);
extern List *pcre2if_match(pcre2_code *re, const unsigned char *subject, int find_all, Pool *poolp, char sentinel);

#endif/*PCRE2IF_H_*/

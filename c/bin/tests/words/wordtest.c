#include <stdlib.h>
#include <stdio.h>
#include <psdtypes.h>
#include "words.h"

#define cuc(str) ((const unsigned char *)(str))

int
main()
{
  enum w2_match res;
  static struct w2_set *set1, *set2;
  const unsigned char *in = cuc("seed funnel");

  w2_init();
  set1 = w2_create_set(in);
  set2 = w2_create_set(cuc("funnel"));
  res = w2_subset(set1, set2);
  printf("`funnel' in `%s': result=%d\n",in,res);
  set2 = w2_create_set(cuc("seed funnel"));
  res = w2_subset(set1, set2);
  printf("`seed funnel' in `%s': result=%d\n",in,res);
  set2 = w2_create_set(cuc("seed plow"));
  res = w2_subset(set1, set2);
  printf("`seed plow' in `%s': result=%d\n",in,res);

  w2_term();

  return 0;
}

const char *prog = "wordtest";
int major_version = 0;
int minor_version = 1;
int verbose = 0;
void help() {}
void usage() {}

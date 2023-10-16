#include <stdio.h>

typedef unsigned int uint;

#include "gdlstate.h"

struct kv
{
  const char *k;
  const char *v;
};

union u {
  struct kv *kv;
  gdlstate_t st;
  void *vp;
};

gdlstate_t gst; /* global state */

int
main(int argc, char **argv)
{
  gdlstate_t s;
  unsigned int i;

  s = gs_reserved9;
  
#if 0
  s.damaged = SB_YE;
  s.stream = SS_S8;

  s.lang = GS_LANG_MAX;
  s.script = GS_SCRIPT_MAX;
#endif
  
  printf("sizeof(gdlstate_t) == %lu; sizeof(u) = %lu\n", sizeof(gdlstate_t), sizeof(union u));
  
  return 0;
}

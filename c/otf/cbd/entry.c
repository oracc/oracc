#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include "gx.h"

extern struct header *hdr;

unsigned char **
entry(unsigned char **ll)
{
  if (!strncmp((ccp)ll[0], "@entry", strlen("@entry")))
    {
      fprintf(stderr, "%s\n", (ccp)ll[0]);
      ++ll;
      ++lnum;
    }
  return ll;
}

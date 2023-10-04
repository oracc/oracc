#include <stdio.h>
#include <oraccsys.h>
const char *
itoa(int i)
{
  static char buf[11];
  sprintf(buf, "%d", i);
  return buf;
}

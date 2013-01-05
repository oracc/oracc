#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define TM_MAX 256

int
main(int argc, char **argv)
{
  char tmbuf[TM_MAX];
  time_t t = time(NULL);
  struct tm *gmt = gmtime(&t);
  if (argc > 1)
    strftime(tmbuf, TM_MAX, "%Y-%m-%dT%H:%M:%S+0000",gmt);
  else
    strftime(tmbuf, TM_MAX, "%Y-%m-%d",gmt);;
  fputs(tmbuf, stdout);
  return 0;
}

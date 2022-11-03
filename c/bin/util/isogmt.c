#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define TM_MAX 256

int
main(int argc, char **argv)
{
  char tmbuf[TM_MAX];
  struct tm *gmt;
  if (argc > 2) /* -f FILENAME */
    {
      struct stat st;
      if (!stat(argv[2], &st))
	{
	  gmt = gmtime(&st.st_mtimespec.tv_sec);
	}
      else
	{
	  fprintf(stderr, "isogmt: stat failed on %s\n", argv[2]);
	  exit(1);
	}
    }
  else
    {
      time_t t = time(NULL);
      gmt = gmtime(&t);
    }
  if (argc > 1)
    strftime(tmbuf, TM_MAX, "%Y-%m-%dT%H:%M:%S+0000",gmt);
  else
    strftime(tmbuf, TM_MAX, "%Y-%m-%d",gmt);;
  fputs(tmbuf, stdout);
  return 0;
}

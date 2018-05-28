/* ch12-glob.c --- demonstrate glob(). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>

char *myname;

/* globerr --- print error message for glob() */

int globerr(const char *path, int eerrno)
{
  fprintf(stderr, "%s: %s: %s\n", myname, path, strerror(eerrno));
  return 0;	/* let glob() keep going */
}

int glob_pattern(const char *pat, glob_t *gres)
{
  int flags = 0;
  int ret;

  /* flags |= (i > 1 ? GLOB_APPEND : 0); */
  ret = glob(pat, flags, globerr, gres);
  if (ret != 0) {
    if (ret != GLOB_NOMATCH)
      {
	fprintf(stderr, "%s: problem with %s (%s), stopping early\n",
		myname, pat,
		/* ugly: */	(ret == GLOB_ABORTED ? "filesystem problem" :
				 ret == GLOB_NOMATCH ? "no match of pattern" :
				 ret == GLOB_NOSPACE ? "no dynamic memory" :
				 "unknown problem"));
      }
    return 1;
  }
  return 0;
}

#ifdef MAIN
/* main() --- expand command-line wildcards and print results */

int main(int argc, char **argv)
{
  int i, ret;
  glob_t results;

  if (argc == 1) {
    fprintf(stderr, "usage: %s wildcard ...\n", argv[0]);
    exit(1);
  }

  myname = argv[0];	/* for globerr() */

#if 1
    ret = glob_pattern(argv[1], &results);
#else
  for (i = 1; i < argc; i++) {
    flags |= (i > 1 ? GLOB_APPEND : 0);
    ret = glob_pattern(argv[0], &results);
  }
#endif
  
  for (i = 0; i < results.gl_pathc; i++)
    printf("%s\n", results.gl_pathv[i]);

  globfree(& results);

  return 0;
}

#endif

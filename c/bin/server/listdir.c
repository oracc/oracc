#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAXLEN 1024

void
listdir(const char *arg)
{
  DIR *dirp;
  struct dirent *dp;
  char fname[MAXLEN];

  if ((dirp = opendir(arg)) == NULL) {
    fprintf(stderr, "listdir failed on %s\n", arg);
    perror("couldn't open directory");
    return;
  }
  
  do {
    errno = 0;
    if ((dp = readdir(dirp)) != NULL && '.' != *dp->d_name)
      {
	if ((strlen(arg) + strlen(dp->d_name) + 1) < MAXLEN)
	  {
	    struct stat s;
	    sprintf(fname, "%s/%s", arg, dp->d_name);
	    stat(fname, &s);
	    (void) printf("%s %ld\n", dp->d_name, s.st_mtime);
	  }
	else
	  {
	    fprintf(stderr, "filename %s/%s too long\n", arg, dp->d_name);
	    return;
	  }
      }
  } while (dp != NULL);
  
  if (errno != 0)
    perror("error reading directory");
  (void) closedir(dirp);
  return;
}

#if 0
int main(int argc, char *argv[])
{
  listdir(argv[1]);
  return (0);
}
#endif

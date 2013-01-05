/* Based on dirwalk from K&R, this is a very specialized program
   that searches the Oracc text hierarchy for files which need
   link processing; it does not process directories that have 
   dotted extensions, and it only looks in .xtf files for the
   attribute/value haslinks="1"
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>      /* flags for read and write */
#include <sys/types.h>  /* typedefs */
#include <sys/stat.h>   /* structure returned by stat */
#include <dirent.h>

#define MAX_PATH 1024
char parent_link_buf[MAX_PATH], child_link_buf[MAX_PATH];

void dirwalk(char *, void (*fcn)(char *));
void haslinks(char *);

int
main(int argc, char **argv)
{
  if (argc == 1)  /* default: current directory */
    haslinks(".");
  else
    while (--argc > 0)
      haslinks(*++argv);
  return 0;
}

void
haslinks(char *name)
{
  int len = strlen(name);
  if (len > 4 && name[len-4] == '.')
    {
      if (name[len-3] == 'x' && name[len-2] == 't' && name[len-1] == 'f')
	{
	  int fd = open(name,O_RDONLY);
	  if (fd >= 0)
	    {
	      static char buf[2048];
	      int nbytes = read(fd,buf,2047);
	      buf[nbytes] = '\0';
	      if (strstr(buf,"haslinks=\"1\""))
		puts(name);
	      close(fd);
	    }
	}
    }
  else
    {
      struct stat stbuf;
      lstat(name,&stbuf);
      if (S_ISLNK(stbuf.st_mode))
	{
	  int llen = readlink(name, child_link_buf,MAX_PATH-1);
	  child_link_buf[llen] = '\0';
	  if (!strcmp(child_link_buf,parent_link_buf))
	    ;
	  /* by design we do not handle more than one level of links */
	  else if (*parent_link_buf)
	    {
	      fprintf(stderr,"linkfiles: ignoring level 2 link %s\n",child_link_buf);
	      ;
	    }
	  else
	    {
	      strcpy(parent_link_buf,name);
	      dirwalk(name,haslinks);
	      *child_link_buf = *parent_link_buf = '\0';
	    }
	}
      else if (S_ISDIR(stbuf.st_mode))
	dirwalk(name, haslinks);
    }
}

/* dirwalk:  apply fcn to all files in dir */
void
dirwalk(char *dir, void (*fcn)(char *))
{
  char name[MAX_PATH];
  struct dirent *dp;
  DIR *dfd;
  if ((dfd = opendir(dir)) == NULL) {
    fprintf(stderr, "dirwalk: can't open %s\n", dir);
    return;
  }
  while ((dp = readdir(dfd)) != NULL) {
    if (!strcmp(dp->d_name, ".")
	|| !strcmp(dp->d_name, ".."))
      continue;    /* skip self and parent */
    if (strlen(dir)+strlen(dp->d_name)+2 > sizeof(name))
      fprintf(stderr, "dirwalk: name %s %s too long\n",
	      dir, dp->d_name);
    else {
      sprintf(name, "%s/%s", dir, dp->d_name);
      (*fcn)(name);
    }
  }
  closedir(dfd);
}

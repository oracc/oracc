#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define _MAX_PATH 2048

static char linebuf[_MAX_PATH], *line;

int
main(int argc, char**argv)
{
  line = linebuf;
  while ((line = fgets(line,_MAX_PATH,stdin)))
    {
      if (line[strlen(line)-1] == '\n')
	{
	  int fd;
	  line[strlen(line)-1] = '\0';
	  fd = open(line,O_RDONLY);
	  if (fd >= 0)
	    {
	      static char buf[1025];
	      int nbytes = read(fd,buf,1024);
	      buf[nbytes] = '\0';
	      if (strstr(buf,"haslinks=\"1\""))
		puts(line);
	      close(fd);
	    }
	}
      else
	{
	  fprintf(stderr,"expand: input line too long\n");
	  exit(1);
	}
    }
  return 0;
}

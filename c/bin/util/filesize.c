#include <stdio.h>
#include <sys/stat.h>

int
main(int argc, char **argv)
{
  struct stat buf;
  if (argc == 2)
    {
      if (!stat(argv[1], &buf))
	{
	  unsigned long l = (unsigned long)buf.st_size;
	  printf("%lu", l);
	  return 0;
	}
      else
	return 1;
    }
  else
    {
      fprintf(stderr, "filesize: must give filename as single argument\n");
      return 1;
    }
}

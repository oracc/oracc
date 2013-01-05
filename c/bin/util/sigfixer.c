#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

const char *xsltproc = "/usr/bin/xsltproc";

static char *
argv_cmd(char **argv)
{
  char *cmd = malloc(512);
  int i, len = 512;

  strcpy(cmd, xsltproc);
  for(i = 3; argv[i]; ++i)
    {
      while (strlen(cmd) + strlen(argv[i]) + 2 > len)
	cmd = realloc(cmd, len+=512);
      if (*cmd)
	strcat(cmd, " ");
      strcat(cmd, argv[i]);
    }
  return cmd;
}

int
main(int argc, char**argv)
{
  int ch = 0;
  const char *project = argv[1];
  FILE *in = stdin;
  int i;

  for (i = 0; argv[i]; ++i)
    fprintf(stderr, "%s ", argv[i]);
  fprintf(stderr, "\n");

  if (!project)
    {
      fprintf(stderr, "sigfixer: must give project on command line\n%c",ch);
      exit(1);
    }

  if (argv[2])
    {
      if (!strcmp(argv[2], "xsltproc"))
	{
	  char *xsltcommand = argv_cmd(argv);
	  fprintf(stderr, "sigfixer xsltcommand = %s\n", xsltcommand);
	  in = popen(xsltcommand, "r");
	}
      else
	{
	  in = fopen(argv[2], "r");
	}
    }

  while (EOF != (ch = fgetc(in)))
    {
      if ('p' == ch)
	if ('o' == (ch = fgetc(in)))
	  if ('p' == (ch = fgetc(in)))
	    if ('1' == (ch = fgetc(in)))
	      if ('s' == (ch = fgetc(in)))
		if ('i' == (ch = fgetc(in)))
		  if ('g' == (ch = fgetc(in)))
		    if ('(' == (ch = fgetc(in)))
		      printf("pop1sig('%s','',",project);
		    else
		      printf("pop1sig%c",ch);
		  else
		    printf("pop1si%c",ch);
		else
		  printf("pop1s%c",ch);
	      else
		printf("pop1%c",ch);
	    else
	      printf("pop%c",ch);
	  else
	    printf("po%c",ch);
	else
	  printf("p%c",ch);
      else
	putchar(ch);
    }
  return 0;
}

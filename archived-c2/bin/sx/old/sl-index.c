#include <stdio.h>
#include <stdlib.h>
#include <c2types.h>
#include <dbi.h>

int
main(int argc, char **argv)
{
  unsigned char *buf[20480], *k;
  char dbname[_MAX_PATH], dbdir[_MAX_PATH];
  int boot = 0;
  Dbi_index *dbi = NULL;
  FILE *infile;

  if (argc == 2)
    {
      if (!strcmp(argv[1], "-boot"))
	boot = 1;
    }
  else
    {
      fprintf(stderr, "sl-index.c: must give output DB name or -boot option on command line\n");
      exit(1);
    }

  if (boot)
    {
      strcpy(dbname, "ogsl");
      strcpy(dbdir, "/Users/stinney/orc/pub/ogsl/sl");
      if (!(infile = fopen("/Users/stinney/orc/pub/ogsl/sl/sl.tsv", "r")))
	{
	  fprintf(stderr, "sl-index.c: boot mode requires /Users/stinney/orc/pub/ogsl/sl.tsv\n");
	  exit(1);
	}
    }
  else
    {
      strcpy(dbname, argv[1]);
      strcpy(dbdir, "02pub/sl");
      infile = stdin;
    }

  dbi = dbi_create(dbname, dbdir, 0, 1, DBI_BALK);
  while ((k = (unsigned char *)fgets((char*)buf,20480,infile)))
    {
      unsigned char *v = (unsigned char*)strchr((char*)k,'\t');
      if (v)
	{
	  *v++ = '\0';
	  if (v[strlen((char*)v)-1] == '\n')
	    {
	      v[strlen((char*)v)-1] = '\0';
	      dbi_add(dbi,k,v,strlen((char*)v)+1);
	    }
	  else
	    {
	      fprintf(stderr,"sl-index.c: line too long.\n");
	      exit(1);
	    }
	}
      else
	{
	  fprintf(stderr, "sl-index.c: no tab in line\n");
	  exit(1);
	}
    }
  dbi_flush(dbi);
  return 0;
}

const char *prog = "sl-index";
int major_version = 1, minor_version = 0, verbose;
const char *usage_string = " -boot | [SLDB_NAME]";
int opts(int argc, char *arg){return 0;}
void help (void){}

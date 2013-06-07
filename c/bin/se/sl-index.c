#include <stdio.h>
#include <stdlib.h>
#include <psdtypes.h>
#include <dbi.h>

int
main(int argc, char **argv)
{
  unsigned char *buf[10240], *k, fname[512];
  const char *name = argv[1];
  Dbi_index *dbi = NULL;
  sprintf((char*)fname,"02pub/%s",name);
  dbi = dbi_create(name, "02pub", 0, 1, DBI_BALK);
  while ((k = (unsigned char *)fgets((char*)buf,10240,stdin)))
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
const char *usage_string = " [SLDB_NAME]";
int opts(int argc, char *arg){return 0;}
void help (void){}

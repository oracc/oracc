#include <stdio.h>
#include <unistd.h>
#include <psd_base.h>
#include "options.h"
#include "runexpat.h"
#include "xmlutil.h"
#include "atf.h"
#include "cdf.h"
#include "gdl.h"
#include "lang.h"
#include "pool.h"
#include "npool.h"
#include "gx.h"
#include "sx.h"

static FILE *f_sis = NULL, *f_tis = NULL;
static struct npool *pool = NULL;
static struct Hash_table *hash = NULL;
static const char *base = NULL;

static void
is_id_map(unsigned char *l)
{
  char *insts = strchr((ccp)l, '\t');
  static int id = 0;
  char id_buf[16], *idp;
  
  insts = strchr(insts+1, '\t');
  if (insts)
    {
      *insts++ = '\0';
      if (!(idp = hash_find(hash, (ucp)insts)))
	{
	  sprintf(id_buf, "r%06x", id++);
	  hash_add(hash, npool_copy((ucp)insts, pool), (idp = (char*)npool_copy((ucp)id_buf, pool)));
	}
      fprintf(f_sis, "%s\t%s\n", l, idp);
    }
  else
    fprintf(stderr, "statis: no insts in line\n");
}

static void
tis_print(const unsigned char *key, void*data)
{
  fprintf(f_tis, "%s\t%s\n", (char *)data, (char *)key);
}

int
main(int argc, char **argv)
{
  options(argc,argv,"b:");
  if (base)
    {
      char *sisfile = NULL;
      char *tisfile = NULL;
      unsigned char *l = NULL;

      pool = npool_init();
      hash = hash_create(1024);
      sisfile = malloc(strlen(base) + 5);
      tisfile = malloc(strlen(base) + 5);
      sprintf(sisfile, "%s.sis", base);
      sprintf(tisfile, "%s.tis", base);

      f_sis = xfopen(sisfile, "w");	
      while ((l = xgetline(stdin)))
	{
	  if ('@' == *l)
	    {
	      fputs((ccp)l, f_sis);
	      fputc('\n', f_sis);
	    }
	  
	  else
	    is_id_map(l);
	}
      xfclose(sisfile, f_sis);

      f_tis = xfopen(tisfile, "w");
      hash_exec2(hash, tis_print);
      xfclose(tisfile, f_tis);
    }
  else
    fprintf(stderr, "statis: must give -b BASENAME on command line\n");
  
  
  return 1;
}

int major_version = 1; int minor_version = 0;
const char *project = NULL;
const char *prog = "statis";
const char *usage_string = "";
void help() { ; }
int opts(int och,char *oarg)
{
  switch (och)
    {
    case 'b':
      base = optarg;
      break;
    default:
      return 1;
    }
  return 0;
}

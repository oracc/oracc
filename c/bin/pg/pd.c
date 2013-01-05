#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "psdtypes.h"
#include "messages.h"
#include "options.h"
#include "sortinfo.h"
#include "pg.h"

extern void pd_sort_cache(void);

int l2 = 1;

int csi_debug = 0;
extern int full_count_mode;
int with_counts = 0;
FILE *fdbg = NULL;
FILE *fpag = NULL;

struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

int nheadfields = 0;
int *headfields = NULL;

int page_n = 0;
extern int pagesize;
extern int use_linkmap;
extern int uzpage, zprev, znext;

int quick = 0;
int zoomid = -1;

/*const char *csiname = "/usr/local/oracc/www/cdli/sortinfo.csi";*/
const char *heading_keys = NULL, *outline_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *project = NULL;

extern unsigned char *loadfile(unsigned const char*,size_t*);
extern unsigned char *loadstdin(size_t*);

static int sk_lookup(const char *k);

#include "sk_lookup.c"
#include "pg_load.c"

static int *
set_keys(const char *s, int *nfields)
{
  int nkeys = 1, *fields;
  const char *t;
  
  if (!s || !strcmp(s,"none"))
    {
      *nfields = 0;
      return NULL;
    }

  t = s;
  while (*t)
    if (*t++ == ',')
      ++nkeys;
  fields = malloc(sizeof(int)*(nkeys+1));
  for(t = s, nkeys = 0; *t; ++nkeys)
    {
      if (isdigit(*t))
	fields[nkeys] = atoi(t);
      else
	fields[nkeys] = sk_lookup(t);
      while (*t && ',' != *t)
	++t;
      if (',' == *t)
	++t;
    }
  *nfields = nkeys;
  fields[nkeys] = -1;
  return fields;
}

int
main(int argc, char **argv)
{
  struct sortinfo *sip;
  struct item *items,**pitems;
  int nitems;
  int i, j;

  full_count_mode = 1;

  options(argc,argv,"cdl:p:");

  if (!project)
    project = "cdli";

  sort_keys = "period,designation";
  heading_keys = "period";

  if (NULL == fdbg)
    fdbg = stderr;
  if (!fpag)
    fpag = stdout;

  if (NULL == (sip = si_load_csi()))
    exit(1);
  
  if (heading_keys)
    headfields = set_keys(heading_keys, &nheadfields);
  
  items = pg_load(&nitems);

  if (csi_debug)
    {
      fprintf(stderr,"#initial item list: ID/group/skey\n");
      dbg_dump_items(items,nitems);
    }

  pitems = pg_sort(items, &nitems, sort_keys);
  if (csi_debug)
    {
      fprintf(stderr,"#post-sort item list: ID/group/skey\n");
      dbg_dump_pitems(pitems, nitems);
    }

  pd_sort_cache();

  for (i = j = 0; i < sic_size; ++i)
    {
      u4*pindex = sip->pindex + (sicache[i]->codes - sip->scodes);
      u4 poff = pindex[headfields[0]];
      int curr_group, init_j;
      fputs((char*)&sip->pool[poff], stdout);

      if (with_counts)
	{
	  for (init_j = j, curr_group = pitems[j]->grp; 
	       j < nitems && pitems[j]->grp == curr_group;
	       ++j)
	    ;
	  fprintf(stdout," [%d]", j - init_j);
	}

      if (i < (sic_size-1))
	fputs("; ",stdout);
    }

  exit(0);
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'c':
      with_counts = 1;
      break;
    case 'd':
      csi_debug = 1;
      fdbg = stderr;
      break;
    case 'l':
      listfile = arg;
      break;
    case 'p':
      project = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "pg";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
void
help ()
{

}

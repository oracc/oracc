/* Compile the sort info from sortinfo.tab into 
   an optimized form for runtime use */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include "psdtypes.h"
#include "messages.h"
#include "options.h"
#include "sortinfo.h"
#include "pg.h"
#include "npool.h"
#include "p2.h"

int l2 = 1, o2 = 0;

int csi_debug = 0;
FILE *fdbg = NULL;
FILE *fpag = NULL;

struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

struct npool *pg2_pool = NULL;

int nheadfields = 0;
int *headfields = NULL;

int page_n = 1;

int xml_outline = 0;

extern int pagesize;
extern int use_linkmap;
extern int uzpage, zprev, znext;

int quick = 0;
int zoomid = -1;

/*const char *csiname = "/usr/local/oracc/www/cdli/sortinfo.csi";*/
const char *heading_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *project = NULL;
const char *state = NULL;

const char *outline_keys[6];

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
  struct sortinfo *sip = NULL;
  struct item *items = NULL, **pitems = NULL;
  struct outline *op = NULL;
  struct page *pages = NULL;
  int nitems = 0, nlevels = 0, npages = 0;

  options(argc,argv,"02dg:h:l:mn:o:p:P:qs:S:xz:");

  if (!project)
    project = "cdli";

  if (NULL == fdbg)
    fdbg = stderr;
  if (!fpag)
    fpag = stdout;

  if (!quick)
    {
      struct p2_options *p2opts = NULL;
      char *tmp;
      pg2_pool = npool_init();
      p2opts = p2_load(project, state, pg2_pool);
      
      if (!sort_keys)
	sort_keys = (p2opts->sort_fields 
		     ? p2opts->sort_fields 
		     : "period,genre,provenience,designation");
      else
	sort_keys = p2_maybe_append_designation(sort_keys, pg2_pool);

      if (!heading_keys)
	{
	  heading_keys = (char *)npool_copy((unsigned char *)sort_keys, pg2_pool);
	  tmp = (char*)heading_keys + strlen(heading_keys) - strlen("designation");
	  *tmp = '\0';
	}
      
      if (NULL == (sip = si_load_csi()))
	exit(1);

      if (heading_keys)
	headfields = set_keys(heading_keys, &nheadfields);
    }

  items = pg_load(&nitems);

  if (csi_debug)
    {
      fprintf(stderr,"#initial item list: ID/group/skey\n");
      dbg_dump_items(items,nitems);
    }

  if (use_linkmap)
    {
      const char *dot;
      int with_word_id = 0;

      if ((dot = strchr((const char *)items[0].s,'.')) && strchr(dot+1,'.'))
	with_word_id = 1;

      if (dot)
	items = prune_p_with_q(items, &nitems, with_word_id);

      if (csi_debug)
	{
	  fprintf(stderr,"#post-prune-p item list: ID/group/skey\n");
	  dbg_dump_items(items,nitems);
	}
    }

  if (!quick)
    {
      pitems = pg_sort(items, &nitems, sort_keys);
      if (csi_debug)
	{
	  fprintf(stderr,"#post-sort item list: ID/group/skey\n");
	  dbg_dump_pitems(pitems, nitems);
	}
  
      op = pg_outline(&nlevels);
    }
  else
    {
      int i;
      pitems = malloc(nitems*sizeof(struct item*));
      for (i = 0; i < nitems; ++i)
	pitems[i] = &items[i];
      
    }

  if (nitems)
    pages = pg_page(pitems, nitems, &npages, op);

  if (!quick)
    {
      if (csi_debug) /* after pg_page to see page/count info */
	dbg_dump_outline(op,nlevels);
      
      if (xml_outline)
	{
	  char *ofname = malloc(strlen(listfile)+13), *slash;
	  FILE *ofile = NULL;
	  strcpy(ofname,listfile);
	  if ((slash = strrchr(ofname, '/')))
	    strcpy(slash+1, "outline.html");
	  else
	    strcpy(ofname, "outline.html");
	  if ((ofile = fopen(ofname, "w")))
	    pg_outline_dump(ofile,op,nlevels);
	  else
	    {
	      fprintf(stderr,"pg: xml outline file `%s' not writable\n", ofname);
	      exit(1);
	    }
	}
      else
	pg_outline_dump(fpag,op,nlevels);
    }
  else
    fputc('\n',fpag);

  if (xml_outline)
    {
      char *ofname = malloc(strlen(listfile)+13), *slash;
      strcpy(ofname,listfile);
      if ((slash = strrchr(ofname, '/')))
	strcpy(slash+1, "pg.xml");
      else
	strcpy(ofname, "pg.xml");
      if (!(fpag = fopen(ofname, "w")))
	{
	  fprintf(stderr,"pg: xml page file `%s' not writable\n", ofname);
	  exit(1);
	}
    }

  if (zoomid >= 0)
    pg_page_dump_zoomed(fpag,pitems,&nitems,&npages,zoomid,page_n);
  else if (!page_n)
    pg_page_dump_all(fpag,pages,npages);
  else if (page_n <= npages)
    pg_page_dump_one(fpag,&pages[page_n-1]);
  else if (nitems)
    {
      fprintf(stderr,"pg: page %d too big (max %d)\n", page_n, npages);
      exit(1);
    }

  fprintf(fpag,"pages %d\nitems %d\n", npages, nitems);
  if (zoomid >= 0)
    fprintf(fpag,"uzpage %d\nzprev %d\nznext %d\n", uzpage, zprev+1, znext+1);
  else
    fprintf(fpag,"uzpage 0\nzprev 0\nznext 0\n");
  exit(0);
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case '0':
      o2 = 1;
      break;
    case '2':
      l2 = 1;
      break;
#if 0
    case 'c':
      csiname = (const char *)arg;
      break;
#endif
    case 'd':
      csi_debug = 1;
      fdbg = stderr;
      break;
#if 0
      /* This option is bogus because heading_keys must always
	 be a subset of sort_keys in order to make sense, so it's
	 better to derive the heading_keys directly from sort_keys */
    case 'h':
      heading_keys = arg;
      break;
#endif
    case 'l':
      listfile = arg;
      break;
    case 'm':
      use_linkmap = 1;
      break;
    case 'n':
      page_n = atoi(arg);
      break;
    case 'o':
      {
	int o_index = atoi(arg);
	if (o_index > 0 && o_index <= 6)
	  {
	    outline_keys[o_index-1] = arg;
	  }
	else
	  {
	    fprintf(stderr,"pg: outline index must be between 1 and 6 inclusive\n");
	    exit(1);
	  }
      }
    case 'P':
      pagesize = atoi(arg);
      break;
    case 'p':
      project = arg;
      break;
    case 'q':
#if 1
      /* FIXME: need to decide whether to drop the quick mode 
	 altogether or ensure that XIS lists are correctly
	 sorted */
      quick = 1;
#endif
      break;
    case 's':
      sort_keys = arg;
      break;
    case 'S':
      state = arg;
      break;
    case 'x':
      xml_outline = 1;
      break;
    case 'z':
      zoomid = atoi(arg)-1; /* zoomid is now emitted as counting from 1 not 0 */
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

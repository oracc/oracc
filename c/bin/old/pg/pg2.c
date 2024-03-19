/* Compile the sort info from sortinfo.tab into 
   an optimized form for runtime use */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include "psdtypes.h"
#include "messages.h"
#include "options.h"
#include "hash.h"
#include "sortinfo.h"
#include "pg.h"
#include "npool.h"
#include "xpd2.h"
#include "p2.h"

int l2 = 1, o2 = 0, p3 = 0;

const char *config = NULL;
int csi_debug = 0;
int fragment = 0;
int ood_mode = 0;
int quiet = 0;
int tis_mode = 0;
FILE *fdbg = NULL;
FILE *fpag = NULL;
Hash_table *seen = NULL;

struct item *items = NULL;
int items_used = 0;
struct sortinfo *sip;

struct npool *pg2_pool = NULL;

int nheadfields = 0;
int *headfields = NULL;
const char *terminal_sort_field = "designation";

int page_n = 1, page_selector_index = 0, page_selector_page_n = 0;

int xml_outline = 0;

extern int pagesize;
extern int use_linkmap;
extern int uzpage, zprev, znext;

int quick = 0;
int zoomid = -1;

/*const char *csiname = "/home/oracc/www/cdli/sortinfo.csi";*/
const char *heading_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *page_selector_id = NULL;
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

  options(argc,argv,"023C:dg:fh:i:l:mn:o:p:P:qs:S:txz:");

  if (!project)
    project = "cdli";

  seen = hash_create(1024);

  if (strstr(project, "/ood/"))
    ood_mode = 1;

  if (NULL == fdbg)
    fdbg = stderr;
  if (!fpag)
    fpag = stdout;

  if (!ood_mode && !quick)
    {
      struct p2_options *p2opts = NULL;
      char *tmp;
      pg2_pool = npool_init();
      p2opts = p2_load(project, state, pg2_pool);
      
      if (!sort_keys)
	sort_keys = (p2opts->sort_fields 
		     ? p2opts->sort_fields 
		     : "period,genre,provenience"); /* p2_load does maybe_append */
      else
	sort_keys = p2_maybe_append_designation(sort_keys, pg2_pool, p2opts);

      if (!heading_keys)
	{
	  heading_keys = (char *)npool_copy((unsigned char *)sort_keys, pg2_pool);
	  tmp = (char*)heading_keys + strlen(heading_keys) - strlen(p2opts->sort_final);
	  *tmp = '\0';
	}
      
      if (!ood_mode)
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

  if (!ood_mode && !quick)
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

  if (!tis_mode)
    {
      if (!ood_mode && !quick)
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
		  fprintf(stderr,"pg2: xml outline file `%s' not writable\n", ofname);
		  exit(1);
		}
	    }
	  else
	    pg_outline_dump(fpag,op,nlevels);
	}
      else
	fputc('\n',fpag);
    }

  if (xml_outline)
    {
      char *ofname = malloc(strlen(listfile)+13), *slash;
      strcpy(ofname,listfile);
      if ((slash = strrchr(ofname, '/')))
	strcpy(slash+1, "pg.info");
      else
	strcpy(ofname, "pg.info");
      if (!(fpag = fopen(ofname, "w")))
	{
	  fprintf(stderr,"pg2: info page file `%s' not writable\n", ofname);
	  exit(1);
	}
    }

  if (zoomid >= 0)
    {
      pg_page_dump_zoomed(fpag,pitems,&nitems,&npages,zoomid,page_n,page_selector_id);
    }
  else 
    {
      if (tis_mode)
	{
	  int i;
	  struct page *p = &pages[0];

	  for (i = 0; i < p->used; ++i)
	    {
	      char *s = p->p[i];
	      if ('#' == *s)
		{
		  while (*s)
		    {
		      if ('#' == *s)
			{
			  if (i)
			    fputc('\n', fpag);
			}
		      else if ('_' == *s)
			fputc(' ', fpag);
		      else
			fputc(*s, fpag);
		      ++s;
		    }
		  fputc('\t', fpag);
		}
	      else
		fprintf(fpag,"%s ", p->p[i]);
	    }
	  fputc('\n',fpag);
	}
      else
	{
	  if (page_selector_id)
	    page_n = pg_find_page_with_id(pages,npages,page_selector_id);
	  if (!page_n)
	    pg_page_dump_all(fpag,pages,npages);
	  else if (page_n <= npages)
	    pg_page_dump_one(fpag,&pages[page_n-1]);
	  else if (nitems)
	    {
	      fprintf(stderr,"pg2: page %d too big (max %d)\n", page_n, npages);
	      exit(1);
	    }
	}
    }

  if (!tis_mode)
    {
      fprintf(fpag,"pages %d\nitems %d\n", npages, nitems);
      if (zoomid >= 0)
	fprintf(fpag,"uzpage %d\nzprev %d\nznext %d\n", uzpage, zprev+1, znext+1);
      else
	fprintf(fpag,"uzpage 0\nzprev 0\nznext 0\n");
      if (page_selector_id)
	{
	  fprintf(fpag, "page_selector_index %d\n", page_selector_index);
	  fprintf(fpag, "page_selector_page_n %d\n", page_selector_page_n);
	}
    }
  fclose(fpag);

  hash_free2(seen, free, NULL);

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
    case '3':
      p3 = 1;
      break;
#if 0
    case 'c':
      csiname = (const char *)arg;
      break;
#endif
    case 'C':
      config = arg;
      xpd_set_configname(config);
      break;
    case 'd':
      csi_debug = 1;
      fdbg = stderr;
      break;
    case 'f':
      fragment = 1;
      break;
#if 0
      /* This option is bogus because heading_keys must always
	 be a subset of sort_keys in order to make sense, so it's
	 better to derive the heading_keys directly from sort_keys */
    case 'h':
      heading_keys = arg;
      break;
#endif

    case 'i':
      page_selector_id = arg;
      break;

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
	    fprintf(stderr,"pg2: outline index must be between 1 and 6 inclusive\n");
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
      quiet = 1;
      break;
    case 'Q':
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
    case 't':
      tis_mode = 1;
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

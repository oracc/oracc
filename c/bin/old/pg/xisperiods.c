#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype128.h>
#include "psdtypes.h"
#include "messages.h"
#include "list.h"
#include "hash.h"
#include "npool.h"
#include "options.h"
#include "runexpat.h"
#include "sortinfo.h"
#include "pg.h"
#include "periodmap.h"

const char *xis_uri = "http://oracc.org/ns/xis/1.0";
const char *xisfile = NULL;
FILE *xisfp = NULL, *subxisfp = NULL;

Hash_table *seen = NULL;

extern void pd_sort_cache(void);

int period_index = 0;
int pmap_counts[PCODE_MAX];
int *pmaptab;

const char *curr_ref = NULL;
const char *tis_file = NULL;
int percent = 0;
int csi_debug = 0;
extern int full_count_mode;
int with_counts = 0;
FILE *fdbg = NULL;
FILE *fpag = NULL;
FILE *tis_fp = NULL;

int nheadfields = 0;
int *headfields = NULL;

int page_n = 0;
extern int pagesize;
extern int use_linkmap;
extern int uzpage, zprev, znext;

int quick = 0;
int zoomid = -1;

struct sortinfo *sip;
struct item *items,**pitems;
int nitems;

struct npool *r_pool = NULL;
List *r_list = NULL;


/*const char *csiname = "/usr/local/oracc/www/cdli/sortinfo.csi";*/
const char *heading_keys = NULL, *outline_keys = NULL, *sort_keys = NULL;
const char *listfile = NULL;
const char *project = NULL;

extern unsigned char *loadfile(unsigned const char*,size_t*);
extern unsigned char *loadstdin(size_t*);

static int sk_lookup(const char *k);

#include "sk_lookup.c"

static int
ipct(int amount, int total)
{
  if (total) {
    double pct = 100 * ((double)amount/(double)total);
    return (int)(pct+.5);
  } else {
    return 0;
  }
}

#if 0
static unsigned char *
adjust_s(unsigned char *stop, unsigned char *s)
{
  unsigned char *t = s;
  while (t > stop && isspace(t[-1]))
    --t;
  if (isspace(*t))
    *t = '\0';
  while (isspace(s[1]))
    ++s;
  return s;
}
#endif

static int
is_lang_id(const char *s)
{
  while (isalnum(*s) || '-' == *s)
    ++s;
  if (*s == '.')
    ++s;
  return 'x' == *s;
}


const char *
next_xis_id(const char *xis_id)
{
  static int subid = 0;
  if (xis_id)
    {
      static char subid_buf[64];
      sprintf(subid_buf, "%s.p.s%03d",xis_id,subid++);
      return subid_buf;
    }
  else
    {
      subid = 0;
      return NULL;
    }
}

const char *
sub_xis(int count, int start, struct item **items, const char *xis_id)
{
  int i;
  const char *ret_id = next_xis_id(xis_id);
  fprintf(subxisfp, "<xis xml:id=\"%s\" efreq=\"%d\">", ret_id, count);
  for (i = 0; i < count; ++i)
    fprintf(subxisfp, "<r>%s</r>", items[start+i]->s);
  fputs("</xis>", subxisfp);
  fprintf(tis_fp, "%s\t%d\t", ret_id, count);
  for (i = 0; i < count; ++i)
    {
      if (i)
	fputc(' ', tis_fp);
      fputs((const char *)items[start+i]->s, tis_fp);
    }
  fputs("\n", tis_fp);
  return ret_id;
}

void
xis_dump(void)
{
  unsigned char *s;
  int items_used = 0, i, j;

  if (!r_list)
    return;

  /* we know that each entry takes up at least 8 bytes */
  items = malloc((list_len(r_list) + 1) * sizeof(struct item));
  for (s = list_first(r_list); s; s = list_next(r_list))
    {
      char *colon = NULL, *dot;
      unsigned char *orig_s = s;

      /* if the list is qualified, pg can ignore the
	 project parts and just look up the items by
	 ID because it is only using the project's
	 own sortinfo */
      colon = strchr((char*)s,':');
      if (colon)
	{
	  orig_s = s;
	  s = (unsigned char *)++colon;
	}
    
      if (*s == 'P' || *s == 'Q' || *s == 'X' || is_lang_id((char *)s))
	{
	  items[items_used].s = orig_s;
	  while (*s && '\n' != *s)
	    ++s;
	  /* s = adjust_s(buf,s); */
	  *s++ = '\0';
	  items[items_used].pq = (unsigned char*)strdup(colon 
							? colon
							: (char*)items[items_used].s);
	  if ((dot = strchr((const char *)items[items_used].pq,'.')))
	    {
	      *dot++ = '\0';
	      items[items_used].lkey = atoi(dot);
	      /* fprintf(stderr,"scanned lkey value %d from %s\n",items[items_used].lkey,s); */
	    }
	  else
	    items[items_used].lkey = 0;
	  ++items_used;
	}
      else
	{
	  fprintf(stderr,"pg: bad list entry `%s'\n",s);
	  exit(1);
	}
    }

  items[items_used].s = NULL;
  items[items_used].pq = NULL;
  nitems = items_used;

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

  memset(pmap_counts, '\0', PCODE_MAX*sizeof(int));
  
  (void)next_xis_id(NULL);

  fprintf(xisfp, "<periods xml:id=\"%s.p\">", curr_ref);
  for (i = j = 0; i < sic_size; ++i)
    {
      s4*sindex = sip->scodes + (sicache[i]->codes - sip->scodes);
      u4*pindex = sip->pindex + (sicache[i]->codes - sip->scodes);
      u4 poff = pindex[headfields[0]];
      int curr_group, init_j;
      int count = 0;
      const char *sub_xis_id = NULL;

      for (init_j = j, curr_group = pitems[j]->grp; 
	   j < nitems && pitems[j]->grp == curr_group;
	   ++j)
	;
      count = j - init_j;

      if (count)
	sub_xis_id = sub_xis(count, init_j, pitems, curr_ref);

      pmap_counts[pmaptab[sindex[headfields[0]]]] += count;

      fprintf(xisfp, 
	      "<p icount=\"%d\" ipct=\"%d\" xis=\"%s\">%s</p>", 
	      count, ipct(count,percent), sub_xis_id, (char*)&sip->pool[poff]);
#if 0
      if (i < (sic_size-1))
	fputs("; ",stdout);
#endif
    }
  fprintf(xisfp, "</periods><instbar xml:id=\"%s.i\">", curr_ref);
  for (i = 0; i < PCODE_MAX; ++i) {
    fprintf(xisfp, "<i>%d</i>", pmap_counts[i]);
  }
  fprintf(xisfp, "</instbar>");
}

static void
sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name, "http://oracc.org/ns/xis/1.0:xis"))
    {
      curr_ref = get_xml_id(atts);
      percent = atoi(findAttr(atts, "efreq"));
      r_list = list_create(LIST_SINGLE);
      r_pool = npool_init();
    }
}

static void
eH(void *userData, const char *name)
{
  if (!strcmp(name, "http://oracc.org/ns/xis/1.0:r"))
    list_add(r_list, npool_copy((unsigned char*)charData_retrieve(), r_pool));
  else if (!strcmp(name, "http://oracc.org/ns/xis/1.0:xis"))
    {
      xis_dump();
      list_free(r_list, NULL);
      r_list = NULL;
      npool_term(r_pool);
      r_pool = NULL;
    }
}

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

      if (!strncmp(t,"period",6))
	period_index = fields[nkeys];

      while (*t && ',' != *t)
	++t;
      if (',' == *t)
	++t;
    }
  *nfields = nkeys;
  fields[nkeys] = -1;
  return fields;
}

void
create_pmap(void)
{
  int i, val_max = -1;

  for (i = 0; i < sip->nmember; ++i)
    {
      int mindex = (i * sip->nfields)+period_index;
      if (sip->scodes[mindex] > val_max)
	val_max = sip->scodes[mindex];
    }

  pmaptab = calloc((1+val_max), sizeof(int));

  for (i = 0; i < sip->nmember; ++i)
    {
      int mindex = (i * sip->nfields)+period_index;
      struct periodmap *pm = periodmap((char*)&sip->pool[sip->pindex[mindex]],
				       strlen((char*)&sip->pool[sip->pindex[mindex]]));
      if (0)
	printf("%d=%s=%d\n", 
	       sip->scodes[mindex], 
	       &sip->pool[sip->pindex[mindex]],
	       pm ? pm->p : -1);
      else if (!pm)
	{
	  fprintf(stderr, "xisperiods: no entry for '%s' in periodmap\n",
		  &sip->pool[sip->pindex[mindex]]);
	  
	}

      if (!pmaptab[sip->scodes[mindex]])
	{
	  if (pm)
	    pmaptab[sip->scodes[mindex]] = pm->p;
	  else
	    pmaptab[sip->scodes[mindex]] = XX;
	}
    }
}

int
main(int argc, char **argv)
{
  const char *fname[2];
  char *subxisfn = NULL;

  seen = hash_create(1024);  

  xisfp = stdout;

  full_count_mode = 1;

  options(argc,argv,"cdl:p:Pt:x:");

  if (tis_file)
    {
      if (!(tis_fp = fopen(tis_file,"a")))
	{
	  fprintf(stderr, "xisperiods: unable to open .tis file %s\n", tis_file);
	  exit(1);
	}
    }
  
  subxisfn = malloc(strlen(xisfile) + 5);
  (void)sprintf(subxisfn, "%s.sub", xisfile);
  subxisfp = fopen(subxisfn,"w");
  fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n", subxisfp);
  fputs("<xisses xmlns=\"http://oracc.org/ns/xis/1.0\" xmlns:xis=\"http://oracc.org/ns/xis/1.0\">", subxisfp);

  fname[0] = xisfile;
  fname[1] = NULL;

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

  /* must do set_keys first because that sets period_index */
  create_pmap();

  fprintf(xisfp, "<xis-periods xmlns=\"%s\" xmlns:xis=\"%s\">", xis_uri, xis_uri);

  runexpatNS(i_list,fname,sH,eH,":");

  fprintf(xisfp, "</xis-periods>");
  fputs("</xisses>",subxisfp);
  fclose(subxisfp);

  /*hash_free2(seen, free, NULL);  */

  return 0;
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
    case 'p':
      project = arg;
      break;
    case 't':
      tis_file = arg;
      break;
    case 'x':
      xisfile = arg;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "xisperiods";
int major_version = 1, minor_version = 1, verbose = 0;
const char *usage_string = "[file]";
void
help ()
{

}

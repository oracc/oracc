#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <psd_base.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>
#include <npool.h>
#include <memblock.h>

#include "se.h"
#include "types.h"
#include "selib.h"
#include "wm.h"

/* This program requires have-xtf.lst to be 
   properly sorted */

char last_ref[128];
int first_ref = 0;
int verbose = 0;

struct npool *wm_pool = NULL;

const char *kwic_fn = "02pub/kwic.map";
const char *ranges_fn = "02pub/unit.map";

/*const char *unit_fn = "02pub/unit.map";*/

FILE *kwic_fp = NULL;
FILE *ranges_fp = NULL;

/*FILE *unit_fp = NULL;*/

int quiet = 0;

const char *project = NULL;

const char *norm_ns_uri = "http://oracc.org/ns/norm/1.0";
const char *xcl_ns_uri = "http://oracc.org/ns/xcl/1.0";
const char *gdl_ns_uri = "http://oracc.org/ns/gdl/1.0";
const char *xtf_ns_uri = "http://oracc.org/ns/xtf/1.0";

static struct wm_kwic kwic;
static struct wm_unit unit;

struct wm_range *ur;
struct mb *mm_unit_ranges;

const char *input = NULL;
FILE *input_fp = NULL;

static char fnbuf[_MAX_PATH];
static char **fnlist = NULL;
static size_t findex;

const char *
attr_by_name(const char **atts,const char *name)
{
  int i;
  for (i = 0; atts[i] != NULL; i+=2)
    if (!strcmp(atts[i],name))
      return atts[i+1];
  return NULL;
}

static int
is_ns(const char *name, const char *uri)
{
  const char *ns_end = strchr(name, '|');
  if (ns_end && !strncmp(name, uri, ns_end - name))
    return strlen(uri)+1;
  else
    return 0;
}

static void
kwic_map_entry(void)
{
  xfwrite(kwic_fn, TRUE, &kwic, sizeof(struct wm_kwic), 1, kwic_fp);
}

static void
unit_map_entry(void)
{
  /* xfwrite(unit_fn, TRUE, &unit, sizeof(struct wm_unit), 1, unit_fp); */
  return;
}

static void
start_wm(void *userUnit, const char *name, const char **atts)
{
  int post_ns = 0;
  if ((post_ns = is_ns(name,xcl_ns_uri)))
    {
      if (!strcmp(name+post_ns,"l"))
	{
	  strcpy(last_ref, findAttr(atts, "ref"));
	  if (first_ref)
	    {
	      wm_text(&ur->t, last_ref);
	      wm_lineword2(&ur->start, last_ref);
	      first_ref = 0;
	    }
	  wm_lineword(&unit, &kwic, last_ref);
	  wm_unitlemm(&unit, xml_id(atts));
	  unit_map_entry();
	}
      else if (!strcmp(name+post_ns,"c") 
	       && !strcmp(findAttr(atts, "type"), "sentence"))
	{
	  const char *xid = NULL /*, *U*/;
	  if (ur)
	    wm_lineword2(&ur->end, last_ref);
	  
	  ur = mb_new(mm_unit_ranges);
	  first_ref = 1;
	  xid = xml_id(atts);
#if 0
	  if ((U = strchr(xid,'U')))
	    {
	      ur->s = atoi(U+1);
	    }
	  else
	    {
	      fprintf(stderr, "wmapper: no 'U' in id %s\n", xid);
	    }
#endif
	}
    }
  else if ((post_ns = is_ns(name,gdl_ns_uri)))
    {
      name = name + post_ns;
      if (!strcmp(name,"w"))
	{
	  const char *xid = xml_id(atts);
	  if (strncmp(xid, "gdl", 3))
	    {
	      wm_lineword(&unit, &kwic, xid);
	      kwic_map_entry();
	    }
	}
    }
  else if ((post_ns = is_ns(name,norm_ns_uri)))
    {
      name = name + post_ns;
      if (!strcmp(name,"w"))
	{
	  const char *xid = xml_id(atts);
	  if (strncmp(xid, "gdl", 3))
	    {
	      wm_lineword(&unit, &kwic, xid);
	      kwic_map_entry();
	    }
	}
    }
  charData_discard();
}

static void
end_wm(void *userUnit, const char *name)
{
  int post_ns = 0;
  if ((post_ns = is_ns(name,xcl_ns_uri)))
    {
      if (!strcmp(name+post_ns,"xcl") && ur)
	wm_lineword2(&ur->end, last_ref);
    }
  charData_discard();
}

static void
fn_expand(void *p)
{
  char *fn = (char*)l2_expand(NULL, p, "xtf"); /* let l2_expand figure it out from p */
  fnlist[findex] = (char*)npool_copy((unsigned char*)fn, wm_pool);
  if (!access(fnlist[findex],R_OK))
    {
      if (verbose)
	fprintf(stderr,"found %s\n",fnlist[findex]);
      ++findex;
    }
  else 
    {
      if (!quiet)
	fprintf(stderr,"no input file for %s\n",(char*)p);
    }
}

int
main (int argc, char **argv)
{
  char *fn = NULL;
  List *files = list_create(LIST_SINGLE);
  struct wm_range *ranges = NULL;
  size_t ranges_bytes = 0;

  options(argc, argv, "i:p:v");
  if (!project)
    {
      fprintf(stderr,"wmapper: must give -p PROJECT on command line\n");
      exit(1);
    }

  if (input)
    input_fp = xfopen(input, "r");
  else
    input_fp = stdin;

  wm_pool = npool_init();
  while (NULL != (fn = fgets(fnbuf,_MAX_PATH,input_fp)))
    {
      fn[strlen(fn)-1] = '\0';
      list_add(files,npool_copy((unsigned char *)fn, wm_pool));
    }
  fnlist = malloc((1+files->count) * sizeof(const char *));
  findex = 0;
  list_exec(files,fn_expand);
  fnlist[findex] = NULL;

  kwic_fp = xfopen(kwic_fn,"wb");
  /*  unit_fp = xfopen(unit_fn,"wb"); */

  mm_unit_ranges = mb_init(sizeof(struct wm_range), 32);

  runexpatNS(i_list, fnlist, start_wm, end_wm, "|");

  xfclose(kwic_fn,kwic_fp);
  /*  xfclose(unit_fn,unit_fp); */

  ranges_fp = xfopen(ranges_fn, "wb");
  ranges = mb_merge(mm_unit_ranges, &ranges_bytes);
  xfwrite(ranges_fn, TRUE, ranges, 1, ranges_bytes, ranges_fp);

  mb_term(mm_unit_ranges);
  list_free(files, NULL);
  free(fnlist);
  npool_term(wm_pool);
  
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'i':
      input = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "wmapper";
int major_version = 1, minor_version = 0;
const char *usage_string = "";
void
help ()
{
}

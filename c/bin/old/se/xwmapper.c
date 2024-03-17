#include <stdio.h>
#include <stdarg.h>
#include <string.h>
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
int verbose;

const char *kwic_fn = "02pub/kwic.map";
const char *ranges_fn = "02pub/ranges.map";
const char *unit_fn = "02pub/unit.map";

FILE *kwic_fp = NULL;
FILE *ranges_fp = NULL;
FILE *unit_fp = NULL;

int quiet = 0;

const char *project = NULL;

const char *norm_ns_uri = "http://oracc.org/ns/norm/1.0";
const char *xcl_ns_uri = "http://oracc.org/ns/xcl/1.0";
const char *gdl_ns_uri = "http://oracc.org/ns/gdl/1.0";
const char *xtf_ns_uri = "http://oracc.org/ns/xtf/1.0";

static struct wm_unit data;

struct wm_unit_range *ur;
struct mb *mm_unit_ranges;

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
  xfwrite(kwic_fn, TRUE, &data.t, sizeof(Four_bytes), 1, kwic_fp);
  xfwrite(kwic_fn, TRUE, &data.w, sizeof(struct wid), 1, kwic_fp);
}

static void
unit_map_entry(void)
{
  xfwrite(unit_fn, TRUE, &data, sizeof(struct wm_unit), 1, unit_fp);
}

static Four_bytes
wm_q(Four_bytes id)
{
  return id|WM_Q_BIT;
}

static Four_bytes
wm_x(Four_bytes id)
{
  return id|WM_X_BIT;
}

static void
wm_lineword2(struct wid *w, const char *id)
{
  id += 8;
  w->l = atoi(id);
  while (*id && '.' != *id)
    ++id;
  ++id;
  w->w = atoi(id);
}

static void
wm_lineword(const char *id)
{
  data.t = atoi(id+1);
  if (*id == 'Q')
    data.t = wm_q(data.t);
  else if (*id == 'X')
    data.t = wm_x(data.t);
}

static void
wm_unitlemm(const char *id)
{
  id += 8;
  data.s = atoi(id);
#if 0
  while (*id && '.' != *id)
    ++id;
  ++id;
  data.u.l = atoi(id);
#endif
}

static void
start_wm(void *userData, const char *name, const char **atts)
{
  int post_ns = 0;
  if ((post_ns = is_ns(name,xcl_ns_uri)))
    {
      if (!strcmp(name+post_ns,"l"))
	{
	  strcpy(last_ref, findAttr(atts, "ref"));
	  wm_lineword(last_ref);
	  wm_unitlemm(xml_id(atts));
	  unit_map_entry();
	}
      else if (!strcmp(name+post_ns,"c") 
	       && !strcmp(findAttr(atts, "type"), "sentence"))
	{
	  const char *xid = NULL, *U;
	  if (ur)
	    wm_lineword2(&ur->end, last_ref);
	  
	  ur = mm_new(mm_unit_ranges);
	  first_ref = 1;
	  xid = xml_id(atts);
	  if ((U = strchr(xid,'U')))
	    {
	      ur->s = atoi(U+1);
	    }
	  else
	    {
	      fprintf(stderr, "wmapper: no 'U' in id %s\n", xid);
	    }
	}
    }
  else if ((post_ns = is_ns(name,gdl_ns_uri)))
    {
      name = name + post_ns;
      if (!strcmp(name,"w"))
	{
	  wm_lineword(xml_id(atts));
	  kwic_map_entry();
	}
    }
  else if ((post_ns = is_ns(name,norm_ns_uri)))
    {
      name = name + post_ns;
      if (!strcmp(name,"w"))
	{
	  wm_lineword(xml_id(atts));
	  kwic_map_entry();
	}
    }
  charData_discard();
}

static void
end_wm(void *userData, const char *name)
{
  charData_discard();
}

static void
fn_expand(void *p)
{
  fnlist[findex] = (char*)l2_expand(project, p, "xtf");
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
  struct npool *wm_pool = npool_init();
  void *ranges = NULL;
  size_t ranges_bytes = 0;

  project = argv[1];
  if (!project)
    {
      fprintf(stderr,"wmapper: must give project on command line\n");
      exit(1);
    }

  while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
    {
      fn[strlen(fn)-1] = '\0';
      list_add(files,npool_copy((unsigned char *)fn, wm_pool));
    }
  fnlist = malloc((1+files->count) * sizeof(const char *));
  findex = 0;
  list_exec(files,fn_expand);
  fnlist[findex] = NULL;

  kwic_fp = xfopen(kwic_fn,"wb");
  unit_fp = xfopen(unit_fn,"wb");

  mm_unit_ranges = mb_init(sizeof(struct wm_unit_range, 32));

  runexpatNS(i_list, fnlist, start_wm, end_wm, "|");

  xfclose(kwic_fn,kwic_fp);
  xfclose(unit_fn,unit_fp);

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

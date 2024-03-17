#include <stdio.h>
#include <stdarg.h>
#include <psd_base.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>

#include "se.h"
#include "../types.h"
#include "selib.h"
#include "wm.h"

const char *kwic_fn = "02pub/kwic.map";
const char *unit_fn = "02pub/unit.map";

FILE *kwic_fp = NULL;
FILE *unit_fp = NULL;

const char *norm_ns_uri = "http://oracc.org/ns/norm/1.0";
const char *xcl_ns_uri = "http://oracc.org/ns/xcl/1.0";
const char *xtf_ns_uri = "http://oracc.org/ns/xtf/1.0";

static struct wm data;

static void
kwic_map_entry(void)
{
  xfwrite(kwic_fn, TRUE, &data.w, sizeof(struct wm_wid), 1, kwic_fp);
}

static void
unit_map_entry(void)
{
  xfwrite(unit_fn, TRUE, &data.u, sizeof(struct wm_uid), 1, unit_fp);
}

static void
wm_lineword(const char *id)
{
  data.w.t = atoi(id+1);
  if (*id == 'Q')
    data.w.t = wm_q(data.w.t);
  else if (*id == 'X')
    data.w.t = wm_x(data.w.t);
  id += 8;
  data.w.l = atoi(id);
  while (*id && '.' != *id)
    ++id;
  ++id;
  data.w.w = atoi(id);
}

static void
wm_unitlemm(const char *id)
{
  id += 8;
  data.u.u = atoi(id);
  while (*id && '.' != *id)
    ++id;
  ++id;
  data.u.l = atoi(id);
}

static void
start_wm(void *userData, const char *name, const char **atts)
{
  int post_ns = 0;
  if ((post_ns = is_ns(name,xcl_ns_uri)))
    {
      if (!strcmp(name+post_ns,"l"))
	{
	  wm_lineword(findAttr(atts, "ref"));
	  wm_unitlemm(xml_id(atts));
	  unit_map_entry();
	}
    }
  else if ((post_ns = is_ns(name,xtf_ns_uri)))
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

int
main (int argc, char **argv)
{
  char *fn = NULL;
  List *files = list_create(LIST_SINGLE);
  while (NULL != (fn = fgets(fnbuf,_MAX_PATH,stdin)))
    {
      fn[strlen(fn)-1] = '\0';
      list_add(files,strdup(fn));
    }
  fnlist = malloc((1+files->count) * sizeof(const char *));
  findex = 0;
  list_exec(files,fn_expand);
  fnlist[findex] = NULL;

  kwic_fp = xfopen(kwic_fn,"b");
  unit_fp = xfopen(unit_f,"b");

  runexpatNS(i_list, fnlist, start_wm, end_wm, "|");

  xfclose(kwic_fn,kwic_fp;
  xfclose(unit_fn_kwic_fp);
}

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <psd_base.h>
#include <options.h>
#include <loadfile.h>

#include "se.h"
#include "types.h"
#include "selib.h"
#include "wm.h"

struct wm_kwic *kwic_map;
struct wm_range *unit_map;

const char *kwic_fn = "02pub/kwic.map";
const char *unit_fn = "02pub/unit.map";

int dump_kwic = 0, dump_unit = 0, verbose = 0;

int
main (int argc, char **argv)
{
  size_t kwic_size, unit_size;

  options(argc, argv, "ku");

  kwic_map = (struct wm_kwic*)loadfile((unsigned char *)kwic_fn,&kwic_size);
  unit_map = (struct wm_range*)loadfile((unsigned char *)unit_fn,&unit_size);

  if (dump_kwic)
    {
      size_t i;
      for (i = 0; i < (kwic_size/sizeof(struct wm_kwic)); ++i)
	{
	  struct wm_kwic *k = &kwic_map[i];
	  fprintf(stdout, "%c%06d.%d.%d\n", 
		  wm_prefix_char(k->t),
		  wm_text_id(k->t),
		  k->w.l,
		  k->w.w);
	}
    }
  else if (dump_unit)
    {
      size_t i;
      for (i = 0; i < (unit_size/sizeof(struct wm_range)); ++i)
	{
	  struct wm_range *u = &unit_map[i];
	  fprintf(stdout, "%c%06d.%d.%d %d.%d\n", 
		  wm_prefix_char(u->t),
		  wm_text_id(u->t),
		  u->start.l, u->start.w, 
		  u->end.l, u->end.w);
	}      
    }
  return 0;
}

int
opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'k':
      dump_kwic = 1;
      break;
    case 'u':
      dump_unit = 1;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "wmdebug";
int major_version = 1, minor_version = 0;
const char *usage_string = "";
void
help ()
{
}

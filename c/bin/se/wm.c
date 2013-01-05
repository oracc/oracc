#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <psd_base.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>
#include <npool.h>
#include <loadfile.h>

#include "se.h"
#include "types.h"
#include "selib.h"
#include "wm.h"

/* This program requires have-xtf.lst to be 
   properly sorted */

const char *project = NULL;
int verbose;

struct kwic_range
{
  struct wid start;
  struct wid end;
};

enum wm_format wm_type = KU_NONE;

const char *input, *output;
FILE *infp, *outfp;

const char *kwic_fn = NULL;
const char *unit_fn = NULL;
struct wm_range *unit_map = NULL;
struct wm_kwic *kwic_map = NULL;
size_t kwic_len, kwic_num, unit_len, unit_num;

int kwic_radius = 5;

char *id_arg = NULL;

static char *
no_newline(char *s)
{
  s[strlen(s)-1] = '\0';
  return s;
}

void
kwic_print(FILE *fp, const char *proj, const char *id, struct kwic_range *k)
{
  char *period = strchr(id, '.');
  if (period)
    *period = '\0';
  fprintf(fp, "%s:%s.%d.%d", proj, id, k->start.l, k->start.w);
  fputc(' ', fp);
  fprintf(fp, "%s.%d.%d", id, k->end.l, k->end.w);
  if (period)
    *period = '.';
  fprintf(fp, " %s\n", id);
}

struct wm_kwic *
kwic_find_wid(struct wm_kwic *w)
{
  /* initial implementation, linear search;
     to be replaced by bsearch
   */
  size_t i;

  if (!w)
    return NULL;

  for (i = 0; i < kwic_num; ++i)
    if (w->t == kwic_map[i].t
	&& w->w.l == kwic_map[i].w.l
	&& w->w.w == kwic_map[i].w.w)
      return &kwic_map[i];

  return NULL;
}

struct kwic_range *
kwic_find(const char *wid)
{
  struct wm_kwic *index, ktmp;
  struct wm_unit utmp;
  wm_text(&utmp.t, wid);
  wm_lineword(&utmp, &ktmp, wid);
  if ((index = kwic_find_wid(&ktmp)))
    {
      int i, j;
      static struct kwic_range k;
      for (i = j = 0; (index-j) > kwic_map && index[j-1].t == index->t && ++i < kwic_radius;)
	--j;
      k.start = index[j].w;
      for (i = j = 0; j < kwic_num && index[j+1].t == index->t && ++i < kwic_radius;)
	++j;
      k.end = index[j].w;
      return &k;
    }
  else
    return NULL;
}

void
kwic_controller(char *id)
{
  struct kwic_range *k = NULL;
  if ((k = kwic_find(id)))
    kwic_print(outfp, project_of(id), skip_project(id), k);
}

void
kwic_init(const char *project)
{
  static char fn[_MAX_PATH];
  sprintf(fn,"%s/pub/%s/kwic.map",oracc_home(),project);
  kwic_map = (struct wm_kwic*)(void*)loadfile((unsigned char *)fn,&kwic_len);
  kwic_num = (kwic_len / sizeof(struct wm_kwic));
  kwic_fn = fn;
}

void
kwic_term(void)
{
  free(kwic_map);
  kwic_map = NULL;
}

void
unit_print(FILE *fp, const char *project, const char* id, struct wm_range *u)
{
  char *period = strchr(id, '.');
  if (period)
    *period = '\0';
  fprintf(fp, "%s:%s.%d.%d %s.%d.%d", 
	  project,
	  id, u->start.l, u->start.w, 
	  id, u->end.l, u->end.w);
  if (period)
    *period = '.';
  fprintf(fp, " %s\n", id);
}

struct wm_range *
unit_find_range(struct wm_unit *utmp)
{
  size_t i;
  /* find the first text which matches */
  for (i = 0; i < unit_len; ++i)
    if (utmp->t == unit_map[i].t)
      break;
  if (i < unit_len)
    {
      size_t j;
      for (j = i; unit_map[i].t == unit_map[j].t && j < unit_len; ++j)
	{
	  if (utmp->w.l >= unit_map[j].start.l
	      && utmp->w.l <= unit_map[j].end.l)
	    {
	      if ((utmp->w.l != unit_map[j].start.l
		   && utmp->w.l != unit_map[j].end.l)
		  || (utmp->w.l == unit_map[j].start.l
		      && utmp->w.w >= unit_map[j].start.w)
		  || (utmp->w.l == unit_map[j].end.l
		      && utmp->w.w <= unit_map[j].end.w))
		break;
	    }
	}
      if (j < unit_len)
	return &unit_map[j];
    }
  return NULL;
}

struct wm_range *
unit_find(const char *wid)
{
  struct wm_unit utmp;
  wm_text(&utmp.t, wid);
  wm_lineword(&utmp, NULL, wid);
  return unit_find_range(&utmp);
}

void
unit_controller(char *id)
{
  struct wm_range *u = NULL;
  if ((u = unit_find(id)))
    unit_print(outfp, project_of(id), skip_project(id), u);
}

void
unit_init(const char *project)
{
  static char fn[_MAX_PATH];
  sprintf(fn,"%s/pub/%s/unit.map",oracc_home(),project);
  unit_map = (struct wm_range*)(void*)loadfile((unsigned char *)fn,&unit_len);
  unit_num = (unit_len / sizeof(struct wm_range));
  unit_fn = fn;
}

void
unit_term(void)
{
  free(unit_map);
  unit_map = NULL;
}

/* FEATUREME: add radius support to line context */
void
line_print(FILE *fp, const char *project, const char*line_id, const char *id)
{
  fprintf(outfp, "%s:%s %s %s\n", project, line_id, line_id, id);
}

void
line_controller(char *id)
{
  char *line_id = malloc(strlen(id)+1), *dot;
  strcpy(line_id, skip_project(id));
  if ((dot = strchr(line_id, '.')))
    {
      if ((dot = strchr(dot+1, '.')))
	*dot = '\0';
    }
  line_print(outfp, project_of(id), line_id, skip_project(id));
}

int
main (int argc, char **argv)
{
#define _MAX_ID 32
  char idbuf[_MAX_ID], *id;
  options(argc, argv, "i:I:klo:p:r:u");

  if (!project)
    {
      fprintf(stderr,"wm: must give -p PROJECT on command line\n");
      exit(1);
    }

  if (input)
    infp = xfopen(input, "r");
  else
    infp = stdin;

  if (output)
    outfp = xfopen(output, "w");
  else
    outfp = stdout;

  if (wm_type == KU_UNIT)
    {
      unit_init(project);
      if (id_arg)
	unit_controller(id_arg);
      else
	while (NULL != (id = fgets(idbuf,_MAX_ID,infp)))
	  unit_controller(no_newline(id));
      unit_term();
    }
  else if (wm_type == KU_KWIC)
    {
      kwic_init(project);
      if (id_arg)
	kwic_controller(id_arg);
      else
	while (NULL != (id = fgets(idbuf,_MAX_PATH,infp)))
	  kwic_controller(no_newline(id));
      kwic_term();
    }
  else if (wm_type == KU_LINE)
    {
      if (id_arg)
	line_controller(id_arg);
      else
	while (NULL != (id = fgets(idbuf,_MAX_PATH,infp)))
	  line_controller(no_newline(id));      
    }
  else
    {
      fprintf(stderr, "wm: must give -k, -l or -u (kwic or unit) on command line\n");
      exit(1);
    }
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
    case 'I':
      id_arg = arg;
      break;
    case 'k':
      wm_type = KU_KWIC;
      break;
    case 'l':
      wm_type = KU_LINE;
      break;
    case 'o':
      output = arg;
      break;
    case 'p':
      project = arg;
      break;
    case 'r':
      kwic_radius = atoi(arg);
      break;
    case 'u':
      wm_type = KU_UNIT;
      break;
    default:
      return 1;
    }
  return 0;
}

const char *prog = "wm";
int major_version = 1, minor_version = 0;
const char *usage_string = "";
void
help ()
{
}

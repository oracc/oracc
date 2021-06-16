#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype128.h>
#include <psdtypes.h>
#include <hash.h>
#include <messages.h>
#include <npool.h>
#include <xmd.h>
#include <fname.h>
#include <xmlutil.h>
#include <xpd2.h>
#include "ce.h"
#include "p2.h"

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

static int nfields = 0, nwidths = 0, nlabels = 0;

extern int p3;

extern int item_offset, tabbed;
extern const char *arg_fields;
extern const char *mode, *project;
const char *xmd_fields = NULL, *xmd_widths = NULL, *xmd_labels;

static const char *default_fields 
  = "designation,primary_publication,subgenre|genre,period,place|provenience";
static const char *default_widths = "auto,17,17,17,17";
static const char *default_labels = NULL; /*"Designation,Publication,Content,Period,Provenience";*/

static struct npool *ce_xmd_pool;

const char **field_specs;
const char **width_specs;
const char **label_specs;
List **field_lists;

char *url_base = NULL;

int
xmd_field_count(void)
{
  return nfields;
}

int
count_entries(const char *tmp, const char *option)
{
  int i;

  if (!tmp)
    return 0;

  for (i = 1; *tmp; ++tmp)
    {
      if (',' == *tmp)
	{
	  ++i;
	  while (isspace(tmp[1]))
	    ++tmp;
	  if (',' == tmp[1])
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      while (isspace(tmp[1]) || ',' == tmp[1])
		++tmp;
	    }
	}
    }
  return i;
}

void
set_entries(const char **entries, const char *option)
{
  int i;
  char *tmp;

  if (!option)
    return;

  if (!state)
    state = "default";

  tmp = malloc(strlen(option)+1);
  (void)strcpy(tmp, option);
  for (i = 0, entries[0] = tmp; *tmp; )
    {
      if (',' == *tmp)
	{
	  *tmp++ = '\0';
	  while (isspace(tmp[1]))
	    ++tmp;
	  if (',' == tmp[1])
	    {
	      fprintf(stderr, "ce_xmd2: %s/00lib/config.xml: empty field in `%s'\n",
		      project, option);
	      while (isspace(tmp[1]) || ',' == tmp[1])
		++tmp;
	    }
	  if (*tmp)
	    entries[++i] = tmp;
	}
      else
	++tmp;
    }
  entries[++i] = NULL;
}

static void
set_field_lists(const char **fieldspecs)
{
  int i;
  vec_sep_str = "|";
  field_lists = malloc((1+nfields) * sizeof(List *));
  for (i = 0; fieldspecs[i]; ++i)
    {
      char *str = malloc(strlen(fieldspecs[i])+1);
      strcpy(str, fieldspecs[i]);
      field_lists[i] = list_from_str(str, NULL, LIST_SINGLE);
      free(str);
    }
  field_lists[i] = 0;
}

int
xmdinit2(const char *project)
{
  struct p2_options *p2opt = NULL;

  ce_xmd_pool = npool_init();
  p2opt = p2_load(project, state, ce_xmd_pool);

  if (!xmd_fields)
    xmd_fields = (p2opt->catalog_fields ? p2opt->catalog_fields : default_fields);
  if (!xmd_widths)
    xmd_widths = (p2opt->catalog_widths ? p2opt->catalog_widths : default_widths);
  if (!xmd_labels)
    xmd_labels = (p2opt->catalog_labels ? p2opt->catalog_labels : default_labels);

  nfields = count_entries(xmd_fields, "catalog-fields");
  nwidths = count_entries(xmd_widths, "catalog-widths");
  nlabels = count_entries(xmd_labels, "catalog-widths");

  if (nfields != nwidths)
    {
      fprintf(stderr, 
	      "ce_xmd2: %s/00lib/config.xml: `%s-catalog-fields' and `%s-catalog-widths' should have same number of entries\n",
	      project, state, state);
      return 1;
    }
  if (xmd_labels && nfields != nlabels)
    {
      fprintf(stderr, 
	      "ce_xmd2: %s/00lib/config.xml: `%s-catalog-fields' and `%s-catalog-labels' should have same number of entries\n",
	      project, state, state);
      return 1;
    }
  

  field_specs = malloc((nfields+1)*sizeof(const char *));
  width_specs = malloc((nwidths+1)*sizeof(const char *));
  label_specs = malloc((nlabels+1)*sizeof(const char *));

  set_entries(field_specs, xmd_fields);
  set_entries(width_specs, xmd_widths);
  if (xmd_labels)
    set_entries(label_specs, xmd_labels);

  set_field_lists(field_specs);

  return 0;
}

static int
alldigit(const char *s)
{
  while (isdigit(*s))
    ++s;
  return (*s == '\0');
}

/* Previous version had this:

   Hack designation/primary_publication issues: if 0 is empty, use 1;
     if 0 == 1, empty 1 
  if (!strcmp(print_ptrs[0]," "))
    {
      print_ptrs[0] = print_ptrs[1];
      print_ptrs[1] = " ";
    }
  else if (!strcmp(print_ptrs[0], print_ptrs[1]))
    print_ptrs[1] = " ";
 */
void
xmdprinter2(const char *pq)
{
  static int nth = 0;
  extern int in_group;

  if ('#' == *pq)
    {
      if (in_group)
	fputs("</ce:group>", stdout);
      else
	in_group = 1;
      fputs("<ce:group>", stdout);
      fprintf(stdout, "<ce:heading>%s</ce:heading>", xmlify((unsigned char *)++pq));
    }
  else
    {
      Hash_table *fields = NULL;
      int i;
      const char *designation = NULL;
      const char *icon = NULL, *icon_alt;
      const char *id = NULL;
      const char *colon = NULL;

      ++nth;
      xmd_init();

      if (!url_base)
	url_base = malloc(strlen(project) + strlen("javascript:catItemView('xtf',)")+8);
#if 0
      sprintf(url_base,"http://oracc.museum.upenn.edu/%s/cat",project);
#else
      if (p3)
	sprintf(url_base, "javascript:p3item('xtf',%d)", item_offset+nth);
      else
	sprintf(url_base, "javascript:catItemView(%d)", item_offset+nth);
#endif

      /* pq is a qualified ID, so use the project from that */
      if ((colon = strchr(pq, ':')))
	{
	  const char *fn = l2_expand(project,colon+1,"xmd");
	  /* only use the qualifying project if there is no local XMD */
	  if (!xaccess(fn,R_OK,0))
	    fields = l2_xmd_load(project, colon+1);
	  else 
	    fields = l2_xmd_load(NULL, pq);
	}
      else
	fields = l2_xmd_load(project, pq);

      if (!in_group)
	{
	  in_group = 1;
	  fputs("<ce:group>", stdout);
	}

      fputs("<ce:data><tr xmlns=\"http://www.w3.org/1999/xhtml\">",stdout);

      if ((id = hash_find(fields, (unsigned char *)"id_text")))
	{
	  if (*id == 'P')
	    {
	      icon = "cdli-icon.png";
	      /* url_base = "http://oracc.museum.upenn.edu/%s/cat"; */
	      /* url_base = "http://cdli.ucla.edu"; */
	      icon_alt = "CDLI catalog";
	    }
	  else
	    {
	      static char projurl[128];
	      icon = "xnum-icon.png";
	      sprintf(projurl, "/%s", project);
	      /* url_base = "http://oracc.museum.upenn.edu/cat"; */
	      icon_alt = "project catalog";
	    }
	}
      else if ((id = hash_find(fields, (unsigned char *)"id_composite")))
	{
	  icon = "Qcat-icon.png";
	  /* url_base = "http://oracc.museum.upenn.edu/cat"; */
	  icon_alt = "Q catalog";
	}
#if 0
      fprintf(stdout, "<td class=\"ce-xmd-icon\"><a href=\"%s/%s\"><img src=\"/img/%s\" alt=\"%s in %s\"/></a></td>", url_base, id, icon, id, icon_alt);
#else
      /* fprintf(stdout, "<td class=\"ce-xmd-icon\"><a href=\"%s\"><img src=\"/img/%s\" alt=\"%s in %s\"/></a></td>", url_base, icon, id, icon_alt); */
      fprintf(stdout, "<td class=\"ce-xmd-icon\"><a href=\"%s\"><img src=\"/img/viewtext.png\" alt=\"View text\"/></a></td>", url_base);
#endif
      for (i = 0; width_specs[i]; ++i)
	{
	  List *tmp = field_lists[i];
	  const char *pct = width_specs[i];
	  const char *field, *value, *field_used = "";
	  char pctbuf[4];
	  int this_is_designation = 0;

	  for (field = list_first(tmp); field; field = list_next(tmp))
	    {
	      if ((value = hash_find(fields, (unsigned char *)field)) && strlen(value))
		{
		  field_used = field;
		  break;
		}
	    }
	  if (!strcmp(field_used, "designation") || !strcmp(field_used, "title"))
	    {
	      designation = value;
	      this_is_designation = 1;
	    }
	  if (!value || !strlen(value)
	      || (designation 
		  && !strcmp(field_used, "primary_publication")
		  && !strcmp(designation, value)))
	    value = " ";
	  if (alldigit(pct))
	    {
	      strncpy(pctbuf,pct,2);
	      pctbuf[2] = '%';
	      pctbuf[3] = '\0';
	      pct = pctbuf;
	    }
	  if (p3)
	    {
	      if (this_is_designation || i < link_fields)
		fprintf(stdout, "<td style=\"width: %s\"><a href=\"javascript:p3item('xtf',%d)\">%s</a></td>", pct, item_offset+nth, xmlify((unsigned char *)value));
	      else
		fprintf(stdout, "<td style=\"width: %s;\">%s</td>", pct, xmlify((unsigned char *)value));
	    }
	  else
	    {
	      if (this_is_designation || i < link_fields)
		fprintf(stdout, "<td style=\"width: %s\"><a href=\"javascript:itemView(%d)\">%s</a></td>", pct, item_offset+nth, xmlify((unsigned char *)value));
	      else
		fprintf(stdout, "<td style=\"width: %s;\">%s</td>", pct, xmlify((unsigned char *)value));
	    }
	}
      fputs("</tr></ce:data>",stdout);
    }
}

void
xmd_print_labels(void)
{
  if (nlabels)
    {
      int i;
      fputs("<ce:labels><thead><tr>",stdout);
      if (ood_mode)
	fputs("<th>ID</th>",stdout);
      else
	fputs("<th> </th>",stdout);
      for (i = 0; label_specs[i]; ++i)
	fprintf(stdout, "<th>%s</th>", label_specs[i]);
      fputs("</tr></thead></ce:labels>",stdout);
    }
}

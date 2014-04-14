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
#include <runexpat.h>
#include <gdf.h>
#include <xmlutil.h>
#include <xpd2.h>
#include "selib.h"
#include "ce.h"
#include "p2.h"

#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif

extern int p3;

extern int item_offset, tabbed;

extern const char *arg_fields, *gdf_xml, *idattr;
extern const char *gdfset, *mode, *project;
const char *gdf_fields = NULL, *gdf_widths = NULL;

extern const char **field_specs;
extern const char **width_specs;
extern List **field_lists;

static List *record_hashes;
static struct npool *record_pool;
static Hash_table *curr_record_hash = NULL;

static void gdfprinter2(Hash_table *fields);

static char *url_base = NULL;

extern int *idcountp;
extern Hash_table *ht;
extern int echoing;

void
gdfinit(void)
{
  record_hashes = list_create(LIST_SINGLE);
  record_pool = npool_init();
  return 0;
}

static int
alldigit(const char *s)
{
  while (isdigit(*s))
    ++s;
  return (*s == '\0');
}

void
gdf_sH(void *userData, const char *name, const char **atts)
{
  if (!strcmp(name,"o:record"))
    {
      const char *id = attr_by_name(atts,idattr);
      if (id && (idcountp = hash_find(ht,(const unsigned char*)id)))
	{
	  curr_record_hash = hash_create(1);
	  hash_add(curr_record_hash, (unsigned char *)"o:id", npool_copy((unsigned char *)id, record_pool));
	  echoing = 1;
	}
    }
  else if (echoing)
    {
      if (echoing++ > 1)
	fprintf(stderr, "ce_gdf: GDF records may not contain XML tags\n");
    }
  else
    charData_discard();
}

void
gdf_eH(void *userData, const char *name)
{
  if (echoing)
    {
      if (!strcmp(name,"o:record"))
	{
	  echoing = 0;
	  list_add(record_hashes, curr_record_hash);
	  curr_record_hash = NULL;
	  charData_discard();
	}
      else if (--echoing == 1)
	{
	  hash_add(curr_record_hash, 
		   npool_copy((unsigned char *)name, record_pool), 
		   npool_copy((unsigned char *)charData_retrieve(), record_pool));
	}
    }
  else
    charData_discard();
}

void
gdfprinter(void)
{
  list_exec(record_hashes, (void(*)(void*))gdfprinter2);
}

static void
gdfprinter2(Hash_table *fields)
{
  static int nth = 0;
  int i;
  const char *designation = NULL;
  const char *icon = NULL, *icon_alt = NULL;
  const char *id = NULL;
  extern int in_group;

  if (!in_group)
    {
      in_group = 1;
      fputs("<ce:group>", stdout);
    }

  ++nth;
  if (!url_base)
    url_base = malloc(strlen(project) + strlen("javascript:p3item('cat',)")+8);
  sprintf(url_base, "javascript:p3item('cat',%d)", item_offset+nth);

  fputs("<ce:data><tr xmlns=\"http://www.w3.org/1999/xhtml\">",stdout);
  /*fprintf(stdout, "<td class=\"ce-gdf-icon\"><a href=\"%s\"><img src=\"/img/%s\" alt=\"%s in %s\"/></a></td>", url_base, icon, id, icon_alt);*/
  fprintf(stdout, "<td class=\"ce-ood-id\"><a href=\"javascript:p3item('ood',%d)\">%s</a></td>", item_offset+nth, (char*)hash_find(fields, "o:id"));
  for (i = 0; width_specs[i]; ++i)
    {
      List *tmp = field_lists[i];
      const char *pct = width_specs[i];
      const char *field, *value, *field_used = "";
      char pctbuf[4];
      int this_is_designation = 0;

      for (field = list_first(tmp); field; field = list_next(tmp))
	{
	  if ((value = hash_find(fields, (unsigned char *)field)))
	    {
	      field_used = field;
	      break;
	    }
	}
      if (!strcmp(field_used, "n"))
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

#include <unistd.h>
#include <locale.h>
#include <atflocale.h>
#include <psd_base.h>
#include <ctype128.h>
#include <dbi.h>

#include <index.h>
#include <alias.h>
#include <options.h>
#include <runexpat.h>
#include <list.h>
#include <fname.h>
#include "oracclocale.h"

#include "fields.h"
#include "property.h"
#include "se.h"
#include "selib.h"
#include "vid.h"

#include <sys/unistd.h>

extern int swc_flag;
extern int l2;

extern struct vid_data *vidp;

#ifndef strdup
extern char *strdup(const char *);
#endif

extern struct est *estp;

extern void grapheme_decr_start_column(void);
extern void grapheme_end_column_logo(void);
extern void grapheme_inherit_preceding_properties(void);
extern void grapheme_reset_start_column(void);

#undef xmalloc
#undef xrealloc
#undef xcalloc
#undef xfree
#define xmalloc malloc
#define xrealloc realloc
#define xcalloc calloc
#define xfree free

#include "addgraph.c"

extern Dbi_index *dip;

static int in_qualified = 0;
static int role_logo = 0;
extern char loc_project_buf[];

extern const char *curr_project, *curr_index, *proxies_arg;
Four_bytes curr_line;

struct location8 l8;

Two_bytes curr_properties;
int curr_text_id, curr_unit_id, curr_word_id;

extern int indexing;
extern int quiet;

extern const char **proxies;

extern FILE *f_mangletab;
extern FILE *signmap_err;

extern enum pending_boundary_types pending_boundary;

extern void grapheme_decr_start_column(void);
extern void signmap_init(void);
extern void signmap_term(Dbi_index *);

static void
do_boundary()
{
  switch (pending_boundary)
    {
    case pb_space:
      grapheme_boundary(' ');
      break;
    case pb_hyphen:
      grapheme_boundary('-');
      break;
    case pb_none:
      break;
    }
  pending_boundary = pb_none;
}

static const char *
pos(const char **atts)
{
  int i;
  for (i = 0; atts[i] != NULL; ++i)
    if (!strcmp(atts[i],"pos"))
      return atts[i+1];
  return "";
}

static void
pos_props(const char *pos)
{
  if (pos)
    {
      struct prop_tab *p = propmask(pos,strlen(pos));
      if (p)
	{
	  if (!p->prop)
	    {
	      if (strlen(pos) == 2)
		{
		  int p2 = 0;
		  if (pos[1] == 'N')
		    p2 = PROP_XN;
		  else
		    p2 = PROP_OL;
		  set_property(curr_properties,p2);
		}
	      else
		no_word_props(curr_properties);
	    }
	  else
	    set_property(curr_properties,p->prop);
	}
    }
  else
    no_word_props(curr_properties);
}

void
gdlStartElement(void *userData, const char *name, const char **atts)
{
  const char *v_delim = NULL, *data = NULL;
  switch (name[2])
    {
    case 'w':
      {
	static char qualified_id[128];
	const char *form;
	extern char curr_id[];
	pos_props(pos(atts));
	sprintf(qualified_id, "%s:%s", loc_project_buf, xml_id(atts));
	if (vidp)
	  wid2loc8(vid_map_id(vidp,qualified_id),xml_lang(atts),&l8);
	else
	  wid2loc8(curr_id,xml_lang(atts),&l8);
	form = attr_by_name(atts,"form");
	if (form)
	  est_add((const unsigned char*)attr_by_name(atts,"form"), estp);
	else
	  fprintf(stderr,"setxtx:warning:%s: word has NULL form attribute\n",
		  qualified_id);
	if (swc_flag)
	  {
	    swc_flag = 0;
	    grapheme_reset_start_column();
	  }
	else
	  {
	    const char *hw = findAttr(atts, "headform");
	    if (*hw)
	      {
		/*
		  fprintf(stderr, "setting swc_flag for %s\n", xml_id(atts));
		*/
		swc_flag = 1;
	      }
	  }
      }
      break;
    case 'v':
      v_delim = findAttr(atts, "g:delim");
      if (v_delim && *v_delim == ' ')
	pending_boundary = pb_space;
      else if (v_delim && *v_delim)
	pending_boundary = pb_hyphen;
      do_boundary();
      charData_discard();
      break;
    case 'd':
      do_boundary();
      begin_option();
      charData_discard();
      break;
    case 's':
      if (!strcmp(findAttr(atts,"g:role"),"logo"))
	role_logo = 1;
      do_boundary();
      charData_discard();
      break;
    case 'c':
    case 'n':
      do_boundary();
      charData_discard();
      data = (const char*)attr_by_name(atts,"form");
      grapheme(data);
      est_add((const unsigned char *)data, estp);
      break;
    case 'q':
      do_boundary();
      in_qualified = 1;
      break;
    }
}

void
gdlEndElement(void *userData, const char *name)
{
  extern Hash_table *signmap;
  switch (name[2])
    {
    case 'w':
      pending_boundary = pb_space;
      break;
    case 'q':
      in_qualified = 0;
      pending_boundary = pb_hyphen;
      break;
    case 'd':
      end_option();
      pending_boundary = pb_hyphen;
      break;
    case 'v':
    case 's':
      {
	Char *g = (Char*)charData_retrieve();
	const unsigned char *lg = NULL;
	
	grapheme((const char *)g);
	est_add(g, estp);
	if (name[2] == 's')
	  {
	    extern const unsigned char *utf_lcase(const unsigned char *);
	    unsigned char *s = NULL;
	    lg = utf_lcase((const unsigned char *)g);
	    if (lg)
	      {
		s = hash_find(signmap,lg);
		if (s)
		  {
		    if (strcmp((const char *)s,(const char *)g))
		      {
			progress("indexing sign value %s as sign %s\n",g, s);
			/* fprintf(stderr, "indexing sign value %s as sign %s\n", g, s); */
			grapheme_decr_start_column();
			grapheme((const char *)s);
			est_add(g, estp);
			grapheme_inherit_preceding_properties();
			do_boundary();
		      }
#if 0
		    else
		      fprintf(stderr, "%s == %s not double-indexed\n", s, g);
#endif
		  }
		else
		  fprintf(signmap_err, "%s not found in signmap\n", lg);
	      }
	    else
	      fprintf(stderr, "failed to lowercase %s\n", g);
	    if (role_logo)
	      grapheme_end_column_logo();
	  }
	else /* v */
	  {
	    Char *s = hash_find(signmap,g);
	    if (s)
	      {
		progress("indexing value %s as sign %s\n",g, s);
		grapheme_decr_start_column();
		grapheme((const char *)s);
		est_add(g, estp);
		grapheme_inherit_preceding_properties();
		do_boundary();
	      }
	  }
	if (!in_qualified)
	  pending_boundary = pb_hyphen;
      }
      break;
    case 'c':
    case 'n':
      if (!in_qualified)
	pending_boundary = pb_hyphen;
      break;
    }
}

void
add_graphemes ()
{
  struct grapheme *gnp;
  for (gnp = grapheme_list_base; gnp; gnp = gnp->next)
    {
      Uchar *tmp;
      if (!gnp->text)
	continue;
      if (aliases)
	{
	  tmp  = alias_fast_get_alias (gnp->text);
	  if (verbose && tmp && strcmp((const char*)tmp,(const char *)gnp->text))
	    fprintf(stderr,"%s indexed under alias '%s'\n", gnp->text, tmp);
	  addgraph (dip, tmp ? tmp : gnp->text, &gnp->node->l);
	}
      else
	addgraph (dip, gnp->text, &gnp->node->l);
    }
}

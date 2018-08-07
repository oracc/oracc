#include <ctype128.h>
#include <string.h>
#include <unistd.h>
#include "tree.h"
#include "warning.h"
#include "atf.h"
#include "cdf.h"
#include "hash.h"
#include "ilem.h"
#include "xcl.h"
#include "pool.h"
#include "tree.h"
#include "lemline.h"
#include "label.h"
#include "run.h"

#define xpool_copy(s) (pool_copy(ucc(s)))

extern const char *project;
int ignore_plus = 0;
int lem_status = 0;
extern const char *textid;
extern FILE *f_lemma;
static Hash_table *lemtab = NULL;
extern Hash_table *word_form_index;
extern int lem_standalone, v2;

extern int lemmata(struct xcl_context *xc, struct ilem_form *fp);
static void process(struct xcl_context *xc, struct node *n);

void
lem_init()
{
  lemtab = hash_create(4999);
}

void
lem_term()
{
  hash_free(lemtab,NULL);
  lemtab = NULL;
}

struct xcl_context *
xcl_process(struct run_context *run, struct node *text)
{
  struct xcl_context *xc = xcl_create();
  char *langs = texttag_langs();

  /*  xc->system = xcl_get_global_context()->system; */
  /*xc->cache = xcl_cache();*/
  xc->run = run;
  xc->curr = xc->root = NULL;
  xc->langs = (char*)npool_copy((unsigned char*)langs,xc->pool);
  free(langs);
  xc->project = project;
  xc->textid = textid;
  xc->file = file;
  xc->sigs = sig_context_init();
  process(xc,text);
  return xc;
}

static void
process(struct xcl_context *xc, struct node*n)
{
  int saved_status = status;
  int saved_exit_status = exit_status;
  static struct xcl_c *atpt_cell = NULL;
  static struct xcl_c *atpt_field = NULL;
  /* fprintf(stderr, "xtfxcl: process invoked\n"); */
  if (*n->type == 'e')
    {
      register int i;
      switch (n->etype)
	{
	case e_composite:
	case e_score:
	case e_transliteration:
	  xcl_chunk_id((char*)getAttr(n,"xml:id"),xcl_c_text,xc);
	  xcl_chunk(xc, NULL, xcl_c_text);
	  xc->curr->meta = xcl_hash_lemm_meta(lem_text_meta(),
					      lem_meta_id(),
					      xc);
	  break;
	case e_object:
	  /* fprintf(stderr, "xtfxcl: object found\n"); */
	  /* fix_context(xc,NULL); */
	  xcl_discontinuity(xc, (const char *)getAttr(n,"xml:id"), xcl_d_object, NULL);
	  break;
	case e_surface:
	  /* fix_context(xc,NULL); */
	  xcl_discontinuity(xc, (const char *)getAttr(n,"xml:id"), xcl_d_surface, NULL);
	  break;
	case e_column:
	  /* fix_context(xc,NULL); */
	  {
	    const char *ref = (const char *)getAttr(n,"xml:id");
	    if (*ref)
	      xcl_discontinuity(xc, ref, xcl_d_column, NULL);
	    /* no discontinuity emitted for implicit column breaks */
	  }
	  break;
	case e_m:
	  if (!xstrcmp(getAttr(n,"type"),"discourse"))
	    xcl_fix_context(xc,getAttr(n,"subtype"));
	  break;
	case e_l:
	case e_lg:
	  xcl_fix_context(xc,NULL);
	  xcl_discontinuity(xc, (const char *)getAttr(n,"xml:id"), xcl_d_line_start, NULL);
	  break;
	case e_g_w:
	case e_n_w:
	  xcl_fix_context(xc,NULL);
	  ilem_parse(xc, hash_find(word_form_index,getAttr(n,"xml:id")));
	  break;
	case e_g_nonw:
	  xcl_discontinuity(xc, (const char *)getAttr(n,"xml:id"), xcl_d_nonw, NULL);
	  break;
	case e_nonx:
	  xcl_discontinuity(xc, (const char *)getAttr(n,"xml:id"), xcl_d_nonx, NULL);
	  break;
	case e_c:
	  atpt_cell = xc->curr;
	  xcl_discontinuity(xc, NULL, xcl_d_cell_start, NULL);
	  break;
	case e_f:
	  atpt_field = xc->curr;
	  xcl_discontinuity(xc, NULL, xcl_d_field_start, (const char *)n->data);
	  break;
	default:
	  break;
	}
  
      for (i = 0; i < n->children.lastused; ++i)
	{
	  if (((struct node *)n->children.nodes[i])->etype != e_note_text)
	    process(xc,n->children.nodes[i]);
	}
      
      switch (n->etype)
	{
	case e_c:
	  if (atpt_cell != xc->curr)
	    {
	      struct xcl_c *save = xc->curr;
	      xc->curr = atpt_cell;
	      xcl_discontinuity(xc, NULL, xcl_d_cell_end, NULL);
	      xc->curr = save;
	      atpt_cell = NULL;
	    }
	  else
	    xcl_discontinuity(xc, NULL, xcl_d_cell_end, NULL);
	  break;
	case e_f:
	  if (atpt_field != xc->curr)
	    {
	      struct xcl_c *save = xc->curr;
	      xc->curr = atpt_field;
	      xcl_discontinuity(xc, NULL, xcl_d_field_end, (const char*)n->data);
	      xc->curr = save;
	      atpt_field = NULL;
	    }
	  else
	    xcl_discontinuity(xc, NULL, xcl_d_field_end, (const char*)n->data);
	  break;
	default:
	  break;
	}
    }

  lem_status += status;
  status = saved_status;
  exit_status = saved_exit_status;
}

static const unsigned char *
get_line_id(const char *ref)
{
  static char buf[64],*p;
  strcpy(buf,ref);
  p = strrchr(buf,'.');
  if (p)
    *p = '\0';
  return ucc(buf);
}

static const char *
first_l(struct xcl_c *c)
{
  const char *ret = NULL;
  int i;
  for (i = 0; i < c->nchildren; ++i)
    {
      if (c->children[i].c->node_type == xcl_node_l)
	return cc(label_from_line_id(get_line_id(c->children[i].l->ref)));
      else if (c->children[i].c->node_type == xcl_node_c
	       && (ret = first_l(c->children[i].c)))
	return ret;
    }
  return ret;
}

static const char *
last_l(struct xcl_c *c)
{
  const char *ret = NULL;
  int i;
  for (i = c->nchildren-1; i >= 0; --i)
    {
      if (c->children[i].c->node_type == xcl_node_l)
	return cc(label_from_line_id(get_line_id(c->children[i].l->ref)));
      else if (c->children[i].c->node_type == xcl_node_c
	       && (ret = last_l(c->children[i].c)))
	return ret;
    }
  return ret;
}

void
xcl_sentence_labels(struct xcl_context *xc, struct xcl_c *c)
{
  const char *first = NULL, *last = NULL;
  int n_lems = 0;

  if (c->type != xcl_c_sentence || !c->nchildren)
    return;

  first = first_l(c);
  last = last_l(c);

  if (first)
    {
      if (last && first != last)
	{
	  char *l = malloc(strlen(first)+strlen(last)+4);
	  sprintf(l,"%s - %s",first,last);
	  c->label = cc(pool_copy(ucc(l)));
	  free(l);
	}
      else
	c->label = cc(pool_copy(ucc(first)));
    }
  else if (n_lems) /* sentences that don't have lemmata don't matter */
    {
      vwarning("couldn't compute label for sentence with id=%s",c->ref);
    }
}

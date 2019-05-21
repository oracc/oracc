#include <stdlib.h>
#include "npool.h"
#include "ngram.h"
#include "ilem_form.h"
#include "f2.h"
#include "props.h"
#include "xcl.h"

struct prop_ref_cache prop_cache;

enum d_actions { d_action_nothing , d_action_skip_gloss , d_action_stop };

static int d_action(enum xcl_d_types d);
static void anno_target(struct xcl_l *target, struct xcl_context *xc, 
			struct match *mp, int matchlen,
			struct props_prop *pp, struct prop_ref_cache *cachep);
static struct xcl_l*find_next_nn(int nth, struct xcl_l*from, int max);
static struct xcl_l*find_prev_nn(int nth, struct xcl_l*from, int max);
static void prop_exec(struct xcl_context *xc, struct match *mp, int matchlen, 
		      struct props_prop *pp, struct prop_ref_cache *cachep);

void
props_exec(struct xcl_context *xc, const char *name,
	  props_analyzer analyzer)
{
  struct props_context *pcp = props_init(xc, name);
  if (pcp)
    {
      props_decorate(text_xc, pcp);
      if (analyzer)
	{
	  analyzer(text_xc, pcp);
	  xcl_map(text_xc,NULL,NULL,NULL,props_heads);
	}
    }
}

void
props_decorate(struct xcl_context *xc, struct props_context *pcp)
{
  if (pcp)
    {
      extern int ngramify_per_lang;
      ngramify_per_lang = 1;
      xc->props = pcp;
      xc->user = pcp->nlcps;
      xcl_map(xc,ngramify,NULL,NULL,NULL);
      ngramify_per_lang = 0;
    }
}

void
props_ngram_match(struct xcl_context *xc, struct ML *mlp)
{
  int i,j;
  for (i = 0; i < mlp->matches_used; ++i)
    {
      struct props_term *termp = mlp->matches->user;
      memset(&prop_cache,'\0',sizeof(struct prop_ref_cache));
      for (j = 0; j < termp->props_used; ++j)
	prop_exec(xc, &mlp->matches[i], mlp->matches_used, termp->props[j], &prop_cache);
    }
}

static int
d_action(enum xcl_d_types d)
{
  switch (d)
    {
    case xcl_d_punct: /* should this depend on a subtype? */
      return d_action_stop;
    case xcl_d_line_start:
    case xcl_d_break:
    case xcl_d_cell_start:
    case xcl_d_cell_end:
    case xcl_d_field_start:
    case xcl_d_field_end:
    case xcl_d_object:
    case xcl_d_surface:
    case xcl_d_column:
    case xcl_d_nonw:
    case xcl_d_nonx:
     return d_action_nothing;
    case xcl_d_gloss_start:
    case xcl_d_gloss_end:
    case xcl_d_top:
      return d_action_skip_gloss;
      /* no default */
    }
  return d_action_nothing;
}

static int
d_skip(struct xcl_c *p, int start, enum xcl_d_types stop)
{
  for (; start < p->nchildren; ++start)
    {
      if (p->children[start].c->node_type == xcl_node_d
	  && p->children[start].d->type == stop)
	return start;
    }
  return start;
}

static int
d_skip_back(struct xcl_c *p, int start, enum xcl_d_types stop)
{
  for (; start; --start)
    {
      if (p->children[start].c->node_type == xcl_node_d
	  && p->children[start].d->type == stop)
	return start;
    }
  return start;
}

static struct xcl_l*
find_next_nn(int nth, struct xcl_l*from, int maxlem)
{
  int start, max;
  struct xcl_c *p = from->parent;

  for (start = from->nth + 1, max = maxlem; start < p->nchildren; ++start)
    {
      if (p->children[start].c->node_type == xcl_node_d)
	{
	  switch (d_action(p->children[start].d->type))
	    {
	    case d_action_skip_gloss:
	      start = d_skip(p, start, xcl_d_gloss_end);
	      break;
	    case d_action_stop:
	      return NULL;
	    default:
	      /* do nothing */
	      break;
	    }
	}
      else
	{
	  const char *fpos = NULL;
	  if (p->children[start].c
	      && p->children[start].c->node_type == xcl_node_l)
	    fpos = (const char *)p->children[start].l->f->f2.pos;
	  if (fpos)
	    {
	      if (!strcmp(fpos,"LN") || !strcmp(fpos,"PN") || !strcmp(fpos,"RN"))
		{
		  if (--nth == 0)
		    return p->children[start].l;
		}
	      else
		{
		  if (--max == 0)
		    break;
		}
	    }
	}
    }
  return NULL;
}

static struct xcl_l*
find_prev_nn(int nth, struct xcl_l*from, int max)
{
  int start;
  struct xcl_c *p = from->parent;

  for (start = from->nth - 1; start >= 0; --start)
    {
      if (p->children[start].c->node_type == xcl_node_d)
	{
	  switch (d_action(p->children[start].d->type))
	    {
	    case d_action_skip_gloss:
	      start = d_skip_back(p, start, xcl_d_gloss_start);
	      break;
	    case d_action_stop:
	      return NULL;
	    default:
	      /* do nothing */
	      break;
	    }
	}
      else
	{
	  const char *fpos = (const char *)p->children[start].l->f->f2.pos;
	  if (fpos)
	    {
	      if (!strcmp(fpos,"LN") || !strcmp(fpos,"PN") || !strcmp(fpos,"RN"))
		{
		  if (--nth == 0)
		    return p->children[start].l;
		}
	      else if (--max == 0)
		break;
	    }
	}
    }
  return NULL;
}

struct xcl_l *
props_find_node(struct prop_ref_cache *cachep, struct xcl_l *start, 
		int where, int maxlem)
{
  struct xcl_l *tmp = NULL;
  if (where == -100000)
    ;
  else if (where < 0 && (!cachep || !(tmp = cachep->prevs[(where*-1)-1])))
    {
      tmp = find_prev_nn(where*-1,start,maxlem);
      if (cachep)
	cachep->prevs[(where*-1)-1] = tmp;
    }
  else if (where > 0 && (!cachep || !(tmp = cachep->nexts[where-1])))
    {
      tmp = find_next_nn(where,start,maxlem);
      if (cachep)
	cachep->nexts[where-1] = tmp;
    }
  else if (where == 0)
    tmp = start;

  return tmp;
}

static void
prop_exec(struct xcl_context *xc, struct match *mp, int matchlen,
	  struct props_prop *pp, struct prop_ref_cache *cachep)
{
  if (pp->name)
    {
#if 0
      struct xff_propdef *pd;
      if (!(pd = xff_auto_prop_name(pp->name)))
	{
	  fprintf(stderr,"props: property %s not predeclared in group\n",pp->name);
	  return;
	}
      else
	pp->group = pd->group;
#endif
    }

  if (pp->where == -100000)
    props_head_cue(xc, xc->props, mp->lp, pp);
  else if (pp->where > 0)
    anno_target(props_find_node(cachep, mp->lp, pp->where, pp->maxlem+matchlen),
		xc, mp, matchlen, pp, cachep);
  else if (pp->where == -2)
    {
      struct xcl_l *start = mp->lp;
      while (1)
	{
	  struct xcl_l *found = NULL;
	  if (start->nth > 0
	      && (found = find_prev_nn(1, start, 1)))
	    {
	      anno_target(found, xc, mp, matchlen, pp, cachep);
	      start = found;
	    }
	  else
	    break;
	}
    }
  else
    anno_target(props_find_node(cachep, mp->lp, pp->where, pp->maxlem),
		xc, mp, matchlen, pp, cachep);
}

static void
anno_target(struct xcl_l *target, struct xcl_context *xc, 
	    struct match *mp, int matchlen,
	    struct props_prop *pp, struct prop_ref_cache *cachep)
{
  struct xcl_l *ref = NULL;
  const unsigned char *value = NULL, *idref = NULL;

  if (target)
    {
      if (*pp->value == '#')
	{
	  int where = atoi((const char *)&pp->value[1]);
	  if (where == -100000)
	    props_head_cue(xc, xc->props, mp->lp, pp);
	  else if (where > 0)
	    ref = props_find_node(cachep, mp->lp, where, pp->maxlem+matchlen);
	  else
	    ref = props_find_node(cachep, mp->lp, where, pp->maxlem);

	  if (ref)
	    {
	      char buf[128];
	      sprintf(buf,"#%s",ref->xml_id);
	      idref = npool_copy((unsigned char *)buf,xc->pool);
	      sprintf(buf,"%s[%s]",ref->f->f2.cf,ref->f->f2.gw);
	      value = npool_copy((unsigned char *)buf,xc->pool);
	    }
	}
      else
	value = pp->value;
      if (value || idref)
	{
	  props_add_prop(target->f,pp->group,pp->name,value,(const char*)idref,
			 NULL,NULL,get_ngram_id());
	}
    }
}

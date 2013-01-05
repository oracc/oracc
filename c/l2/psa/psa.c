#include <stdlib.h>
#include "ngram.h"
#include "ilem_form.h"
#include "xcl.h"
#include "psa.h"
#include "props.h"

/*
  Annotate an XCL tree for person-name info.  We work in three
  passes:

  1) group terms which refer to a person-spec that has 
     genealogical information; include professional
     designations here
  2) wrap PN/RN in person-spec if they are at top level;
     also include professional designations here
  3) group person-specs that are joined by horizontal 
     relators like wife/husband/brother/sister
 
  After this, all PNs are wrapped in person-spec, sometimes
  two levels deep.
  */

static void nentify_1(struct xcl_context *,struct xcl_c*);
static void nentify_2(struct xcl_context *,struct xcl_c*);
static void nentify_3(struct xcl_context *,struct xcl_c*);
static void psa_analyzer(struct xcl_context *xc, struct props_context *pcp);

#if 0
static int
groups_with(struct xcl_l *lp, const char *id)
{
  struct xff_prop *xffp = props_find_prop(lp->f,(unsigned char *)"group-with",NULL);
  if (xffp)
    return !strcmp((char*)(xffp->ref+1), id);
  else
    return 0;
}

static int
is_group_head(struct xcl_l *lp)
{
  return props_find_prop(lp->f,(unsigned char *)"group-head",NULL) != NULL;
}

static const char *
has_genealogy_ref(struct ilem_form *f)
{
  struct xff_prop *p = props_find_prop(f,(unsigned char *)"father-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"ancestor-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"mother-is",NULL);
  if (p && p->ref)
    return p->ref;
  else
    return NULL;
}

#endif

static int
is_pn_pos(const char *pos)
{
  return pos && pos[1] == 'N' && (pos[0] == 'L' || pos[0] == 'P' || pos[0] == 'R');
}

void
psa_exec(struct xcl_context *xc)
{
  props_exec(xc,"psa",psa_analyzer);
}

static void
psa_analyzer(struct xcl_context *xc, struct props_context *pcp)
{
  if (pcp)
    {
      Hash_table *titles = NULL;
      xc->props = pcp;
      if (!xc->linkbase)
	xc->linkbase = new_linkbase();
      xc->user = xc->props->nlcps;
      xcl_map(xc,nentify_1,NULL,NULL,NULL);
      titles = hash_from_var_char_star("person-spec", NULL);
      xcl_chunkify_part(xc, titles, NULL, NULL);
      xcl_map(xc,nentify_2,NULL,NULL,NULL);
      xcl_map(xc,nentify_3,NULL,NULL,NULL);
    }
}

static int
find_ref_node(const char *lref,struct xcl_c *cp,int i)
{
  while (i < cp->nchildren)
    {
      if (cp->children[i].c->node_type == xcl_node_l
	  && !strcmp(cp->children[i].l->xml_id,(const char *)(lref+1)))
	return i;
      else
	++i;
    }
  return cp->nchildren; /* can't happen ... */
}

static int
has_marker(struct ilem_form *f)
{
  return props_find_prop_group(f,(unsigned char*)"marker") != NULL;
}

static int
has_h_relator(struct ilem_form *f)
{
  struct prop *p = props_find_prop(f,(unsigned char *)"relator",NULL);
  return (p && p->value) ? (!strcmp((char *)p->value,"h")) : 0;
}

static int
has_v_relator(struct ilem_form *f)
{
  struct prop *p = props_find_prop(f,(unsigned char *)"relator",NULL);
  return (p && p->value) ? (!strcmp((char *)p->value,"v")) : 0;
}

static const char *
has_V_qualifier_ref(struct ilem_form *f)
{
  struct prop *p = NULL;

  if (!p)
    p = props_find_prop(f,(unsigned char *)"father-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"mother-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"ancestor-is",NULL);

  if (p && p->ref)
    return p->ref;
  else
    return NULL;
}

#if 0
static const char *
has_H_qualifier_ref(struct ilem_form *f)
{
  struct prop *p = props_find_prop(f,(unsigned char *)"husband-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"wife-is",NULL);
  if (!p)
    p = props_find_prop(f,(unsigned char *)"spouse-is",NULL);
  if (p && p->ref)
    return p->ref;
  else
    return NULL;
}
#endif

static void
great_check(struct prop *child_is, struct xcl_l *lp)
{
  struct prop *parent_is = NULL;
  if ((parent_is = props_find_prop(lp->f, (unsigned char *)"father-is", NULL)))
    {
      struct xcl_c *tmp;
      int i;
      for (i = lp->nth, tmp = lp->parent; i >= 0; --i)
	if (tmp->children[i].c->node_type == xcl_node_l
	    && !strcmp(tmp->children[i].l->xml_id,child_is->ref+1))
	  break;
      if (i >= 0)
	{
	  props_add_prop(tmp->children[i].l->f, (const unsigned char *)"reln",
			 (const unsigned char *)"grandfather-is",
			 parent_is->value, parent_is->ref,NULL,NULL,
			 -1);
	  for (i = lp->nth, tmp = lp->parent; i < tmp->nchildren; ++i)
	    if (tmp->children[i].c->node_type == xcl_node_l
		&& !strcmp(tmp->children[i].l->xml_id,parent_is->ref+1))
	      break;
	  props_add_prop(tmp->children[i].l->f,
			 (const unsigned char *)"reln",
			 (const unsigned char *)"grandchild-is",
			 child_is->value, child_is->ref,NULL,NULL,
			 -1);
	}
    }
  else if ((parent_is = props_find_prop(lp->f, (unsigned char *)"mother-is", NULL)))
    {
      struct xcl_c *tmp;
      int i;
      for (i = lp->nth, tmp = lp->parent; i >= 0; --i)
	if (tmp->children[i].c->node_type == xcl_node_l
	    && !strcmp(tmp->children[i].l->xml_id,child_is->ref+1))
	  break;
      if (i >= 0)
	{
	  props_add_prop(tmp->children[i].l->f, (const unsigned char *)"reln",
			 (const unsigned char *)"grandmother-is",
			 parent_is->value, parent_is->ref,NULL,NULL,
			 -1);
	  for (i = lp->nth, tmp = lp->parent; i < tmp->nchildren; ++i)
	    if (tmp->children[i].c->node_type == xcl_node_l
		&& !strcmp(tmp->children[i].l->xml_id,parent_is->ref+1))
	      break;
	  props_add_prop(tmp->children[i].l->f,
			 (const unsigned char *)"reln",
			 (const unsigned char *)"grandchild-is",
			 child_is->value, child_is->ref,NULL,NULL,
			 -1);
	}
    }
}

static void
nentify_1(struct xcl_context *xc,struct xcl_c *cp)
{
  int i = 0, last_nent_end = 0;
  for (i = 0; i < cp->nchildren; ++i)
    {
      if (cp->children[i].c->node_type == xcl_node_l
	  && cp->children[i].l->f->f2.pos
	  && is_pn_pos((char*)cp->children[i].l->f->f2.pos))
	{
	  int nent_begin = i, nent_end = -1;
	  const char *lref;

	  while (1)
	    {
	      while (i < cp->nchildren
		     && cp->children[i].c->node_type == xcl_node_d)
		++i; /*FIXME: this should use the d_actions */
	      if (i == cp->nchildren)
		break;

	      lref = has_V_qualifier_ref(cp->children[i].l->f);

	      if (lref)
		{
		  int new_nent_end;
		  struct prop *child_is = NULL;

		  new_nent_end = find_ref_node(lref, cp, i+1);
		  if (new_nent_end == i)
		    break;
		  else
		    i = new_nent_end;

		  /* FIXME: should this catch great-grand also? */
		  if ((child_is = props_find_prop(cp->children[i].l->f, 
						  (unsigned char *)"child-is", NULL)))
		    great_check(child_is, cp->children[i].l);
		    
		  if (i >= cp->nchildren)
		    break;
		  else
		    nent_end = new_nent_end;
		}
	      else
		{
		  int entry = 0;

		  while ((i+1) < cp->nchildren
			 && cp->children[i+1].c->node_type == xcl_node_d)
		    ++i; /*FIXME: this should use the d_actions */

		  /* don't pick up trailing discontinuities */
		  entry = i;

		  while ((i+1) < cp->nchildren
			 && cp->children[i+1].c->node_type == xcl_node_l
			 && has_marker(cp->children[i+1].l->f)
			 && !has_h_relator(cp->children[i+1].l->f))
		    {
		      ++i;
		      if (has_v_relator(cp->children[i].l->f)
			  && (i+1) < cp->nchildren
			  && cp->children[i+1].c->node_type == xcl_node_l
			  && (is_pn_pos((char*)cp->children[i+1].l->f->f2.epos)))
			++i;
		    }
		  if (i == entry)
		    break;
		  else
		    nent_end = i;
		}
	    }

	  if (nent_begin 
	      && (nent_begin-1) > last_nent_end
	      && cp->children[nent_begin-1].c->node_type == xcl_node_l
	      && has_marker(cp->children[nent_begin-1].l->f)
	      && !has_h_relator(cp->children[nent_begin-1].l->f))
	    --nent_begin;
	  else
	    props_head_reg(xc->props, cp->children[nent_begin].l);

#if 0
	  if (is_group_head(cp->children[nent_begin].l))
	    {
	      const char *refid = cp->children[nent_begin].l->xml_id;
	      int group_begin;
	      for (group_begin = 0; group_begin < nent_begin; ++group_begin)
		{
		  if (cp->children[group_begin].c->node_type == xcl_node_l
		      && groups_with(cp->children[group_begin].l,refid))
		    break;
		}
	      props_link(cp, group_begin, nent_begin, "psa", "person-group");
	      /* nent_begin = group_begin; */
	    }
#endif

	  props_link(cp, nent_begin, nent_end, "psa", "person-spec");
	}
    }
}

static void
nentify_2(struct xcl_context *xc,struct xcl_c *cp)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      if (cp->children[i].c->node_type == xcl_node_l
	  && cp->children[i].l->f->f2.pos
	  && is_pn_pos((char*)cp->children[i].l->f->f2.pos))
	props_link(cp, i, i, "psa", "person-name");
    }	
}

static void
nentify_3(struct xcl_context *xc,struct xcl_c *cp)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      if (cp->children[i].c->node_type == xcl_node_l
	  && cp->children[i].l->f
	  && cp->children[i].l->f->props
	  && has_h_relator(cp->children[i].l->f))
	{
	  if (i)
	    {
	      int start = i-1;
	      while (cp->children[start].c->node_type == xcl_node_d)
		--start;
	      if (cp->children[start].c->node_type == xcl_node_c
		  && cp->children[start].c->type == xcl_c_phrase
		  && !strncmp(cp->children[start].c->subtype, "person-", 7))
		{
		  while (i < cp->nchildren
			 && ((cp->children[i].c->node_type == xcl_node_l
			      && has_h_relator(cp->children[i].l->f))
			     || cp->children[i].c->node_type == xcl_node_d))
		    ++i;
		  if (cp->children[start].c->node_type == xcl_node_c
		      && cp->children[start].c->type == xcl_c_phrase
		      && !strncmp(cp->children[start].c->subtype, "person-", 7))
		    props_link(cp, start, i, "psa", "person-co-spec");		    
		}
	    }
	}
    }
}

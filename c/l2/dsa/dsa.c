#include "props.h"

static void date_links(struct xcl_context *xc, struct xcl_c *cp);
static void dsa_analyzer(struct xcl_context *xc, struct props_context *pcp);

void
dsa_exec(struct xcl_context *xc)
{
  props_exec(xc,"dsa",dsa_analyzer);
}

static void
dsa_analyzer(struct xcl_context *xc, struct props_context *pcp)
{
  if (pcp)
    {
      xc->props = pcp;
      if (!xc->linkbase)
	xc->linkbase = new_linkbase();
      xc->user = xc->props->nlcps;
      xcl_map(xc,date_links,NULL,NULL,NULL);
    }
}

static void
date_links(struct xcl_context *xc, struct xcl_c *cp)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      if (cp->children[i].c->node_type == xcl_node_l
	  && props_find_prop_group(cp->children[i].l->f,(unsigned char*)"dsa"))
	{
	  int dsa_begin = i, dsa_end = -1;
	  while (1)
	    {
	      if (i == cp->nchildren)
		break;
	      else if (cp->children[i].c->node_type == xcl_node_d)
		++i;
	      else if (cp->children[i].c->node_type == xcl_node_l
		       && props_find_prop_group(cp->children[i].l->f,(unsigned char*)"dsa"))
		{
		  dsa_end = i++;
		}
	      else
		break;
	    }
	  props_link(cp, dsa_begin, dsa_end, "dsa", "date");
	}
    }

}

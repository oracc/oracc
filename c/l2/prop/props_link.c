#include "xcl.h"
#include "props.h"

static void
props_one_link_c(struct linkset *lsp,int link_index,struct xcl_c *cp)
{
  struct link *l;
  l = &lsp->links[link_index];
  l->lref = cp->id;
  l->cp = cp;
  l->lp = NULL;
  l->role = "name";
  l->title = link_index ? "end" : "start";
}

static void
props_one_link_l(struct linkset *lsp,int link_index,struct xcl_l *lemp)
{
  struct link *l;
  l = &lsp->links[link_index];
  l->lref = lemp->xml_id;
  l->lp = lemp;
  l->cp = NULL;
  l->role = "name";
  l->title = link_index ? "end" : "start";
}

void
props_link(struct xcl_c *cp, int begin, int end, const char *name, const char *role)
{
  struct linkset *lsp;
  lsp = new_linkset(((struct xcl_context*)cp->xc)->linkbase,
		    name, 
		    role);
  if (end > 0)
    {
      preallocate_links(lsp,2);
      switch (cp->children[begin].c->node_type)
	{
	case xcl_node_l:
	  props_one_link_l(lsp,0,cp->children[begin].l);
	  break;
	case xcl_node_c:
	  props_one_link_c(lsp,0,cp->children[begin].c);
	  break;
	default:
	  fprintf(stderr,"props_link[begin]: don't grok node_type == d yet\n");
	  break;
	}
      switch (cp->children[end].c->node_type)
	{
	case xcl_node_l:
	  props_one_link_l(lsp,1,cp->children[end].l);
	  break;
	case xcl_node_c:
	  props_one_link_c(lsp,1,cp->children[end].c);
	  break;
	default:
	  fprintf(stderr,"props_link[end]: don't grok node_type == d yet\n");
	  break;
	}
      lsp->used = 2;
    }
  else
    {
      preallocate_links(lsp,1);
      switch (cp->children[begin].c->node_type)
	{
	case xcl_node_l:
	  props_one_link_l(lsp,0,cp->children[begin].l);
	  break;
	case xcl_node_c:
	  props_one_link_c(lsp,0,cp->children[begin].c);
	  break;
	default:
	  fprintf(stderr,"props_link[single]: don't grok node_type == d yet\n");
	  break;
	}
      lsp->used = 1;
    }
}

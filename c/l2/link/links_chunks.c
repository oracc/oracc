#include "xcl.h"
#include "links.h"

static struct xcl_l *links_xcl_head(union xcl_u *xu);
static void set_cp(struct xcl_chunk_spec *cp, struct linkset *lsp, const char *type);

static int
chunk_spec_cmp(struct xcl_chunk_spec*a, struct xcl_chunk_spec*b)
{
  int ret = strcmp(a->sort_from,b->sort_from); /* first in doc order comes first */
  if (ret)
    return ret;
  return strcmp(b->sort_to,a->sort_to); /* end point that is last in doc order comes first */
}

static const char *
linkset_type(struct linkset *lsp)
{
  const char *role = lsp->role;
  if (!strcmp(role,"dsa"))
    return "date";
  else if (!strcmp(role,"psa"))
    {
      return lsp->title;
    }
  else if (!strcmp(role,"nsa"))
    return "measure";
  else if (!strcmp(role,"psu"))
    return "phr";
  else
    return NULL;
}

struct xcl_chunk_spec *
links_chunks(struct linkbase *lbp, int with_chunkified)
{
  struct xcl_chunk_spec *cp = NULL;
  int i = 0;
  if (lbp && lbp->nsets)
    {
      struct linkset *lsp;
      cp = calloc(lbp->nsets+1, sizeof(struct xcl_chunk_spec));
      for (lsp = lbp->first; lsp; lsp = lsp->next)
	{
	  if (with_chunkified || !lsp->chunkified)
	    {
	      const char *type = linkset_type(lsp);
	      if (type && lsp->links)
		set_cp(&cp[i++],lsp,type);
	    }
	}
    }
  qsort(cp, i, sizeof(struct xcl_chunk_spec), 
	(int (*)(const void*,const void*))chunk_spec_cmp);
  return cp;
}

struct xcl_chunk_spec *
links_chunks_part(struct linkbase *lbp,
		  Hash_table *titles, Hash_table *roles, Hash_table *lnodes,
		  int with_chunkified)
{
  struct xcl_chunk_spec *cp = NULL;
  int i = 0;
  if (lbp && lbp->nsets)
    {
      struct linkset *lsp;
      cp = calloc(lbp->nsets+1, sizeof(struct xcl_chunk_spec));
      for (lsp = lbp->first; lsp; lsp = lsp->next)
	{
	  if (with_chunkified || !lsp->chunkified)
	    {
	      const char *title = lsp->title;
	      const char *role = lsp->role;
	      if ((!titles || (title && hash_find(titles,(unsigned char *)title)))
		  && (!roles || (role && hash_find(roles,(unsigned char *)role)))
		  && lsp->links)
		{
		  if (lsp->links[0].lref)
		    {
		      if (!lnodes 
			  || (hash_find(lnodes, (unsigned char *)lsp->links[0].lref)
			      && (!lsp->links[lsp->used-1].lref
				  || hash_find(lnodes, (unsigned char *)lsp->links[lsp->used-1].lref))))
			{
			  set_cp(&cp[i++], lsp, lsp->title);
#if 0
			  struct xcl_chunk_spec *currcp = &cp[i++];
			  currcp->lsp = lsp;
			  currcp->from_id = lsp->links[0].lref;
			  currcp->to_id = lsp->links[lsp->used-1].lref;
			  
			  currcp->ref = lsp->xml_id;
			  currcp->type = lsp->title;
			  currcp->subtype = lsp->role;
			  if (!currcp->from_id)
			    currcp->from_id = lsp->xml_id;
			  if (!currcp->to_id)
			    currcp->to_id = currcp->from_id;
#endif
			}
		    }
		}
	    }
	}
    }
  qsort(cp, i, sizeof(struct xcl_chunk_spec), 
	(int (*)(const void*,const void*))chunk_spec_cmp);
  return cp;
}

/* this is a copy of links_xcl_head to work around linking
   problems */
static struct xcl_l *
links_xcl_head(union xcl_u *xu)
{
  struct xcl_l *ret = NULL;
  int i;

  switch (xu->c->node_type)
    {
    case xcl_node_c:
      for (i = 0; i < xu->c->nchildren; ++i)
	{
	  if ((ret = links_xcl_head(&xu->c->children[i])))
	    return ret;
	}
      break;
    case xcl_node_d:
      return NULL;
      break;
    case xcl_node_l:
      return xu->l;
      break;
    }
  return NULL;
}

static void
set_cp(struct xcl_chunk_spec *currcp, struct linkset *lsp, const char *type)
{
  currcp->lsp = lsp;
  currcp->ref = lsp->xml_id;
  currcp->type = type;
  currcp->subtype = lsp->role;
  currcp->from_id = lsp->links[0].lref;
  currcp->to_id = lsp->links[lsp->used-1].lref;
  if (!currcp->from_id)
    currcp->from_id = lsp->xml_id;
  if (!currcp->to_id)
    currcp->to_id = currcp->from_id;
  if (lsp->links[0].lp)
    {
      currcp->sort_from = currcp->from_id;
    }
  else
    {
      struct xcl_l *lp = links_xcl_head((union xcl_u*)(&lsp->links[0].cp));
      if (lp)
	currcp->sort_from = lp->xml_id;
      else
	currcp->sort_from = currcp->from_id;
    }
  if (lsp->links[lsp->used-1].lp)
    {
      currcp->sort_to = currcp->to_id;	
    }
  else
    {
      struct xcl_l *lp = links_xcl_head((union xcl_u *)(&lsp->links[0].cp));
      if (lp)
	currcp->sort_to = lp->xml_id;
      else
	currcp->sort_to = currcp->to_id;
    }
}

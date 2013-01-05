#include "xcl.h"
#include "links.h"

struct xcl_chunk_spec *
link_chunks(struct linkbase *lbp)
{
  struct xcl_chunk_spec *cp = NULL;
  if (lbp && lbp->nsets)
    {
      struct linkset *lsp;
      int i = 0;
      cp = calloc(lsp->nsets, sizeof(struct xcl_chunk_spec));
      for (lsp = lbp->first; lsp; lsp = lsp->next)
	{
	  if (lsp->links)
	    {
	      struct xcl_chunk_spec *currcp = &cp[i];
	      struct link *linkp = NULL;
	      currcp->ref = lsp->xml_id;
	      currcp->type = lsp->role;
	      currcp->first = lsp->links->lref;
	      while (linkp)
		{
		  if (!linkp->next)
		    {
		      currcp->last = linkp->lref;
		      break;
		    }
		  else
		    linkp = linkp->next;
		}
	    }
	}
    }
  return cp;
}

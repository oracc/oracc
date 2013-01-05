#include <stdlib.h>
#include <stdio.h>
#include "links.h"

struct linkbase *
new_linkbase(void)
{
  return calloc(1,sizeof(struct linkbase));
}

struct linkset *
new_linkset(struct linkbase *lbp, const char *role, const char *title)
{
  struct linkset *lsp = calloc(1,sizeof(struct linkset));
  if (!lbp->first)
    lbp->first = lbp->last = lsp;
  else
    {
      lbp->last->next = lsp;
      lbp->last = lsp;
    }
  sprintf(lsp->xml_id,"%s.ls%05x",lbp->textid,lbp->idcounter++);
  lsp->role = role;
  lsp->title = title;
  ++lbp->nsets;
  return lsp;
}

void
preallocate_links(struct linkset*lsp,int nlinks)
{
  lsp->links = calloc(1,nlinks * sizeof(struct link));
  lsp->alloced = nlinks;
}

struct link *
new_link(struct linkset *lsp, const char *role, const char *title)
{
  /* most links will be doubletons, so allocing 2 at a time is reasonable 
     (this might change when number parsing is implemented) */
  if (lsp->used == lsp->alloced)
    lsp->links = realloc(lsp->links, (lsp->alloced += 2) * sizeof(struct link));
  return &lsp->links[lsp->used++];
}

/* Do we need to have a user-free routine also? */
void
linkbase_free(struct linkbase *lbp)
{
  if (lbp)
    {
      struct linkset *lsp, *tmp;
      for (lsp = lbp->first; lsp; lsp = tmp)
	{
	  free(lsp->links);
	  tmp = lsp->next;
	  free(lsp);
	}
      free(lbp);
    }
}

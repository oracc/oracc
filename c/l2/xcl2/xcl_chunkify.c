#include "xcl.h"

static int
demote(struct xcl_context *xc, struct xcl_c *cp, int i,
       struct xcl_chunk_spec *chunk)
{
  int n_nodes;
  int first = i;
  for (n_nodes = 0; i < cp->nchildren; ++i, ++n_nodes)
    {
      if ((cp->children[i].c->node_type == xcl_node_l
	   && (chunk->to_id && !strcmp(cp->children[i].l->xml_id,chunk->to_id)))
	  || (cp->children[i].c->node_type == xcl_node_c
	      && (chunk->to_id && !strcmp(cp->children[i].c->id,chunk->to_id))))
	{
	  ++n_nodes;
	  break;
	}
    }
  if (i < cp->nchildren || cp->subtype)
    {
      xcl_chunk_insert(cp,NULL,chunk->ref,xcl_c_phrase,chunk->type,first,n_nodes);
      return 1;
    }
  else
    {
      cp->subtype = chunk->type;
      return 0;
    }
}

static void
xcl_chunkify_one(struct xcl_context *xc, struct xcl_c *cp, 
		 struct xcl_chunk_spec *chunks, int *cindex)
{
  int i;
  for (i = 0; i < cp->nchildren; ++i)
    {
      if (!chunks[*cindex].lsp)
	return;
      switch (cp->children[i].c->node_type)
	{
	case xcl_node_c:
	  if (!chunks[*cindex].lsp->chunkified
	      && !strcmp(cp->children[i].c->id,chunks[*cindex].from_id))
	    {
	      if (demote(xc, cp, i, &chunks[*cindex]))
		{
		  --i; /* ensure we process the new chunk */
		  chunks[*cindex].lsp->chunkified = 1;
		}
	      *cindex = *cindex+1;
	    }
	  else
	    {
	      int entry_cindex = *cindex;
	      xcl_chunkify_one(xc,cp->children[i].c, chunks, cindex);
	      if (*cindex > entry_cindex)
		--i;
	    }

	  if (!chunks[*cindex].from_id)
	    goto ret;
	  break;
	case xcl_node_l:
	  if (!chunks[*cindex].lsp->chunkified
	      && !strcmp(cp->children[i].l->xml_id,chunks[*cindex].from_id))
	    {
	      if (demote(xc, cp, i, &chunks[*cindex]))
		{
		  --i; /* ensure we process the new chunk */
		  chunks[*cindex].lsp->chunkified = 1;
		}
	      *cindex = *cindex+1;
	      if (!chunks[*cindex].from_id)
		goto ret;
	    }
	  break;
	case xcl_node_d:
	  break;
	}
    }
 ret:
  (void)0;
}

void
xcl_chunkify(struct xcl_context *xc)
{
  struct xcl_chunk_spec *cs;
  int cindex = 0;
  FILE *clog;
  if (xc && xc->linkbase && xc->linkbase->nsets)
    {
      cs = links_chunks(xc->linkbase,0);
      xcl_chunkify_one(xc,xc->root, cs, &cindex);
      free(cs);
    }
#if 0 /* note that x2_serialize removes levels from the xcl tree */
  if ((clog = fopen("chunkify.log", "w")))
    {
      x2_serialize(xc, clog, 0);
      fclose(clog);
    }
#endif
}

void
xcl_chunkify_part(struct xcl_context *xc, 
		  Hash_table *titles, Hash_table *roles, Hash_table *lnodes)
{
  struct xcl_chunk_spec *cs;
  int cindex = 0;
  if (xc && xc->linkbase && xc->linkbase->nsets)
    {
      cs = links_chunks_part(xc->linkbase, titles, roles, lnodes, 0);
      xcl_chunkify_one(xc,xc->root, cs, &cindex);
      free(cs);
    }
}

Hash_table *
xcl_lnodes_hash(struct xcl_c *c)
{
  return NULL;
}

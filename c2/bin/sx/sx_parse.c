#include <tree.h>
#include <cat.h>
#include <gdl.h>
#include "sx.h"

void
sx_gparse(Node *np, char *data)
{
  Tree *tp = gdlparse_string(data);
  tree_graft(np, tp);
  gdlparse_reset();
  
}

/* This routine needs to wrap the preamble and/or postamble that some
   @v nodes have in (#...#) or the like before passing them to the gdl
   parser */   
void
sx_vpp_parse(Node *np, char *data)
{
  sx_gparse(np, data);
}

#if 0
void
sx_p_sign(Node *np, char *data)
{
  Tree *tp = gdlparse_string(data);
  node_graft_tree(np, tp);
#if 0  
  /* assert(!tp->root->kids->next) */
  np->data = tp->root/*->kids*/; /* root will be g:gdl, data data will begin at root->kids */
  tree_ns_merge(np->tree, tp->ns_used);
#endif
  gdlparse_reset();
}

void
sx_p_form(Node *np, char *data)
{
  Tree *tp = gdlparse_string(data);
  /* assert(!tp->root->kids->next) */
  np->data = tp->root/*->kids*/; /* root will be g:gdl, data data will begin at root->kids */
  tree_ns_merge(np->tree, tp->ns_used);
  gdlparse_reset();
}

void
sx_p_v(Node *np, char *data)
{
  Tree *tp = gdlparse_string(data);
  /* assert(!tp->root->kids->next) */
  np->data = tp->root/*->kids*/; /* root will be g:gdl, data data will begin at root->kids */
  tree_ns_merge(np->tree, tp->ns_used);
  gdlparse_reset();
}
#endif

#include <tree.h>
#include <cat.h>
#include <gdl.h>
#include "sx.h"

void
sx_p_sign(Node *np, char *data)
{
  Tree *tp = gdlparse_string(data);
  /* assert(!tp->root->kids->next) */
  np->data = tp->root/*->kids*/; /* root will be g:gdl, data data will begin at root->kids */
  tree_ns_merge(np->tree, tp->ns_used);
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

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "cat.h"

/* Debugging tool to output generic XML dump for tree generated via catherd */

void
catxml_node(Node *np, void *user)
{
  fprintf(stderr, "name=%s; data=%s\n", np->name, np->data);
}

void
cat_xml(FILE *fp, Tree *tp)
{
  if (!fp)
    fp = stdout;
  tree_iterator(tp, catxml_node, NULL);
}

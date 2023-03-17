#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "cat.h"
#include "xml.h"

/* Debugging tool to output generic XML dump for tree generated via catherd */

static void
catxml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s>\n", np->name);
  if (np->data)
    fprintf(xhp->fp, "<data>%s</data>\n", xmlify((uccp)np->data));
}

static void
catxml_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>\n", np->name);
}

void
cat_xml(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, catxml_node, catxml_post);
  free(xhp);
}

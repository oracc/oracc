#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "cat.h"
#include "xml.h"

/* Debugging tool to output generic XML dump for tree generated via catherd */

nodehandlers catxmlhandlers;

static void
catxml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s>", np->name);
  if (np->text)
    fprintf(xhp->fp, "<data>%s</data>", xmlify((uccp)np->text));
  if (np->data && catxmlhandlers[np->ns])
    (catxmlhandlers[np->ns])(np, xhp);
}

static void
catxml_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
}

void
cat_xml(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, catxml_node, catxml_post);
  free(xhp);
}

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"
#include "xml.h"

/* Debugging tool to output generic XML dump for tree generated via catherd */

static void
gdlxml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s>", np->name);
  if (np->data)
    fprintf(xhp->fp, "<data>%s</data>", xmlify((uccp)np->data));
}

static void
gdlxml_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
}

void
gdl_xml(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, gdlxml_node, gdlxml_post);
  free(xhp);
}

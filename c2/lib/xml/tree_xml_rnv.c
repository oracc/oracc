#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include "xml.h"
#include "rnvxml.h"

static struct rnvdata rnvd;

static void
tree_xml_rnv_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_o_handlers[np->ns])
    {
      (treexml_o_handlers[np->ns])(np, &rnvd);
      
    }
}

static void
tree_xml_rnv_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_c_handlers[np->ns])
    {
      (treexml_c_handlers[np->ns])(np, &rnvd);
    }
}

void
tree_xml_rnv(FILE *fp, Tree *tp, struct xnn_data *xdp, const char *rncbase)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  rnvxml_init(tp, xdp, rncbase);
  tree_iterator(tp, xhp, tree_xml_rnv_node, tree_xml_rnv_post);
  free(xhp);
}

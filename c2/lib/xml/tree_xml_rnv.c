#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include "xml.h"
#include "rnvxml.h"

static struct rnvdata rnvd;
int xml_printing, xml_validating;

static void
tree_xml_rnv_node(Node *np, void *user)
{  
  Xmlhelper *xhp = user;
  if (treexml_o_handlers[np->ns])
    {
      (treexml_o_handlers[np->ns])(np, &rnvd);
      if (xml_validating)
	{
	  rnvval_ea(rnvd.tag , rnvd.ratts);
	  if (rnvd.chardata)
	    rnvval_ch((ccp)rnvd.chardata);
	}
      if (xml_printing)
	{
	  fprintf(xhp->fp, "<%s", rnvd.tag);
	  if (rnvd.ns)
	    tree_ns_xml_print(np->tree, xhp->fp);
	  if (rnvd.ratts)
	    xml_attr(rnvd.ratts->atts, xhp->fp);
	  if (rnvd.chardata)
	    fputs((ccp)rnvd.chardata, xhp->fp);
	}
    }
}

static void
tree_xml_rnv_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_c_handlers[np->ns])
    {
      (treexml_c_handlers[np->ns])(np, &rnvd);
      if (xml_validating)
	rnvval_ee(rnvd.tag);
      if (xml_printing)
	fprintf(xhp->fp, "</%s>", rnvd.tag);
    }
}

void
tree_xml_rnv(FILE *fp, Tree *tp, struct xnn_data *xdp, const char *rncbase)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  rnvxml_init_tree(tp, xdp, rncbase);
  tree_iterator(tp, xhp, tree_xml_rnv_node, tree_xml_rnv_post);
  free(xhp);
}

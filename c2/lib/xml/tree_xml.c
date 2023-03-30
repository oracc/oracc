#include <stdlib.h>
#include <stdio.h>
#include <tree.h>
#include "xml.h"
#include "rnvxml.h"

struct ns_key_val {
  const char *prefix; const char *url;
} ns_key_val[] = {
  { "", "" } ,
  { "c" , "http://oracc.org/ns/cbd/1.0" } ,
  { "g" , "http://oracc.org/ns/gdl/1.0" } ,
  { "s" , "http://oracc.org/ns/sl/1.0" } ,
  { "x" , "http://oracc.org/ns/xtf/1.0" } ,
  { "m" , "http://oracc.org/ns/xmd/1.0" } ,
  { NULL , NULL } 
};

/* For validating vs non-validating XML output register distinct
   routines in these handler arrays which are all used by
   tree_xml_node and tree_xml_post via a tree_xml call to
   tree_iterator */
nodehandlers treexml_o_handlers;
nodehandlers treexml_p_handlers;
nodehandlers treexml_c_handlers;

void
tree_ns_xml_print(Tree *tp, FILE *fp)
{
  enum nscode nsx = NS_NONE+1;
  if (tp->ns_used[0])
    fprintf(fp, " xmlns=\"%s\"", ns_key_val[tp->ns_used[0]].url);
  for (; nsx < NS_LAST; ++nsx)
    if (tp->ns_used[nsx])
      fprintf(fp, " xmlns:%s=\"%s\"", ns_key_val[nsx].prefix, ns_key_val[nsx].url);
}

void
treexml_o_generic(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s", np->name);

  if (!np->rent)
    tree_ns_xml_print(np->tree, xhp->fp);

  fputc('>', xhp->fp);

  if (np->text)
    fprintf(xhp->fp, "<text>%s</text>", xmlify((uccp)np->text));
}

/* no generic output for parsed nodes */

void
treexml_c_generic(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
}

static void
tree_xml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_o_handlers[np->ns])
    (treexml_o_handlers[np->ns])(np, xhp);
  else
    treexml_o_generic(np, xhp);
#if 0
  if (np->data && treexml_p_handlers[np->data->ns])
    (treexml_p_handlers[np->data->ns])(np, xhp);
#endif
}

static void
tree_xml_post(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  if (treexml_c_handlers[np->ns])
    (treexml_c_handlers[np->ns])(np, xhp);
  else
    treexml_c_generic(np, xhp);
}

void
tree_xml(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, tree_xml_node, tree_xml_post);
  free(xhp);
}

void
tree_xml_rnv(FILE *fp, Tree *tp, struct xnn_data *xdp, const char *rncbase)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  rnvxml_init(tp, xdp, rncbase);
  tree_iterator(tp, xhp, tree_xml_node, tree_xml_post);
  free(xhp);
}

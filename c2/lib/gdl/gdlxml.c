#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"
#include "xml.h"

static void gdl_xml_o(Node *np, void *user);
static void gdl_xml_c(Node *np, void *user);

void
gdlxml_setup(void)
{
  nodeh_register(treexml_o_handlers, NS_GDL, gdl_xml_o);
  nodeh_register(treexml_p_handlers, NS_GDL, NULL);
  nodeh_register(treexml_c_handlers, NS_GDL, gdl_xml_c);
}

static void
gdlxml_keqv(const char *k, const char *v, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, " %s=\"%s\"", k, xmlify((uccp)v));
}

#if 0
static void
gdlxml_ns(Node *np, void *user)
{
  Prop *p = np->props;
  while (p)
    {
      if (p->g == PG_XNS)
	gdlxml_keqv(p->k,p->v,user);
      p = p->next;
    }
}
#endif

static void
gdlxml_attr(Node *np, void *user)
{
  Prop *p = np->props;
  while (p)
    {
      if (p->g == PG_XML)
	gdlxml_keqv(p->k,p->v,user);
      p = p->next;
    }
}

static void
gdlxml_parsed(gvl_g *gp, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<gvl_g><c10e>%s</c10e>", xmlify(gp->c10e));
  if (gp->mess)
    fprintf(xhp->fp, "<mess>%s</mess>", xmlify(gp->mess));
  fprintf(xhp->fp, "</gvl_g>");
}

static void
gdl_xml_o(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s", np->name);
  if (!np->rent)
    tree_ns_xml_print(np->tree, xhp->fp);
  gdlxml_attr(np, user);
  fputc('>', xhp->fp);
  if (np->text)
    fprintf(xhp->fp, "<text>%s</text>", xmlify((uccp)np->text));
  if (np->user)
    gdlxml_parsed((gvl_g*)np->user, user);
}

static void
gdl_xml_c(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
}

#if 0
void
gdl_xml_handler(Node *np, void *xhp)
{
  node_iterator(np->data, xhp, gdl_xml_o, gdl_xml_c);
}
#endif

#if 0
void
gdl_xml_node(FILE *fp, Node *np)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  node_iterator(np, xhp, gdlxml_node, gdlxml_post);
  free(xhp);
}

void
gdl_xml_tree(FILE *fp, Tree *tp)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  tree_iterator(tp, xhp, gdlxml_node, gdlxml_post);
  free(xhp);
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"
#include "xml.h"

/* Debugging tool to output generic XML dump for tree generated via catherd */

static void
gdlxml_keqv(const char *k, const char *v, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, " %s=\"%s\"", k, xmlify((uccp)v));
}

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
gdlxml_node(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s", np->name);
  gdlxml_ns(np, user);
  gdlxml_attr(np, user);
  fputc('>', xhp->fp);
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

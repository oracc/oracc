#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "gdl.h"
#include "xml.h"
#include "rnvxml.h"

static void gdl_xml_o(Node *np, void *user);
static void gdl_xml_c(Node *np, void *user);
static void gdl_xml_rnv_o(Node *np, void *user);
static void gdl_xml_rnv_c(Node *np, void *user);

void
gdlxml_setup(void)
{
  nodeh_register(treexml_o_handlers, NS_GDL, gdl_xml_o);
  nodeh_register(treexml_p_handlers, NS_GDL, NULL);
  nodeh_register(treexml_c_handlers, NS_GDL, gdl_xml_c);
}

void
gdlxml_rnv_setup(void)
{
  nodeh_register(treexml_o_handlers, NS_GDL, gdl_xml_rnv_o);
  nodeh_register(treexml_p_handlers, NS_GDL, NULL);
  nodeh_register(treexml_c_handlers, NS_GDL, gdl_xml_rnv_c);
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
gdlxml_xml_props(Node *np, void *user)
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
  fprintf(xhp->fp, "<gvl_g><orig>%s</orig>", xmlify(gp->orig));
  fprintf(xhp->fp, "<c10e>%s</c10e>", xmlify(gp->c10e));
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
  gdlxml_xml_props(np, user);
  fputc('>', xhp->fp);
  if (np->text)
    fprintf(xhp->fp, "<text>%s</text>", xmlify((uccp)np->text));
  if (np->props)
    {
      Prop *p = np->props;
      fprintf(xhp->fp, "<props>");
      while (p)
	{
	  fprintf(xhp->fp, "<prop g=\"%d\" p=\"%d\"", p->g, p->p);
	  if (p->k)
	    fprintf(xhp->fp, " k=\"%s\" v=\"%s\"", p->k, xmlify((uccp)p->v));
	  fprintf(xhp->fp, "/>");
	  p = p->next;
	}
      fprintf(xhp->fp, "</props>");
    }
  if (np->user)
    gdlxml_parsed((gvl_g*)np->user, user);
}

static void
gdl_xml_c(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "</%s>", np->name);
}

static const char *
gdl_xml_tag(const char *nodename)
{
  return nodename;
}

struct rnvval_atts *
gdl_xml_ratts(Node *np, void *user)
{
  return NULL;
}

static unsigned const char *
gdl_xml_ch_orig(Node *np)
{
  if (np->name[2] == 's' || np->name[2] == 'v')
    {
      if (np->user)
	return ((gvl_g*)(np->user))->orig;
      else
	return (uccp)np->text;
    }
  return NULL;
}

static unsigned const char *
gdl_xml_ch_c10e(Node *np)
{
  if (np->name[2] == 's' || np->name[2] == 'v')
    {
      if (np->user)
	return ((gvl_g*)(np->user))->c10e;
      else
	return (uccp)np->text;
    }
  return NULL;
}

static void
gdl_xml_rnv_o(Node *np, void *user)
{
  struct rnvdata *rp = user;
  rp->tag = gdl_xml_tag(np->name);
  if (gdl_orig_mode)
    rp->chardata = gdl_xml_ch_orig(np);
  else
    rp->chardata = gdl_xml_ch_c10e(np);
    
  if (!np->rent)
    rp->ns = 1;

  rp->ratts = gdl_xml_ratts(np, user);
}

static void
gdl_xml_rnv_c(Node *np, void *user)
{
  struct rnvdata *rp = user;
  rp->tag = gdl_xml_tag(np->name);
}

#include <stdlib.h>
#include <tree.h>
#include <list.h>

#include "sll.h"
#include "gvl.h"

extern int gvl_strict;

static unsigned char *gvl_c_form(Node *ynp, void (*fnc)(Node *np, void *user));
static void gvl_c_node_orig(Node * np, void *user);
static void gvl_c_node_c10e(Node * np, void *user);

/* This routine should be called after cp->orig and cp->c10e have been set */
void
gvl_c(gvl_g *cp)
{
  unsigned const char *l = NULL;
  if ((l=gvl_lookup(cp->c10e)))
    {
      cp->oid = (ccp)l;
      cp->sign = gvl_lookup(sll_tmp_key(l,""));
      if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && gvl_strict)
	cp->mess = gvl_vmess("compound %s should be %s", cp->orig, cp->c10e);
    }
  else
    {
      unsigned char *c10e_no_p = sll_strip_pp(cp->c10e);
      if (c10e_no_p && (l=gvl_lookup(c10e_no_p)))
	{
	  cp->oid = (ccp)l;
	  cp->sign = gvl_lookup(sll_tmp_key(l,""));
	  if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && gvl_strict)
	    {
	      cp->mess = gvl_vmess("compound %s should be %s", cp->orig, c10e_no_p);
	      cp->c10e = pool_copy(c10e_no_p, curr_sl->p);
	    }
	}
      else
	{
	  if (!strchr((ccp)cp->orig,'X')) /* don't error on compounds with X--should be configurable */
	    {
	      if (strcmp((ccp)cp->orig, (ccp)cp->c10e) && c10e_no_p && strcmp((ccp)cp->c10e, (ccp)c10e_no_p))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s/%s)", cp->orig, cp->c10e, c10e_no_p);
	      else if (strcmp((ccp)cp->orig, (ccp)cp->c10e))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, cp->c10e);
	      else if (c10e_no_p && strcmp((ccp)cp->orig, (ccp)c10e_no_p))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, c10e_no_p);
	      else
		cp->mess = gvl_vmess("unknown compound: %s", cp->orig);
	    }
	}
      free(c10e_no_p);
    }
}

unsigned char *
gvl_c_orig(Node *ynp)
{
  return gvl_c_form(ynp, gvl_c_node_orig);
}

unsigned char *
gvl_c_c10e(Node *ynp)
{
  return gvl_c_form(ynp, gvl_c_node_c10e);
}

static void
gvl_c_node_orig(Node * np, void *user)
{
  if (strcmp(np->name, "g:c"))
    {
      if (np->user)
	list_add((List*)user, (void*)((gvl_g*)(np->user))->orig);
      else if (np->data)
	list_add((List*)user, (void*)np->data);
    }
}

static void
gvl_c_node_c10e(Node * np, void *user)
{
  if (strcmp(np->name, "g:c"))
    {
      if (np->user)
	list_add((List*)user, (void*)((gvl_g*)(np->user))->sign);
      else if (np->data)
	list_add((List*)user, (void*)np->data);
    }
}

static unsigned char *
gvl_c_form(Node *ynp, void (*fnc)(Node *np, void *user))
{
  List *lp = list_create(LIST_SINGLE);
  unsigned char *s = NULL, *ret = NULL;
  list_add(lp, "|");
  node_iterator(ynp, lp, fnc, NULL);
  list_add(lp, "|");
  s = list_concat(lp);
  ret = pool_copy(s, curr_sl->p);
  free(s);
  free(lp);
  return ret;
}

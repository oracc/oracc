#include <stdlib.h>
#include <tree.h>
#include <list.h>

#include "sll.h"
#include "gvl.h"

/* This routine should be called after cp->orig and cp->c10e have been set */
void
gvl_c(gvl_g *cp)
{
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
	      else if (strcmp((ccp)g, (ccp)c10e))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, cp->c10e);
	      else if (c10e_no_p && strcmp((ccp)g, (ccp)c10e_no_p))
		cp->mess = gvl_vmess("unknown compound: %s (also tried %s)", cp->orig, c10e_no_p);
	      else
		cp->mess = gvl_vmess("unknown compound: %s", cp->orig);
	    }
	}
      free(c10e_no_p);
    }
}

static void gvl_c_node_orig(Node * np, void *user)
{
  list_add((List*)user, np->orig);
}

static void gvl_c_node_c10e(Node * np, void *user)
{
  list_add((List*)user, np->c10e);
}

unsigned char *
gvl_c_orig(Node *ynp)
{
  List *lp = list_create(LIST_SINGLE);
  unsigned char *s = NULL;
  node_iterate(ynp, gvl_c_node_orig, NULL, lp);
  s = list2str(lp);
  free(lp);
  return s;
}

unsigned char *
gvl_c_c10e(Node *ynp)
{
  List *lp = list_create(LIST_SINGLE);
  unsigned char *s = NULL;
  node_iterate(ynp, gvl_c_node_c10e, NULL, lp);
  s = list2str(lp);
  free(lp);
  return s;
}

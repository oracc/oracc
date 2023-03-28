#include <stdlib.h>
#include <stdio.h>
#include "pool.h"
#include "memo.h"
#include "tree.h"
#include "xml.h"
#include "rnvxml.h"

/* For validating vs non-validating XML output register distinct
   routines in these handler arrays which are all used by
   tree_xml_node and tree_xml_post via a tree_xml call to
   tree_iterator */
nodehandlers treexml_o_handlers;
nodehandlers treexml_p_handlers;
nodehandlers treexml_c_handlers;

Tree *
tree_init(void)
{
  Tree *tp = calloc(1, sizeof(Tree));
  tp->nodemem = memo_init(sizeof(Node), 1024);
  tp->pool = pool_init();
  tp->propmem = memo_init(sizeof(Prop), 1024);
  return tp;
}

void
tree_term(Tree *tp)
{
  memo_term(tp->nodemem);
  pool_term(tp->pool);
  free(tp);
}

Node *
tree_node(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = memo_new(tp->nodemem);
  np->tree = tp;
  np->name = name;
  np->depth = depth;
  np->mloc = loc;
  return np;
}

Node *
tree_root(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = tree_node(tp, name, depth, loc);
  tp->root = tp->curr = np;
  return np;
}

Node *
tree_add(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = NULL;

  if (tp)
    {
      if (tp->curr && tp->curr->kids)
	{
	  np = tree_node(tp, name, depth, loc);
	  np->rent = tp->curr->last->rent;
	  tp->curr->last->next = np;
	  tp->curr->last = np;
	}
      else
	{
	  np = tree_node(tp, name, depth, loc);
	  if (!tp->curr)
	    {
	      tp->curr = tp->root = np;
	      /* leave tp->rent and tp->kids as NULL */
	    }
	  else
	    {
	      np->rent = tp->curr;
	      tp->curr->last = tp->curr->kids = np;
	    }
	}
    }
  return np;
}

Node *
tree_pop(Tree *tp)
{
  if (tp->curr->rent)
    return tp->curr = tp->curr->rent;
  else
    return NULL;
}

void
tree_push(Tree *tp)
{
  if (tp->curr->last)
    tp->curr = tp->curr->last;
}

static void
_do_node(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  if (np)
    {
      Node *entry_np = np;
      if (nodefnc)
	nodefnc(np, user);
      for (np = np->kids; np; np = np->next)
	_do_node(np, user, nodefnc, postfnc);
      if (postfnc)
	postfnc(entry_np, user);
    }
}

void
tree_iterator(Tree *tp, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  if (tp && tp->root)
    _do_node(tp->root, user, nodefnc, postfnc);
}

void
treexml_o_generic(Node *np, void *user)
{
  Xmlhelper *xhp = user;
  fprintf(xhp->fp, "<%s>", np->name);
  if (np->data)
    fprintf(xhp->fp, "<data>%s</data>", xmlify((uccp)np->data));
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
  if (np->data && treexml_p_handlers[np->data->ns])
    (treexml_p_handlers[np->data->ns])(np, xhp);
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

void
node_iterator(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  _do_node(np, user, nodefnc, postfnc);
}

void
kids_add_node(Tree *tp, Node *np)
{
  if (tp && tp->curr)
    {
      if (tp->curr->kids)
	{
	  np->rent = tp->curr->last->rent;
	  tp->curr->last->next = np;
	  tp->curr->last = np;
	}
      else
	{
	  np->rent = tp->curr;
	  tp->curr->last = tp->curr->kids = np;
	}
    }
}

Node *
kids_rem_last(Tree *tp)
{
  Node *np = NULL;
  if (tp && tp->curr)
    {
      if (tp->curr->kids)
	{
	  Node *kp = tp->curr->kids;
	  if (kp->next)
	    {
	      while (kp->next != tp->curr->last)
		kp = kp->next;
	      tp->curr->last = kp;
	      np = kp->next;
	      kp->next = NULL;
	    }
	  else
	    {
	      np = kp;
	      kp->rent->kids = NULL;
	    }
	  np->rent = NULL;
	}
    }
  return np;
}

void
nodeh_register(nodehandlers nh, nscode nt, nodehandler fnc)
{
  nh[nt] = fnc;
}

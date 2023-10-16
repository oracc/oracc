#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pool.h"
#include "memo.h"
#include "tree.h"

static struct treemem *tmem = NULL;

void
treemem_init(void)
{
  tmem = calloc(1, sizeof(struct treemem));
  tmem->tree_mem = memo_init(sizeof(Tree), 1024);
  tmem->node_mem = memo_init(sizeof(Node), 1024);
  tmem->prop_mem = memo_init(sizeof(Prop), 1024);
  tmem->keva_mem = memo_init(sizeof(Keva), 1024);
  tmem->pool = pool_init();
}

void
tmem_term(void)
{
  if (tmem)
    {
      memo_term(tmem->tree_mem);
      memo_term(tmem->node_mem);
      memo_term(tmem->prop_mem);
      memo_term(tmem->keva_mem);
      pool_term(tmem->pool);
      free(tmem);
    }
}

Tree *
tree_init(void)
{
  Tree *tp = NULL;
  if (!tmem)
    treemem_init();
  tp = memo_new(tmem->tree_mem);
  tp->tm = tmem;
  return tp;
}

void
tree_term(void)
{
  tmem_term();
}

Node *
tree_node(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = memo_new(tp->tm->node_mem);
  np->tree = tp;
  np->name = name;
  np->depth = depth;
  np->mloc = loc;
  np->ns = ns;
  if (!tp->ns_used[ns])
    tp->ns_used[ns] = ns;
  return np;
}

void
tree_ns_default(Tree *tp, nscode ns)
{
  tp->ns_used[0] = ns;
}

void
tree_ns_merge(Tree *tp, nsrefs used)
{
  enum nscode ns;
  for (ns = NS_NONE; ns < NS_LAST; ++ns)
    if (used[ns] && !tp->ns_used[ns])
      tp->ns_used[ns] = ns;
}

/* Make np the parent to which new nodes will be added as kids */
void
tree_curr(Node *np)
{
  Tree *tp = np->tree;
  tp->curr = np;
  if (np->kids)
    tp->curr->last = node_last(np->kids);
  else
    tp->curr->last = NULL;
}

Node *
tree_root(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = tree_node(tp, ns, name, depth, loc);
  tp->root = tp->curr = np;
  return np;
}

Node *
tree_add(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = NULL;

  if (tp)
    {
      np = tree_node(tp, ns, name, depth, loc);
      if (tp->curr && tp->curr->kids && tp->curr->last)
	{
	  np->rent = tp->curr->last->rent;
	  np->prev = tp->curr->last;
	  tp->curr->last->next = np;  
	  tp->curr->last = np;
	}
      else
	{
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
    {
      if (tp->curr->mloc)
	mesg_vwarning(tp->curr->mloc->file, tp->curr->mloc->line, "internal error: attempt to pop node with no parent\n");
      else
	fprintf(stderr, "internal error in tree_pop\n");
      return tp->curr;
    }
}

Node *
tree_push(Tree *tp)
{
  if (tp->curr->last)
    return tp->curr = tp->curr->last;
  else
    return NULL;
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

/* Graft the kids of tree tp onto node np */
void
tree_graft(Node *np, Tree *tp)
{
  if (np->kids)
    fprintf(stderr, "tree_graft: internal error: np->kids should be NULL\n");
  else
    {
      np->kids = tp->root->kids;
      tree_ns_merge(np->tree, tp->ns_used);
    }
}

Node *
node_ancestor(Node *np, const char *name)
{
  for (np = np->rent; np; np = np->rent)
    {
      if (!strcmp(np->name, name))
	return np;
    }
  return NULL;
}

Node *
node_ancestor_or_self(Node *np, const char *name)
{
  for (; np; np = np->rent)
    {
      if (!strcmp(np->name, name))
	return np;
    }
  return NULL;
}

/* Insert nkid as the new kid of rent and move all the existing kids
   of rent to be kids of nkid */
Node *
node_insert(Node *rent, Node *nkid)
{
  Node *np;
  for (np = rent->kids; np; np = np->next)
    np->rent = nkid;
  nkid->kids = rent->kids;
  nkid->rent = rent;
  rent->kids = nkid;
  return nkid;
}

void
node_iterator(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  _do_node(np, user, nodefnc, postfnc);
}

Node *
node_last(Node *np)
{
  if (np)
    while (np->next)
      np = np->next;
  return np;
}

/* Replace the contents of to node with from node, but keep the
   linkages of the to node */
void
node_replace(Node *from, Node *to)
{
  to->ns = from->ns;
  to->name = from->name;
  to->text = from->text;
  to->kids = from->kids;
  to->props = from->props;
  to->user = from->user;
  /* leave other structure members because they relate to location in
     file/tree */
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

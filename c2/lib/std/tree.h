#ifndef TREE_H_
#define TREE_H_

#include <memo.h>
#include <mesg.h>
#include <pool.h>
#include <prop.h>

struct tree {
  struct node *root;
  struct node *curr;
  Memo *nodemem;
  Memo *propmem;
  Pool *pool;
};

typedef struct tree Tree;

struct node {
  const char *name;    	/* node name */
  const char *id;      	/* node ID */
  const char *data; 	/* unparsed data when node comes from cat-style input */
  void *parsed;		/* parsed data; parsing is done by caller's routines */
  int depth;		/* nesting depth of node; may be -1 if not
			   used by caller */
  struct node *rent; 	/* parent */
  struct node *kids; 	/* children */
  struct node *last; 	/* last sibling, i.e., where we are adding sibs */
  struct node *next; 	/* next sibling */
  Tree *tree;
  Prop *props;
  Mloc *mloc;
};

typedef struct node Node;

extern Tree *tree_init(void);
extern void tree_term(Tree *tp);
extern Node *tree_pop(Tree *tp);
extern void tree_push(Tree *tp);
extern Node *tree_add(Tree *tp, const char *name, int depth, Mloc *loc);
extern Node *tree_node(Tree *tp, const char *name, int depth, Mloc *loc);
extern Node *tree_root(Tree *tp, const char *name, int depth, Mloc *loc);
extern void tree_iterator(Tree *tp, void *user,
			  void (*nodefnc)(Node *np, void *user),
			  void (*postfnc)(Node *np, void *user));

#endif/*TREE_H_*/

#ifndef TREE_H_
#define TREE_H_

#include <memo.h>
#include <mesg.h>
#include <pool.h>

struct tree {
  struct node *root;
  struct node *curr;
  Memo *nodemem;
  Pool *pool;
};

typedef struct tree Tree;

struct node {
  const char *name;    	/* node name */
  const char *id;      	/* node ID */
  void *data;		/* data */
  int depth;		/* nesting depth of node; may be -1 if not
			   used by caller */
  struct node *rent; 	/* parent */
  struct node *kids; 	/* children */
  struct node *last; 	/* last sibling, i.e., where we are adding sibs */
  struct node *next; 	/* next sibling */
  Mloc *mloc;
};

typedef struct node Node;

extern Tree *tree_init(void);
extern void tree_term(Tree *tp);
extern Node *tree_pop(Tree *tp);
extern void tree_push(Tree *tp);
extern Node *tree_add(Tree *tp, const char *name, int depth, Mloc *loc);
extern Node *tree_node(Tree *tp, const char *name, int depth, Mloc *loc);

#endif/*TREE_H_*/

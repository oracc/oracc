#ifndef TREE_H_
#define TREE_H_

#include <memo.h>
#include <mesg.h>
#include <pool.h>
#include <prop.h>

enum nscode { NS_NONE=0 , NS_CBD , NS_GDL , NS_SL , NS_XTF , NS_XMD , NS_LAST };
typedef enum nscode nscode;
typedef nscode nsrefs[NS_LAST];

struct tree {
  struct node *root;
  struct node *curr;
  Memo *nodemem;
  Memo *propmem;
  Pool *pool;
  nsrefs ns_used;
};

typedef struct tree Tree;
struct prop;

struct node {
  enum nscode ns;   	/* node namespace */
  const char *name;    	/* node name */
  const char *text; 	/* unparsed text-data when node comes from cat-style input */
  const char *id;      	/* node ID unique to current run; used for
			   hashing/listing data when processing
			   tree */
  int depth;		/* nesting depth of node; may be -1 if not
			   used by caller */
  struct node *rent; 	/* parent */
  struct node *kids; 	/* children */
  struct node *last; 	/* last sibling, i.e., where we are adding sibs */
  struct node *next; 	/* next sibling */
  Tree *tree;
  struct prop *props;
  Mloc *mloc;
  void *user;		/* data dependent on node prefix/prefix:name combination */
};

typedef struct node Node;

typedef void (*nodehandler)(Node *np, void *user);
typedef nodehandler nodehandlers[NS_LAST];
extern void nodeh_register(nodehandlers nh, nscode c, nodehandler fnc);

extern Tree *tree_init(void);
extern void tree_term(Tree *tp);
extern Node *tree_pop(Tree *tp);
extern Node *tree_push(Tree *tp);
extern Node *tree_add(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc);
extern Node *tree_node(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc);
extern Node *tree_root(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc);
extern void tree_iterator(Tree *tp, void *user,
			  void (*nodefnc)(Node *np, void *user),
			  void (*postfnc)(Node *np, void *user));
extern void node_iterator(Node *tp, void *user,
			  void (*nodefnc)(Node *np, void *user),
			  void (*postfnc)(Node *np, void *user));

extern void tree_iterator_nh(Tree *tp, void *user, nodehandlers nh_node, nodehandlers nh_post);

extern void kids_add_node(Tree *tp, Node *np);
extern Node *kids_rem_last(Tree *tp);

extern void tree_curr(Node *np);
extern void tree_graft(Node *np, Tree *tp);
extern void tree_xml(FILE *fp, Tree *tp);

extern void tree_ns_default(Tree *tp, nscode ns);
extern void tree_ns_merge(Tree *tp, nsrefs used);

extern Node *node_ancestor(Node *np, const char *name);
extern Node *node_ancestor_or_self(Node *np, const char *name);
extern Node *node_insert(Node *rent, Node *nkid);
extern Node *node_last(Node *np);
extern void node_replace(Node *from, Node *to);

#endif/*TREE_H_*/

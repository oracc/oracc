#ifndef CAT_H_
#define CAT_H_

#include <tree.h>

struct cat {
  const char *f;	/* filename */
  struct catchunk *c;	/* input chunks */
  struct catnode *t; 	/* tree of nodes */
};

struct catchunk {
  struct cat *c;
  char *text;
  int   line;
  int 	last;
  struct catchunk *next;
};

struct catnode {
  char *name; 		/* @-tag name */
  struct catchunk *c; 	/* chunk where this node begins */
  void *d;		/* data */
  struct catnode *p; 	/* parent */
  struct catnode *k; 	/* kids */
  struct catnode *next; /* next sibling */
  struct catnode *last;	/* last sibling--i.e., where we are attaching nodes */
};

enum ci_rel { CI_PARENT , CI_CHILD , CI_END };

struct catconfig {
  char *head; 	/* @-tag which is the tree root; normally virtual
		   because, e.g., .asl files don't use @signs, bug
		   signs is the nomain root of an asl tree */
  char * (*getname)(struct catchunk *, char **data); 	/* function to get a name from
							   a chunk */
  struct catinfo * (*chkname)(const char *,size_t);  	/* function to test name is known and
							   obtain its catinfo */
  void (*parse)(struct catchunk *, char *data);		/* function to parse data based on name of chunk */
};

struct catinfo {
  char *name;		/* name of @-tag */
  enum ci_rel rel;	/* parent or child type */
  int depth;		/* tree depth, root level == 1 */
  int end;		/* 0 == @end is optional ; 1 == @end is required */
  void (*parse)(Node *, char *); /* function to parse data associated with name */
  char *pre_flags;	/* string of characters allowed in pre-tag flags */
  char *post_flags;	/* string of characters allowed in post-tag flags */
};

struct catstate {
  struct catinfo *cip;
  struct catnode *cn;
  int end;
};

extern nodehandlerset catxmlhandlers;

extern int catparse(void); /* bison */

extern void cat_chunk(int l, char *t);
extern void cat_cont(int l, char *t);
extern void cat_dump(struct catchunk *ccp);
extern void cat_init(void);
extern char* cat_name(struct catchunk *cp, char **data);
extern void cat_term(void);
extern struct catchunk *cat_read(const char *file);
extern Tree *cat_herd(struct catchunk *cp, struct catconfig *cfg);
extern void cat_xml(FILE *fp, Tree *tp);

extern struct catchunk *catyacc(void);

#endif/*CAT_H_*/

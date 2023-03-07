#ifndef CAT_H_
#define CAT_H_

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
  char *n; 		/* name */
  struct catchunk *c; 	/* chunk where this node begins */
  void *d;		/* data */
  struct catnode *k; 	/* kids */
  struct catnode *p; 	/* parent */
};

enum ci_rel { CI_PARENT , CI_CHILD };

struct catconfig {
  char *root; 	/* @-tag which is the tree root; normally virtual
		   because, e.g., .asl files don't use @signs, bug
		   signs is the nomain root of an asl tree */
  char * (*getname)(struct catchunk *); /* function to get a name from
					   a chunk */
  struct catinfo * (*chkname)(char *);  /* function to test name is
					   known and obtain its
					   catinfo */
};

struct catinfo {
  char *n;		/* name of @-tag */
  enum ci_rel rel;	/* parent or child type */
  int depth;		/* tree depth, root level == 1 */
  int end;		/* 0 == @end is optional ; 1 == @end is required */
  char *pre_flags;	/* string of characters allowed in pre-tag flags */
  char *post_flags;	/* string of characters allowed in post-tag flags */
};

extern int catparse(void); /* bison */

extern void cat_cont(int l, char *t);
extern void cat_dump(struct catchunk *ccp);
extern void cat_init(void);
extern void cat_chunk(int l, char *t);
extern void cat_term(void);
extern struct catchunk *cat_read(const char *file);
extern struct catnode *cat_herd(struct catchunk *cp);

#endif/*CAT_H_*/

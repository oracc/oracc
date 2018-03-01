#ifndef _TREE_H
#define _TREE_H
#include <sys/unistd.h>
#include <stdio.h>
#include "cdf.h"
#include "xmlnames.h"
#include "tokenizer.h"

/* Avoid having to include xcl.h and all the lem stuff */
struct xcl_context;

struct nodelist
{
  void**  nodes;
  ssize_t lastnode;
  ssize_t lastused;
};

struct grapheme;

struct node
{
  const char *type;
  enum e_type etype;
  enum t_type ttype; /* token type */
  struct xname *names;
  struct node*parent;
  const char *const*xmlns;
  struct nodelist attr;
  struct nodelist children;
  char **attrp; 	/* expat-style attr array */
  unsigned const char *data;
  int lnum;
  void *user;
  enum block_levels level;
  int clone;
  struct grapheme *grapheme;
};

#define firstChild(n) (n)->children.nodes[0]
void addToNodeList(struct nodelist*nl, void *vp);
#define appendAttr(p,a) addToNodeList(&((p)->attr),a)
void appendAttrCat(struct node *p, char sep, enum a_type atype, const char *aname, unsigned const char *aval);
struct node*ancestor_or_self(struct node*node,char *name);
#define appendChild(p,c) _appendChild((p),(c),__FILE__,__LINE__)
struct node*_appendChild(struct node*parent,struct node*child,const char *FILE, int LINE);
struct attr *attr(enum a_type a,const unsigned char *data);
struct attr *gattr(enum a_type a,unsigned const char *data);
ssize_t buffer(struct node *n,unsigned char *b,ssize_t index,ssize_t len);
struct node *cdata(unsigned char *data);
struct node *gcdata(unsigned char *data);
void clear_blocks(void);
struct node *cloneNode(struct node *np);
struct node *elem(enum e_type,struct node*parent,
		  int lnum,enum block_levels b);
struct node *gelem(enum e_type,struct node*parent,
		  int lnum,enum block_levels b);
unsigned const char *getAttr(struct node *elem,const char *attrName);
unsigned const char *getAttrI(struct node *elem,const char *attrName);
void removeAttr(struct node *elem,const char *attrName);
struct node*insertBefore(struct node *parent,int cindex,struct node *r);
struct node*lastChild(struct node*elem);
struct attr *newattr(void);
struct attr *gnewattr(void);
struct node *newnode(void);
struct node *gnewnode(void);
struct node* removeFirstChild(struct node*elem);
struct node*removeLastChild(struct node*elem);
struct node *replaceChild(struct node *parent,int cindex,struct node *r);
void serialize(struct node*n, int with_xcl);
int setAttr(struct node *elem, enum a_type a, const unsigned char *value);
int gsetAttr(struct node *elem, enum a_type a, const unsigned char *value);
void setName(struct node *elem, enum e_type e);
struct node *textElem(enum e_type e, struct node *parent, 
		      int lnum, enum block_levels b, unsigned char *data);
struct node *textNode(const unsigned char *data);
struct node *gtextElem(enum e_type e, struct node *parent, 
		      int lnum, enum block_levels b, const unsigned char *data);
struct node *gtextNode(const unsigned char *data);
int tree_get_line(void);
void tree_init(void);
void tree_term(void);
void tree_validate(struct node *np);
unsigned char *cdf_xmlify(const unsigned char *s);
unsigned char *xmlify_no_pool(const unsigned char *s);
void atf_file_pi(const char *file);
void atf_lnum_pi(struct node *n);
void xml_decl(void);

extern void tree_functions(void (*xcl_serialize_arg)(struct xcl_context *,FILE*,int));
extern void
set_or_append_attr(struct node *n, enum a_type atype, const char *aname,
		   unsigned char *text);
#endif /*_TREE_H*/

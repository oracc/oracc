#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "cdf.h"
#include "xmlnames.c"
#include "tree.h"
#include "npool.h"
#include "rnvif.h"
#include "xcl.h"
#include "list.h"

List *tree_mem_list = NULL;
static struct npool *tree_pool;
#define pool_copy(x) npool_copy((x),tree_pool)

#define BLOCK_SIZE    2048
#define gBLOCK_SIZE   2048
#define A_BLOCK_SIZE 16384
#define A_gBLOCK_SIZE 2048

#define NL_BASE	   8

#undef TVDEBUG

static void (*xcl_serialize_p)(struct xcl_context *,FILE*,int) = NULL;

int pretty = 0;
extern int no_pi;
extern FILE *f_xml;

#if 0
static struct node empty_node = {
  0, 0, 0, 0, 0,
  { 0, 0, 0 },
  { 0, 0, 0 },
  0, 0, 0, 0, 0
};
#endif

static struct node *blocks[BLOCK_SIZE];
static int block_lastused = -1;
static int block_lastallocated = -1;
static int lastused = 0;
static int lastnode = 0;

static struct node *gblocks[gBLOCK_SIZE];
static int gblock_lastused = -1;
static int gblock_lastallocated = -1;
static int glastused = 0;
static int glastnode = 0;

static struct attr *a_blocks[A_BLOCK_SIZE];
static int a_block_lastused = -1;
static int a_block_lastallocated = -1;
static int a_lastused = 0;
static int a_lastnode = 0;

static struct attr *a_gblocks[A_gBLOCK_SIZE];
static int a_gblock_lastused = -1;
static int a_gblock_lastallocated = -1;
static int a_glastused = 0;
static int a_glastnode = 0;

static int curr_line;
static const char * xmlchars[256];

#define check_node_list(nl) \
  if (nl->lastused == nl->lastnode) \
    { \
      nl->lastnode = (nl->lastnode ? (nl->lastnode * 2) : NL_BASE);	\
      nl->nodes = realloc(nl->nodes, nl->lastnode*sizeof(struct node*)); \
    }

static void serialize_kids(struct node*n);

void
tree_functions(void (*xcl_serialize_arg)(struct xcl_context *,FILE*,int))
{
  xcl_serialize_p = xcl_serialize_arg;
}

int
tree_get_line()
{
  return curr_line;
}

void
tree_validate(struct node *n)
{
  register int i = 0, i_used;
  if (!n)
    {
      fprintf(stderr,"attempt to validate NULL node\n");
      return;
    }
  if (!n->type)
    {
      fprintf(stderr,"attempt to validate node with NULL type\n");
      return;
    }
  if (*n->type == 't')
    {
#ifdef TVDEBUG
      fprintf(stderr,"tv: chars: %s\n",n->data);
#endif
      rnv_characters(NULL,(const char*)n->data,xxstrlen(n->data));
      return;
    }

  if (!xstrcmp(n->type,"cdata"))
    {
      char *p = (char*)cdf_xmlify(n->data);
#ifdef TVDEBUG
      fprintf(stderr,"tv: chars: %s\n",p);
#endif
      rnv_characters(NULL,p,strlen(p));
      return;
    }

  curr_line = n->lnum;

  n->attrp = malloc(((2*n->attr.lastused)+1)*sizeof(char*));
  for (i = i_used = 0; i < n->attr.lastused; ++i)
    {
      if (strncmp(((struct attr*)(n->attr.nodes[i]))->renpair[0],"xmlns:",6))
	memcpy(&n->attrp[i_used++*2],
	       ((struct attr*)(n->attr.nodes[i]))->valpair,
	       2*sizeof(char*));
    }
  n->attrp[i_used*2] = NULL;

#ifdef TVDEBUG
  {
    const char **tvp = n->attrp;
    fprintf(stderr, "tv: selem: %s\n",n->names->qname);
    while (tvp[0])
      {
	fprintf(stderr,"\t%s=\"",*tvp++);
	fprintf(stderr,"%s\"\n",*tvp++);
      }
  }
#endif
  rnv_start_element(NULL,n->names->qname,(const char **)n->attrp);

  if (n->children.lastused)
    for (i = 0; i < n->children.lastused; ++i)
      tree_validate(n->children.nodes[i]);

#ifdef TVDEBUG
  fprintf(stderr,"tv: endel: %s\n",n->names->qname);
#endif
  rnv_end_element(NULL,n->names->qname);
  free(n->attrp);
  n->attrp = NULL;
}

void
tree_init()
{
  /*memset(xmlchars,'\0',256*sizeof(const char *));*/
  int i;
  tree_pool = npool_init();
  xmlchars[0] = "";
  for (i = 1; i < 256; ++i)
    {
      static unsigned char buf[2];
      buf[0] = i; buf[1] = '\0';
      xmlchars[i] = (const char *)pool_copy(buf);
    }
  xmlchars['&'] = "&amp;";
  xmlchars['"'] = "&quot;";
  xmlchars['<'] = "&lt;";

  tree_mem_list = list_create(LIST_SINGLE);
}

void
tree_term()
{
  int i;
  (void)cdf_xmlify(NULL);
  list_free(tree_mem_list,free);
  tree_mem_list = NULL;
  for (i = 0; i <= block_lastallocated; ++i)
    {
      int j;
#if 0
      if (i < block_lastused)
	jtop = BLOCK_SIZE;
      else
	jtop = lastused;
#endif
      for (j = 0; j < BLOCK_SIZE; ++j)
	{
	  if (!blocks[i][j].type)
	    continue;
	  if (blocks[i][j].children.nodes)
	    {
	      if (!blocks[i][j].clone)
		free(blocks[i][j].children.nodes);
	    }
	  if (blocks[i][j].attr.nodes)
	    free(blocks[i][j].attr.nodes);
	  if (blocks[i][j].user > (void*)1)
	    free(blocks[i][j].user);	  
	}
      free(blocks[i]);
    }

  for (i = 0; i <= gblock_lastallocated; ++i)
    {
      int j, jtop;
      if (i < gblock_lastused)
	jtop = gBLOCK_SIZE;
      else
	jtop = glastused;
      for (j = 0; j < jtop; ++j)
	{
	  if (gblocks[i][j].children.nodes)
	    free(gblocks[i][j].children.nodes);
	  if (gblocks[i][j].attr.nodes)
	    free(gblocks[i][j].attr.nodes);
	}
      free(gblocks[i]);
    }

  for (i = 0; i <= a_block_lastallocated; ++i)
    free(a_blocks[i]);
  for (i = 0; i <= a_gblock_lastallocated; ++i)
    free(a_gblocks[i]);

  npool_term(tree_pool);
}

void
addToNodeList(struct nodelist*nl, void *vp)
{
  check_node_list(nl);
  nl->nodes[nl->lastused++] = vp;
}

struct node *
ancestor_or_self(struct node *node,char *name)
{
  while (node && xstrcmp(node->names->pname,name))
    node = node->parent;
   return node;
}

struct node *
appendChild(struct node*parent, struct node*child)
{
  if (parent && child)
    {
      addToNodeList(&parent->children,child);
      child->parent = parent;
      return child;
    }
  else
    return NULL;
}

struct attr*
attr(enum a_type a, const unsigned char *value)
{
  struct avpair
  {
    unsigned char *iv;  /* internal value */
    unsigned char *xv;  /* xmlified value */
  };
  struct attr *ap = newattr();
  struct avpair *avp = NULL;
  if (value == NULL) {
    value = ucc("NULL");
  }
  if (!avp)
    {
#define AVPAIR_SIZE 32768
      static struct avpair *avblock = NULL;
      static int last_avpair_used = AVPAIR_SIZE;
      if (last_avpair_used == AVPAIR_SIZE)
	{
	  avblock = malloc(AVPAIR_SIZE * sizeof(struct avpair));
	  list_add(tree_mem_list,avblock);
	  last_avpair_used = 0;
	}
      avp = &avblock[last_avpair_used++];
      if (a == a_xml_id || !strpbrk(cc(value),"<&\""))
	avp->xv = avp->iv = pool_copy(value);
      else
	{
	  avp->iv = pool_copy(value);
	  avp->xv = cdf_xmlify(value);
	}
    }
  *ap = abases[a];
  assert(ap->valpair[0] != NULL);
  ap->valpair[1] = (char*)avp->iv;
  ap->renpair[1] = (char*)avp->xv;
  return ap;
}

struct attr*
gattr(enum a_type a, const unsigned char *value)
{
  struct avpair
  {
    unsigned char *iv;  /* internal value */
    unsigned char *xv;  /* xmlified value */
  };
  struct attr *ap = gnewattr();
  struct avpair *avp = NULL;
  assert(value != NULL);
  if (!avp)
    {
#define AVPAIR_SIZE 32768
      static struct avpair *avblock = NULL;
      static int last_avpair_used = AVPAIR_SIZE;
      if (last_avpair_used == AVPAIR_SIZE)
	{
	  avblock = malloc(AVPAIR_SIZE * sizeof(struct avpair));
	  list_add(tree_mem_list,avblock);
	  last_avpair_used = 0;
	}
      avp = &avblock[last_avpair_used++];
      if (a == a_xml_id || !strpbrk(cc(value),"<&\""))
	avp->xv = avp->iv = pool_copy(value);
      else
	{
	  avp->iv = pool_copy(value);
	  avp->xv = cdf_xmlify(value);
	}
    }
  *ap = abases[a];
  ap->valpair[1] = (char*)avp->iv;
  ap->renpair[1] = (char*)avp->xv;
  return ap;
}

struct node *
cdata(unsigned char *data)
{
  struct node *t = newnode();
#if 1
  t->type = "t";
  t->data = cdf_xmlify(data);
#else
  t->type = "cdata";
  t->data = data;
#endif
  return t;
}

struct node *
gcdata(unsigned char *data)
{
  struct node *t = gnewnode();
#if 1
  t->type = "t";
  t->data = cdf_xmlify(data);
#else
  t->type = "cdata";
  t->data = data;
#endif
  return t;
}

void
clear_blocks()
{
  int i = 0;
  while (i <= block_lastused)
    {
      int j;
#if 0
      if (i < block_lastused)
	jtop = BLOCK_SIZE;
      else
	jtop = topused;
#endif
      for (j = 0; j < BLOCK_SIZE; ++j)
	{
	  if (!blocks[i][j].type)
	    continue;
	  if (!blocks[i][j].clone)
	    {
	      if (blocks[i][j].children.nodes)
		free(blocks[i][j].children.nodes);
	    }
	  if (blocks[i][j].attr.nodes)
	    free(blocks[i][j].attr.nodes);
	  if (blocks[i][j].user > (void*)1)
	    free(blocks[i][j].user);
	}
      memset(blocks[i++],'\0',BLOCK_SIZE*sizeof(struct node));
    }
  lastused = 0;
  lastnode = BLOCK_SIZE;
  block_lastused = 0;

  for (i = 0; i <= a_block_lastused; ++i)
    memset(a_blocks[i],'\0',A_BLOCK_SIZE*sizeof(struct attr));
  a_lastused = 0;
  a_lastnode = A_BLOCK_SIZE;
  a_block_lastused = 0;
}

struct node*
cloneNode(struct node*np)
{
  struct node *clone = NULL;
  if (np)
    {
      int i;
      clone = newnode();
      *clone = *np;
      clone->clone = 1;
      memset(&clone->attr,'\0',sizeof(struct nodelist));
      for (i = 0; i < np->attr.lastused; ++i)
	appendAttr(clone,np->attr.nodes[i]);
    }
  return clone;
}

struct node *
elem(enum e_type e, struct node *parent, int lnum, enum block_levels b)
{
  struct node *ep = newnode();
  ep->type = "e";
  ep->etype = e;
  ep->names = &enames[e];
  ep->parent = parent;
  ep->lnum = lnum;
  ep->level = b;
  ep->attr.lastused = ep->children.lastused = 0;
  return ep;
}

struct node *
gelem(enum e_type e, struct node *parent, int lnum, enum block_levels b)
{
  struct node *ep = gnewnode();
  ep->type = "e";
  ep->names = &enames[e];
  ep->parent = parent;
  ep->lnum = lnum;
  ep->level = b;
  return ep;
}

struct node*
fix_node(struct node *n)
{
  struct node *fixed = malloc(sizeof(struct node));
  *fixed = *n;
  return fixed;
}

void
appendAttrCat(struct node *p, char sep, enum a_type atype, const char *aname, unsigned const char *aval)
{
  const char *xval = cc(getAttr(p,aname));
  if (*xval)
    {
      static char *newval = NULL;
      static int newval_size = 0;
      if ((strlen(cc(aval)) + strlen(cc(xval)) + 2) > newval_size)
	{
	  newval_size = strlen(cc(aval))+strlen(xval)+2;
	  newval_size *= 2;
	  newval = realloc(newval,newval_size);
	}
      sprintf(newval,"%s%c%s",xval,sep,aval);
      setAttr(p,atype,ucc(newval));
    }
  else
    appendAttr(p,attr(atype,aval));
}

unsigned const char *
getAttr(struct node *elem, const char *attrName)
{
  int i;
  for (i = 0; i < elem->attr.lastused; ++i)
    {
      if (!xstrcmp(((struct attr*)(elem->attr.nodes[i]))->renpair[0],attrName))
	return ucc(((struct attr*)(elem->attr.nodes[i]))->renpair[1]);
    }
  return (unsigned const char *)"";
}
/* return internal, non-xmlified form of attr */
unsigned const char *
getAttrI(struct node *elem, const char *attrName)
{
  int i;
  for (i = 0; i < elem->attr.lastused; ++i)
    {
      if (!xstrcmp(((struct attr*)(elem->attr.nodes[i]))->renpair[0],attrName))
	return ucc(((struct attr*)(elem->attr.nodes[i]))->valpair[1]);
    }
  return (unsigned const char *)"";
}

void
removeAttr(struct node *elem, const char *attrName)
{
  int i;
  for (i = 0; i < elem->attr.lastused; ++i)
    {
      if (!xstrcmp(((struct attr*)(elem->attr.nodes[i]))->renpair[0],attrName))
	{
	  int dest = i++;
	  while (i < elem->attr.lastused)
	    elem->attr.nodes[dest++] = elem->attr.nodes[i++];
	  --elem->attr.lastused;
	  return;
	}
    }
}

struct node*
insertBefore(struct node *parent,int cindex,struct node *r)
{
  if (parent->children.lastused > cindex)
    {
      check_node_list((&parent->children));
      ++parent->children.lastused;
      memmove(&parent->children.nodes[parent->children.lastused-1],
	      &parent->children.nodes[cindex],
	      sizeof(struct node*) * (parent->children.lastused-1-cindex));
      parent->children.nodes[cindex] = r;
      r->parent = parent;
    }
  else
    {
      appendChild(parent,r);
    }
  return r;
}

struct node*
lastChild(struct node*elem)
{
  if (!elem)
    return NULL;
  if (elem->children.lastused == 0)
    return NULL;
  return elem->children.nodes[elem->children.lastused-1];
}

struct attr *
newattr()
{
  if (a_lastused == a_lastnode)
    {
      if (a_block_lastused == (A_BLOCK_SIZE-1))
	{
	  fprintf(stderr,"atf2xtf: too many attribute blocks\n");
	  exit(2);
	}
      if (++a_block_lastused > a_block_lastallocated)
	{
	  a_blocks[++a_block_lastallocated] = calloc(A_BLOCK_SIZE, sizeof(struct attr));
	}
      a_lastnode = A_BLOCK_SIZE;
      a_lastused = 0;
    }
  return &a_blocks[a_block_lastused][a_lastused++];
}

struct attr *
gnewattr()
{
  if (a_glastused == a_glastnode)
    {
      if (a_gblock_lastused == (A_gBLOCK_SIZE-1))
	{
	  fprintf(stderr,"atf2xtf: too many gattribute blocks\n");
	  exit(2);
	}
      if (++a_gblock_lastused > a_gblock_lastallocated)
	{
	  a_gblocks[++a_gblock_lastallocated] = calloc(A_gBLOCK_SIZE, sizeof(struct attr));
	}
      a_glastnode = A_gBLOCK_SIZE;
      a_glastused = 0;
    }
  return &a_gblocks[a_gblock_lastused][a_glastused++];
}

struct node *
newnode()
{
  if (lastused == lastnode)
    {
      if (block_lastused == (BLOCK_SIZE-1))
	{
	  fprintf(stderr,"atf2xtf: too many node blocks\n");
	  exit(2);
	}
      if (++block_lastused > block_lastallocated)
	{
	  blocks[++block_lastallocated] = calloc(BLOCK_SIZE, sizeof(struct node));
	}
      lastnode = BLOCK_SIZE;
      lastused = 0;
    }
  /*blocks[block_lastused][lastused] = empty_node;*/
  return &blocks[block_lastused][lastused++];
}

struct node *
gnewnode()
{
  if (glastused == glastnode)
    {
      if (gblock_lastused == (gBLOCK_SIZE-1))
	{
	  fprintf(stderr,"atf2xtf: too many gnode blocks\n");
	  exit(2);
	}
      if (++gblock_lastused > gblock_lastallocated)
	{
	  gblocks[++gblock_lastallocated] = calloc(gBLOCK_SIZE, sizeof(struct node));
	}
      glastnode = gBLOCK_SIZE;
      glastused = 0;
    }
  return &gblocks[gblock_lastused][glastused++];
}

struct node*
removeLastChild(struct node*elem)
{
  if (!elem)
    return NULL;
  if (elem->children.lastused == 0)
    return NULL;
  return elem->children.nodes[--elem->children.lastused];
}

struct node *
replaceChild(struct node *parent,int cindex,struct node *r)
{
  if (parent->children.lastused > 0)
    {
      parent->children.nodes[0] = r;
      r->parent = parent;
    }
  else
    appendChild(parent,r);
  return r;
}

void
serialize(struct node*n, int with_xcl)
{
  register int i;

  if (!n)
    {
      fprintf(stderr,"attempt to render NULL node\n");
      return;
    }
    
  if (!n->type)
    {
      fprintf(stderr,"attempt to render node with NULL type\n");
      return;
    }
  if (*n->type == 't')
    {
      fputs((const char*)n->data,f_xml);
      return;
    }

  if (!xstrcmp(n->type,"cdata"))
    {
      fputs("<![CDATA[",f_xml);
      fputs((const char*)n->data,f_xml);
      fputs("]]>",f_xml);
      return;
    }

  if (n->lnum && n->level < CELL)
    atf_lnum_pi(n);

  fputc('<',f_xml);
  fputs(n->names->pname,f_xml);

  if (n->level == TEXT && n->xmlns)
    {
      const char *const*ns = n->xmlns;
      while (ns[0][0])
	{
	  fprintf(f_xml," %s=\"%s\"",ns[0],ns[1]);
	  ns += 2;
	}
    }

  if (n->attr.lastused)
    for (i = 0; i < n->attr.lastused; ++i)
      {
	fputc(' ',f_xml);
	fputs((const char *)((struct attr*)(n->attr.nodes[i]))->renpair[0],f_xml);
	fputs("=\"",f_xml);
	fputs((const char*)((struct attr*)(n->attr.nodes[i]))->renpair[1],f_xml);
	fputc('"',f_xml);
      }

  fputc('>',f_xml);
  if (pretty && n->level < WORD  && xstrcmp(n->names->pname,"cmt"))
    fputc('\n',f_xml);
  for (i = 0; i < n->children.lastused; ++i)
    serialize_kids(n->children.nodes[i]);

  if (with_xcl)
    {
      extern struct xcl_context *text_xc;
      (*xcl_serialize_p)(text_xc,f_xml,0);
    }

  fprintf(f_xml,"</%s>",n->names->pname);
  if (pretty && n->level < GRAPHEME)
    fputc('\n',f_xml);
}

/* this routine never processes nodes where n->level == TEXT */
static void
serialize_kids(struct node*n)
{
  register int i;

  if (!n)
    {
      fprintf(stderr,"attempt to render NULL node\n");
      return;
    }
    
  if (!n->type)
    {
      fprintf(stderr,"attempt to render node with NULL type\n");
      return;
    }
  if (*n->type == 't')
    {
      if (n->data)
	fputs((const char*)n->data,f_xml);
      return;
    }

  if (!xstrcmp(n->type,"cdata"))
    {
      fputs("<![CDATA[",f_xml);
      fputs((const char*)n->data,f_xml);
      fputs("]]>",f_xml);
      return;
    }

  if (n->lnum && n->level < CELL)
    atf_lnum_pi(n);

  fputc('<',f_xml);
  fputs(n->names->pname,f_xml);

  if (n->attr.lastused)
    for (i = 0; i < n->attr.lastused; ++i)
      {
	fputc(' ',f_xml);
	fputs((const char *)((struct attr*)(n->attr.nodes[i]))->renpair[0],f_xml);
	fputs("=\"",f_xml);
	fputs((const char*)((struct attr*)(n->attr.nodes[i]))->renpair[1],f_xml);
	fputc('"',f_xml);
      }

  if (n->children.lastused)
    {
      fputc('>',f_xml);
      if (pretty && n->level < WORD  && xstrcmp(n->names->pname,"cmt"))
	fputc('\n',f_xml);
      for (i = 0; i < n->children.lastused; ++i)
	serialize_kids(n->children.nodes[i]);
      fprintf(f_xml,"</%s>",n->names->pname);
    }
  else
    {
      fputs("/>",f_xml);
    }
  if (pretty && n->level < GRAPHEME)
    fputc('\n',f_xml);
}

int
setAttr(struct node *elem, enum a_type a, const unsigned char *value)
{
  int i;
  if (!elem)
    return -1;

  if (elem->attr.lastused)
    {
      for (i = 0; i < elem->attr.lastused; ++i)
	if (!xstrcmp(((struct attr*)(elem->attr.nodes[i]))->renpair[0],
		     anames[a].pname))
	  break;
      if (i < elem->attr.lastused)
	{
	  elem->attr.nodes[i] = attr(a,value);
	  return 1;
	}
      else
	addToNodeList(&elem->attr,attr(a,value));
    }
  else
    addToNodeList(&elem->attr,attr(a,value));
  return 0;
}

int
gsetAttr(struct node *elem, enum a_type a, const unsigned char *value)
{
  int i;
  if (elem->attr.lastused)
    {
      for (i = 0; i < elem->attr.lastused; ++i)
	if (!xstrcmp(((struct attr*)(elem->attr.nodes[i]))->renpair[0],
		     anames[a].pname))
	  break;
      if (i < elem->attr.lastused)
	{
	  elem->attr.nodes[i] = gattr(a,value);
	  return 1;
	}
      else
	addToNodeList(&elem->attr,gattr(a,value));
    }
  else
    addToNodeList(&elem->attr,gattr(a,value));
  return 0;
}

void
setName(struct node *elem, enum e_type e)
{
  elem->etype = e;
  elem->names = &enames[e];
}

struct node *
textElem(enum e_type e, struct node *parent, int lnum, enum block_levels b,
	 unsigned char *data)
{
  struct node *ep = elem(e,parent,lnum,b);
  struct node *t = textNode(data);
  appendChild(ep,t);
  return ep;
}

struct node *
gtextElem(enum e_type e, struct node *parent, int lnum, enum block_levels b,
	 const unsigned char *data)
{
  struct node *ep = gelem(e,parent,lnum,b);
  struct node *t = gtextNode(data);
  appendChild(ep,t);
  return ep;
}

struct node *
textNode(const unsigned char *data)
{
  struct node *t = newnode();
  t->type = "t";
  t->data = cdf_xmlify(data);
  return t;
}

struct node *
gtextNode(const unsigned char *data)
{
  struct node *t = gnewnode();
  t->type = "t";
  t->data = cdf_xmlify(data);
  return t;
}

unsigned char *
cdf_xmlify(const unsigned char *s)
{
  static unsigned char *buf = NULL;
  static int bufsize = 0;

  if (!s)
    {
      free(buf);
      buf = NULL;
      bufsize = 0;
      return NULL;
    }

  if (strpbrk(cc(s),"<&\""))
    {
      int bufused = 0;
      register unsigned char *p = buf;
      while (1+(10*xxstrlen(s)) > bufsize)
	{
	  if (!bufsize)
	    bufsize = 4096;
	  else
	    bufsize *= 2;
	  buf = realloc(buf,bufsize);
	}
      while (*s)
	{
	  register const char *x = xmlchars[*s];
	  while (*x)
	    buf[bufused++] = *x++;
	  ++s;
	}
      buf[bufused] = '\0';
      p = pool_copy(buf);
      /*      free(buf); */
      return p;
      /*      return realloc(buf,xxstrlen(buf)+1); */
    }
  else
    return pool_copy(s);
}

unsigned char *
xmlify_no_pool(const unsigned char *s)
{
  if (strpbrk(cc(s),"<&\""))
    {
      static unsigned char *buf;
      static int bufsize = 0;
      int bufused = 0;
      while (1+(10*xxstrlen(s)) > bufsize)
	{
	  if (!bufsize)
	    bufsize = 4096;
	  else
	    bufsize *= 2;
	  buf = realloc(buf,bufsize);
	}
      while (*s)
	{
	  register const char *x = xmlchars[*s];
	  while (*x)
	    buf[bufused++] = *x++;
	  ++s;
	}
      buf[bufused] = '\0';
      return buf;
    }
  else
    return uc(s);
}

void
atf_file_pi(const char *file)
{
  if (!no_pi)
    fprintf(f_xml,"<?atf-file %s?>",file);
}

void
atf_lnum_pi(struct node *n)
{
  if (!no_pi)
    {
      fputs("<?atf-line ",f_xml);
      fprintf(f_xml,"%d",n->lnum);
      fputs("?>",f_xml);
      if (pretty)
	fputc('\n',f_xml);
    }
}

void
xml_decl()
{
  fprintf(f_xml,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
}

void
set_or_append_attr(struct node *n, enum a_type atype, const char *aname,
		   unsigned char *text)
{
  const unsigned char *already = getAttr(n,aname);
  if (*already)
    {
      unsigned char *tmp = malloc(strlen((char*)already)+strlen((char*)text)+2);
      sprintf((char*)tmp,"%s,%s",(char*)already,text);
      setAttr(n, atype, pool_copy(tmp));
      free(tmp);
    }
  else
    appendAttr(n,attr(atype,pool_copy(text)));
}


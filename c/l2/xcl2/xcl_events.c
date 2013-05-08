#include <stdlib.h>
#include <stdio.h>
#include <ctype128.h>
#include <string.h>
#include <f2.h>
#include <ilem_form.h>
#include "xcl.h"
#include "npool.h"

#ifndef strdup
char *strdup(const char *);
#endif

#define add_child xcl_add_child

const char *default_discourse_level = "body";

#define BLOCK_SIZE 2048

struct mm
{
  char **blocks;
  int node_size;
  int block_lastused;
  int block_lastallocated;
  int lastused;
  int lastnode;
};

static struct npool *xcl_pool;

static struct mm c_mm_info = { NULL, sizeof(struct xcl_c), -1, -1, 0, 0 };
static struct mm d_mm_info = { NULL, sizeof(struct xcl_d), -1, -1, 0, 0 };
static struct mm l_mm_info = { NULL, sizeof(struct xcl_l), -1, -1, 0, 0 };
static struct mm u_mm_info = { NULL, sizeof(union  xcl_u), -1, -1, 0, 0 };

static void *
new_node(struct mm* i)
{
  if (i->lastused == i->lastnode)
    {
      if (i->block_lastused == (BLOCK_SIZE-1))
	{
	  fprintf(stderr,"atf2xtf: too many node blocks\n");
	  exit(2);
	}
      if (++i->block_lastused > i->block_lastallocated)
	{
	  i->blocks[++i->block_lastallocated] = calloc(BLOCK_SIZE, i->node_size);
	}
      i->lastnode = BLOCK_SIZE;
      i->lastused = 0;
    }
  return &i->blocks[i->block_lastused][i->node_size*i->lastused++];
}

static void
mm_free(struct mm *m)
{
  int i = 0;
  while (i <= m->block_lastused)
    free(m->blocks[i++]);
  free(m->blocks);
  m->blocks = NULL;
  m->block_lastused = m->block_lastallocated = -1;
  m->lastused = m->lastnode = 0;
}

struct xcl_context *
xcl_create()
{
  struct xcl_context *xc = calloc(1, sizeof(struct xcl_context));
  if (c_mm_info.blocks == NULL)
    {
      c_mm_info.blocks = calloc(BLOCK_SIZE, sizeof(char*));
      d_mm_info.blocks = calloc(BLOCK_SIZE, sizeof(char*));
      l_mm_info.blocks = calloc(BLOCK_SIZE, sizeof(char*));
      u_mm_info.blocks = calloc(BLOCK_SIZE, sizeof(union xcl_u));
    }
  if (!xcl_pool)
    xcl_pool = npool_init();
  xc->pool = xcl_pool;
  xc->langs = xc->project = xc->textid = NULL;
  xc->psus = hash_create(1);
  return xc;
}

static void
xcl_free_tree(void *vp)
{
  enum xcl_node_types t;
  int i;

  if (!vp)
    return;

  t = ((struct xcl_c *)vp)->node_type;

#define cp ((struct xcl_c*)vp)
#define dp ((struct xcl_d*)vp)
#define lp ((struct xcl_l*)vp)

  switch (t)
    {
    case xcl_node_c:
      /* free((char*)cp->id); */
      if (cp->meta)
	hash_free(cp->meta,NULL);
      for (i = 0; i < cp->nchildren; ++i)
	{
	  switch (cp->children[i].c->node_type)
	    {
	    case xcl_node_c:
	      xcl_free_tree(cp->children[i].c);
	      break;
	    case xcl_node_d:
	      xcl_free_tree(cp->children[i].d);
	      break;
	    case xcl_node_l:
	      xcl_free_tree(cp->children[i].l);
	      break;
	    }
	}
      if (cp->children)
	free(cp->children);
      break;
    case xcl_node_d:
      
      break;
    case xcl_node_l:
#if 0
      if (lp->sigs_found)
	free(lp->sigs_found);
#endif
      break;
    }

#undef cp
#undef dp
#undef lp
}

void
xcl_final_term(void)
{
  if (xcl_pool)
    {
      npool_term(xcl_pool);
      xcl_pool = NULL;
    }
}

void
xcl_destroy(struct xcl_context **xc)
{
  if (xc && *xc)
    {
      if ((*xc)->root)
	xcl_free_tree((*xc)->root);
      mm_free(&c_mm_info);
      mm_free(&d_mm_info);
      mm_free(&l_mm_info);
      mm_free(&u_mm_info);
#if 0
      npool_term((*xc)->pool);
#endif
      hash_free((*xc)->psus,(hash_free_func*)list_free);
      linkbase_free((*xc)->linkbase);
#if 0 /* we should be using one global sig context now */
      if ((*xc)->sigs)
	sig_context_term((*xc)->sigs);
#endif
      free(*xc);
      *xc = NULL;
    }
}

void
xcl_add_child(struct xcl_c*p, void *c, enum xcl_node_types type)
{
  union xcl_u *up;
  if (p->nchildren == p->children_alloced)
    {
      p->children_alloced += 16;
      p->children = realloc(p->children, p->children_alloced * sizeof(union xcl_u));
    }
  up = &p->children[p->nchildren++];
  switch (type)
    {
    case xcl_node_c:
      up->c = (struct xcl_c*)c;
      break;
    case xcl_node_d:
      up->d = (struct xcl_d*)c;
      break;
    case xcl_node_l:
      up->l = (struct xcl_l*)c;
      break;
    }
}

const char *
xcl_chunk_id(const char *idbase, enum xcl_c_types t, struct xcl_context *xc)
{
  static const char *base = NULL;
  static int uid;
  if (idbase)
    {
      base = idbase;
      uid = 0;
      return NULL;
    }
  else /* if (t == xcl_c_sentence) */
    {
      static char buf[32];
      sprintf(buf,"%s.U%d",base,uid++);
      return (const char*)npool_copy((unsigned char *)buf,xc->pool);
    }
}

void
xcl_chunk(struct xcl_context *xc, const char *xml_id, enum xcl_c_types t)
{
  struct xcl_c *c = new_node(&c_mm_info);
  extern Hash_table *curr_meta;
  c->node_type = xcl_node_c;
  c->type = t;
  if (xml_id)
    c->id = (const char *)npool_copy((unsigned char *)xml_id,xc->pool);
  else
    c->id = xcl_chunk_id(NULL, t, xc);
  c->parent = xc->curr;
  c->xc = xc;
  c->meta = curr_meta;
  if (c->parent)
    c->ancestor_meta = c->parent->meta;
  c->children = NULL;
  if (xc->curr)
    {
      add_child(xc->curr, c, c->node_type);
      c->level = xc->curr->level + 1;
    }
  else
    {
      xc->root = c;
      xc->root->level = c->level = 0;
    }
  c->bracketing_level = 0;
  xc->curr = c;
}

void
xcl_chunk_end(struct xcl_context *xc)
{
  if (xc->curr)
    xc->curr = xc->curr->parent;
}

void
xcl_chunk_insert(struct xcl_c *curr_c, const char *xml_id, const char *ref,
		 enum xcl_c_types t, const char *subtype,
		 int first_child, int ncopy)
{
  struct xcl_c *c = new_node(&c_mm_info);
  int remaining_children = curr_c->nchildren - (first_child+ncopy);
  c->node_type = xcl_node_c;
  c->type = t;
  c->subtype = subtype;
  if (xml_id)
    c->id = (const char *)npool_copy((unsigned char *)xml_id,curr_c->xc->pool);
  else
    c->id = xcl_chunk_id(NULL, t,curr_c->xc);
  c->ref = ref;
  c->parent = curr_c;
  c->xc = curr_c->xc;
  c->nchildren = c->children_alloced = ncopy;
  c->children = malloc(ncopy * sizeof(union xcl_u *));
  memcpy(c->children,&curr_c->children[first_child],ncopy*sizeof(union xcl_u *));
  curr_c->children[first_child].c = c;
  if (remaining_children > 0 && ncopy > 1)
    {
      memmove(&curr_c->children[first_child+1],&curr_c->children[first_child+ncopy],
	     remaining_children*sizeof(union xcl_u*));
      curr_c->nchildren = curr_c->nchildren-(ncopy-1);
    }
  else
    curr_c->nchildren -= (ncopy-1);
}

Hash_table *
xcl_create_meta(struct xcl_context *xc, const char *xml_id)
{
  xc->curr->meta = hash_create(1);
  hash_add(xc->curr->meta,
	   npool_copy((unsigned char *)"#xml:id",xc->pool),
	   npool_copy((unsigned char *)xml_id,xc->pool));
  return xc->curr->meta;
}

Hash_table *
xcl_hash_lemm_meta(const char *const*lmeta, const char *xml_id,
		   struct xcl_context *xc)
{
  Hash_table *tmp = hash_create(1);
  const char *const*freeable_lmeta = lmeta;
  hash_add(tmp,
	   npool_copy((unsigned char *)"#xml:id", xc->pool),
	   npool_copy((unsigned char *)xml_id,xc->pool));

  while (*lmeta)
    {
      hash_add(tmp,
	       npool_copy((unsigned char *)*lmeta,xc->pool),
	       lmeta[1] ? npool_copy((unsigned char *)lmeta[1],xc->pool) : (unsigned char *)"");
      lmeta += 2;
    }
  free((char*)freeable_lmeta);
  return tmp;
}

void
xcl_discontinuity(struct xcl_context *xc, const char *ref, enum xcl_d_types t, const char *st)
{
  struct xcl_d *c = new_node(&d_mm_info);
  c->node_type = xcl_node_d;
  c->type = t;
  c->subtype = st;
  c->xc = xc;
  c->ref = ref;
  add_child(xc->curr, c, c->node_type);
}

void
xcl_discontinuity2(struct xcl_context *xc, const char *xid, const char *ref, enum xcl_d_types t)
{
  struct xcl_d *c = new_node(&d_mm_info);
  c->node_type = xcl_node_d;
  c->type = t;
  c->xc = xc;
  c->xml_id = xid;
  c->ref = ref;
  add_child(xc->curr, c, c->node_type);
}

static const char *
lemm_id(struct xcl_context *xc)
{
  static int lindex = 0;
  static char buf[32];
  sprintf(buf,"%s.l%05x",xc->textid,lindex++);
  return buf;
}

struct xcl_l *
xcl_lemma(struct xcl_context *xc, const char *xml_id, const char *ref, 
	  struct ilem_form *fp, void *user, enum ll_types altflag)
{
  static int ll_used[ll_top];
  
  if (altflag && ll_used[altflag])
    {
      struct ilem_form *f = xc->curr->children[xc->curr->nchildren-1].l->f;
      ++ll_used[altflag];
      switch (altflag)
	{
	case ll_ambig:
	  ++f->acount;
	  while (f->ambig)
	    f = f->ambig;
	  f->ambig = fp;
	  break;
	case ll_finds:
	  ++f->fcount;
	  f->finds = realloc(f->finds,f->fcount);
	  f->finds[f->fcount-1] = fp;
	  break;
	case ll_top:
	case ll_none:
	case ll_bad:
	  ;
	}
      return NULL;
    }
  else
    {
      struct xcl_l *c = new_node(&l_mm_info);
      c->node_type = xcl_node_l;
      if (xml_id)
	c->xml_id = xml_id;
      else
	c->xml_id = (char*)npool_copy((unsigned char*)lemm_id(xc),xc->pool);
      c->ref = ref;
      c->f = fp;
      c->user = user;
      c->parent = xc->curr;
      c->nth = xc->curr->nchildren;
      c->xc = xc;
      add_child(xc->curr,c,c->node_type);
      return c;
    }
}

/* FIXME: inserting clauses and phrases is more 
   complicated than this routine can handle at
   present */
void
xcl_insert_ub(struct xcl_context *xc, int nlem, enum xcl_c_types t, int bracketing_level)
{
  struct xcl_l *carry_over = NULL;
  if (nlem)
    {
      carry_over = xc->curr->children[xc->curr->nchildren-1].l;
      --xc->curr->nchildren;
    }

  if (t != xc->curr->type)
    {
      if (t == xcl_c_sentence)
	{
	  /* clear until we are at sentence level */
	  xcl_unsentence(xc);
	  /* Now up one to the discourse level */
	  xc->curr = xc->curr->parent;
	}
      else if (t == xcl_c_phrase)
	{
	  /* This should not be able to happen any more */
	  if (xc->curr->type == xcl_c_sentence)
	    {
	      /* wrap stuff so far in a new phrase and append that to 
		 sentence before proceeding */
	      if (xc->curr->nchildren)
		{
		  if (xc->curr->children[xc->curr->nchildren-1].c->node_type != xcl_node_c
		      || xc->curr->children[xc->curr->nchildren-1].c->type != xcl_c_phrase
		      || xc->curr->children[xc->curr->nchildren-1].c->bracketing_level != bracketing_level)
		    {
		      xcl_chunk_insert(xc->curr, NULL, NULL, t, NULL, 0, xc->curr->nchildren);
		      xc->curr->children[xc->curr->nchildren-1].c->bracketing_level = bracketing_level;
		    }
		}
	    }
	  else
	    {
	      extern int lnum;
	      fprintf(stderr, "%s: %d: xcl_insert_ub: unhandled case\n", file, lnum);
	    }
	}
    }
  else
    {
      xcl_chunk_end(xc);
    }

  /* Because we unconditionally add a chunk here we must be 
     careful to suppress empty chunks when we dump */
  xcl_chunk(xc, NULL, t);
  xc->curr->bracketing_level = bracketing_level;
  xc->curr->subtype = "#auto";
  
  if (carry_over)
    add_child(xc->curr,carry_over,carry_over->node_type);
  
  /* Now clear the current chunk so that ilem_para parsing levels work correctly */
  /* xc->curr = xc->curr->parent; */
}

void
xcl_unsentence(struct xcl_context *xc)
{
  do
    xc->curr = xc->curr->parent;
  while (xc->curr->type != xcl_c_sentence && xc->curr->parent);
}

void
xcl_add_discourse(struct xcl_context *xc, const char *discourse)
{
  xcl_chunk(xc,NULL,xcl_c_discourse);
  xc->curr->subtype = discourse;
  /*  xcl_chunk(xc,NULL,xcl_c_sentence); */
}

static int
get_discourse_type(const char *d)
{
  if (!d)
    return 1;
  else if (!strcmp(d, "body") 
	   || !strcmp(d, "catchline")
	   || !strcmp(d, "colophon")
	   || !strcmp(d, "sealings")
	   || !strcmp(d, "signatures")
	   || !strcmp(d, "witnesses"))
    return 2;
  else
    return 3;
}

void
xcl_fix_context(struct xcl_context *xc, const unsigned char *discourse)
{
  if (discourse)
    {
      int this_level = 0;
      if (xc->curr->type == xcl_c_sentence
	  && xc->curr->nchildren == 0)
	{
	  /* delete the empty sentence */
	  xc->curr = xc->curr->parent;
	  --xc->curr->nchildren;
	}
#if 1
      /* Clear container discourses:
       *   discourse tags are assigned a level
       *   get the level for the new tag
       *   find the ancestor discourse at same level
       *   append new discourse unit to list
       */
      this_level = get_discourse_type((const char *)discourse);
      while (1)
	{
	  if (xc->curr->type == xcl_c_discourse)
	    {
	      int up_level = get_discourse_type((const char *)xc->curr->subtype);
	      if (this_level - up_level == 1)
		break;
	      else
		xc->curr = xc->curr->parent;
	    }
	  else if (xc->curr->parent)
	    xc->curr = xc->curr->parent;
	  else
	    break;
	}
      xcl_add_discourse(xc,(const char *)discourse);
#else
      if (xc->curr->type == xcl_c_sentence)
	xc->curr = xc->curr->parent->parent;
      else if (xc->curr->type == xcl_c_discourse)
	{
	  if (strcmp((const char *)discourse, "signature")
	      || strcmp((const char *)xc->curr->subtype, "sealings"))
	    {
	      xc->curr = xc->curr->parent;
	      if (xc->curr->type == xcl_c_discourse
		  && !strcmp((const char *)xc->curr->subtype, "sealings"))
		xc->curr = xc->curr->parent;
	    }
	}
      xcl_add_discourse(xc,(const char *)discourse);
#endif
    }
  else
    {
      if (xc->curr->type == xcl_c_discourse
	  && (get_discourse_type(default_discourse_level)
	      <= get_discourse_type(xc->curr->subtype)))
	;
      else if (xc->curr->type != xcl_c_sentence
	       && xc->curr->type != xcl_c_phrase)
	xcl_add_discourse(xc,default_discourse_level);

      if (xc->curr->type == xcl_c_discourse)
	xcl_chunk(xc, NULL, xcl_c_sentence);
      if (xc->curr->type == xcl_c_sentence)
	xcl_chunk(xc, NULL, xcl_c_phrase);
    }
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include <prop.h>
#include <ctype128.h>
#include "gdl.tab.h"
#include "gdlstate.h"
#include "gdl.h"
#include "gvl.h"

extern const char *currgdlfile;
extern int gdltrace, gdllineno, gdl_legacy;
extern void gdl_wrapup_buffer(void);
extern void gdl_validate(Tree *tp);
int curr_lang = 's';

int deep_parse = 1;

gdlstate_t gst; /* global gdl state */
Node *lgp = NULL;   /* last grapheme node pointer */

/***********************************************************************
 *
 * Functions for running gdl.y
 *
 ***********************************************************************/

void
gdlparse_deep(Node *np, void *mptr)
{
  if (np->user)
    {
      gvl_g *gp = np->user;
      if (gp->orig && '|' != *gp->orig && gp->sign && '|' == *gp->sign)
	{
	  Tree *tp = tree_init();
	  Mloc *m = mptr;
	  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
	  gdl_setup_buffer((char*)pool_copy((ucp)gp->sign, gdlpool));
	  gdl_set_tree(tp);
	  gdl_lex_init(m->file, m->line);
	  gdlparse();
	  gdl_wrapup_buffer();
	  gp->deep = tp->root->kids;
	}
    }
}

Tree *
gdlparse_string(Mloc *m, char *s)
{
  Tree *tp = tree_init();
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
  gdl_setup_buffer(s);
  gdl_set_tree(tp);
  gdl_lex_init(m->file, m->line);
  gdlparse();
  gdl_wrapup_buffer();
  if (deep_parse)
    tree_iterator(tp, m, gdlparse_deep, NULL);
  return tp;
}

void
gdlparse_init(void)
{
  gdl_init();
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
  gdl_term();
}

/***********************************************************************
 *
 * Utility routines for the tree-builder
 *
 ***********************************************************************/

void
gdl_gp_type(Tree *ytp, enum gdlpropvals p)
{
  gdl_prop(ytp->curr, p, PG_GDL_GROUP);
}

#if 0
void
gdl_node_type(Node *np, enum gdlpropvals p)
{
  gdl_prop(np, p, PG_GDL_GROUP);
}
#endif

static Node *
gdl_graph_node(Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  lgp = np;
  prop_state(np, &gst);
  return np;
}

static Node *
gdl_meta_node(Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  return np;
}

Node *
gdl_pop(Tree *ytp, const char *s)
{
  Node *np = tree_pop(ytp);
  if (!strcmp(np->name, s))
    /* error */;
  return np;
}

Node *
gdl_push(Tree *ytp, const char *s)
{
  tree_add(ytp, NS_GDL, s, ytp->curr->depth, NULL);
  return tree_push(ytp);
}

void
gdl_prop(Node *ynp, int ptype, int gtype)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->propmem, ynp->props, ptype, gtype);
  else
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_prop_kv(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp)
    prop_node_add(ynp, ptype, gtype, k, v);
  else
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_remove_q_error(Mloc m, Node *ynp)
{
  int rem = 0;
  (void)((rem = mesg_remove_error(m.file, m.line, "must be qualified"))
	 || (rem = mesg_remove_error(m.file, m.line, "unknown value"))
	 || (rem = mesg_remove_error(m.file, m.line, "unknown sign")));
  if (rem)
    {
      if (gdltrace)
	mesg_err(&m, "gdl_remove_q_error succeeded");
      ((gvl_g*)(ynp->user))->mess = NULL;
    }
}

/***********************************************************************
 *
 * Tree-building routines called from gdl.y
 *
 ***********************************************************************/

/* Used when a number is not followed by a '('; need to sexify nums here */
Node *
gdl_barenum(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt BARENUM: %s\n", data);
  return gdl_graph_node(ytp, "g:B", data);
}

void
gdl_cell(Tree *ytp, const char *span)
{
  Node *cp = NULL;
  Node *ancestor = NULL;

  if ('&' == *span)
    ++span;
  if (gdltrace)
    fprintf(stderr, "gt: CELL with SPAN = %s\n", span);

  if (!(ancestor = node_ancestor_or_self(ytp->curr, "g:cell")))
    {
      cp = tree_node(ytp, NS_GDL, "g:cell", ytp->root->depth+1, NULL);
      gdl_prop(cp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", "1");
      /* NB: This assumes GDL parser will never be embedded in another grammar */
      (void)node_insert(ytp->root, cp);
      tree_curr(ytp->root);
    }
  else
    tree_curr(ancestor->rent);
  cp = tree_add(ytp, NS_GDL, "g:cell", ytp->root->depth+1, NULL);
  gdl_prop_kv(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", (ccp)pool_copy((uccp)span, ytp->pool));
  tree_curr(cp);
}

Node *
gdl_delim(Tree *ytp, const char *data)
{
  Node *np = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: DELIM: %c\n", '-');
  np = tree_add(ytp, NS_GDL, "g:d", ytp->curr->depth, NULL); 
  np->text = data;
  return np;
}

void
gdl_field(Tree *ytp, const char *ftype)
{
  Node *fp = NULL;
  Node *ancestor = NULL;

  if ('!' == *ftype)
    ++ftype;
  if (gdltrace)
    fprintf(stderr, "gt: FIELD with TYPE = %s\n", ftype);

  if (!(ancestor = node_ancestor_or_self(ytp->curr, "g:field")))
    {
      Node *cellp = node_ancestor_or_self(ytp->curr, "g:cell");
      
      fp = tree_node(ytp, NS_GDL, "g:field", ytp->root->depth+1, NULL);
      gdl_prop(fp, GP_IMPLICIT, PG_GDL_INFO);
      gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", "default");

      /* NB: This assumes GDL parser will never be embedded in another grammar */
      if (!cellp)
	cellp = ytp->root;
      (void)node_insert(cellp, fp);
      tree_curr(cellp);
    }
  else
    tree_curr(ancestor->rent);
  fp = tree_add(ytp, NS_GDL, "g:field", ytp->root->depth+1, NULL);
  gdl_prop_kv(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", (ccp)pool_copy((uccp)ftype, ytp->pool));
  tree_curr(fp);
}

Node *
gdl_graph(Tree *ytp, const char *data)
{
  extern int g_literal_flag, g_logoforce_flag;
  const char *gname = NULL;
  Node *ret = NULL;

  if (ytp->curr->kids && 'R' == ytp->curr->kids->name[2])
    gname = "g:N";
  else
    gname = "g:g";
  
  if (gdltrace)
    fprintf(stderr, "gt: GRAPH[%s]: %s\n", gname, data);

  ret = gdl_graph_node(ytp, gname, data);
  if (g_literal_flag)
    {
      gdl_update_state(ret, gs_force|gs_g_undefined);
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
      gdl_update_state(ret, gs_force|gs_g_flogo1);
      g_logoforce_flag = 0;
    }
  return ret;
}

Node *
gdl_lang(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LANG: %s\n", data);
  if (strstr(data, "/n"))
    curr_lang = 'n';
  else
    curr_lang = data[1];
  return gdl_meta_node(ytp, "g:z", data);
}

Node *
gdl_listnum(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: LISTNUM: %s\n", data);
  return gdl_graph_node(ytp, "g:l", data);
}

Node *
gdl_nongraph(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: NONGRAPH: %s\n", data);
  return gdl_graph_node(ytp, "g:x", data);
}

/* This is triggered by [0-9]/( so we know its a repetition number */
Node *
gdl_number(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: NUMBER: %s\n", data);
  return gdl_graph_node(ytp, "g:R", data);
}

Node *
gdl_punct(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: PUNCT: %s\n", data);
  return gdl_graph_node(ytp, "g:p", data);
}

Node *
gdl_break_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  ret = gdl_meta_node(ytp, "g:z", data);
  gs_on(gs_tok);
  return ret;
}

Node *
gdl_break_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  gdl_update_state(lgp, gs_tok);
  return gdl_meta_node(ytp, "g:z", data);
}

/* If data is /{{[0-9]+:/ the digits are a stream code */
Node *
gdl_gloss_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  int stream = -1;
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/o: %d=%s\n", tok, data);
  if (tok == L_cur_par && data[2])
    {
      stream = atoi(data+2);
      if (stream <= 0 || stream > 99)
	{
	  mesg_vwarning(currgdlfile, gdllineno, "stream out of range in '%s'", data);
	  stream = -1;
	}
      data = "{{";
    }
  (void)gdl_balance_state(mlp, tok, data);
  gdl_push(ytp, "g:glo");
  gs_on(gs_tok);
  ret = gdl_meta_node(ytp, "g:z", data);
  prop_node_add(ret, GP_STREAM, PG_GDL_STATE, (void*)(uintptr_t)stream, NULL);
  return ret;
}

Node *
gdl_gloss_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/c: %d=%s\n", tok, data);
  ret =  gdl_meta_node(ytp, "g:z", data);
  if (!gdl_balance_state(mlp, tok, data))
    gdl_pop(ytp, data);
  gdl_update_state(lgp, gs_tok);
  return ret;
}
Node *
gdl_state_o(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/o: %d=%s\n", tok, data);
  (void)gdl_balance_state(mlp, tok, data);
  ret = gdl_meta_node(ytp, "g:z", data);
  gs_on(gs_tok);
  return ret;
}

Node *
gdl_state_c(Mloc mlp, Tree *ytp, int tok, gdlstate_t gs_tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", tok, data);
  ret =  gdl_meta_node(ytp, "g:z", data);
  (void)gdl_balance_state(mlp, tok, data);
  gdl_update_state(lgp, gs_tok);
  return ret;
}

void
gdl_update_state(Node *np, gdlstate_t gs_tok)
{
  bit_set(np->props->u.s, gs_tok);
}

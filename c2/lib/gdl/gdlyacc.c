#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include <ctype128.h>
#include "gdl.tab.h"
#include "gdl.h"
#include "gvl.h"

extern const char *currgdlfile;
extern int gdltrace, gdllineno, gdl_legacy;
extern void gdl_wrapup_buffer(void);
extern void gdl_validate(Tree *tp);
int curr_lang = 's';

/***********************************************************************
 *
 * Functions for running gdl.y
 *
 ***********************************************************************/

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
  gdl_prop(ytp->curr, p, PG_GDL_GROUP, NULL, NULL);
}

void
gdl_node_type(Node *np, enum gdlpropvals p)
{
  gdl_prop(np, p, PG_GDL_GROUP, NULL, NULL);
}

static Node *
gdl_graph_node(Tree *ytp, const char *name, const char *data)
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
gdl_prop(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->propmem, ynp->props, ptype, gtype, k, v);
  else
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

void
gdl_remove_q_error(Mloc m, Node *ynp)
{
  if (mesg_remove_error(m.file, m.line, "must be qualified") && gdltrace)
    mesg_err(&m, "gdl_remove_q_error succeeded");
  else if (mesg_remove_error(m.file, m.line, "unknown value") && gdltrace)
    mesg_err(&m, "gdl_remove_q_error succeeded");
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
      gdl_prop(cp, GP_IMPLICIT, PG_GDL_INFO, NULL, NULL);
      gdl_prop(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", "1");
      /* NB: This assumes GDL parser will never be embedded in another grammar */
      (void)node_insert(ytp->root, cp);
      tree_curr(ytp->root);
    }
  else
    tree_curr(ancestor->rent);
  cp = tree_add(ytp, NS_GDL, "g:cell", ytp->root->depth+1, NULL);
  gdl_prop(cp, GP_ATTRIBUTE, PG_GDL_INFO, "span", (ccp)pool_copy((uccp)span, ytp->pool));
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
      gdl_prop(fp, GP_IMPLICIT, PG_GDL_INFO, NULL, NULL);
      gdl_prop(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", "default");

      /* NB: This assumes GDL parser will never be embedded in another grammar */
      if (!cellp)
	cellp = ytp->root;
      (void)node_insert(cellp, fp);
      tree_curr(cellp);
    }
  else
    tree_curr(ancestor->rent);
  fp = tree_add(ytp, NS_GDL, "g:field", ytp->root->depth+1, NULL);
  gdl_prop(fp, GP_ATTRIBUTE, PG_GDL_INFO, "field", (ccp)pool_copy((uccp)ftype, ytp->pool));
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
      gdl_prop(ret, '$', PG_GDL_FLAGS, NULL, NULL);
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
      gdl_prop(ret, '~', PG_GDL_FLAGS, NULL, NULL);
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
  return gdl_graph_node(ytp, "g:z", data);
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
gdl_break_o(Mloc mlp, Tree *ytp, int tok, const char *data, enum gdlpropvals gptype)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  ret = gdl_graph_node(ytp, "g:z", data);
  gdl_node_type(ret,gptype);
  return ret;
}

Node *
gdl_break_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  return gdl_graph_node(ytp, "g:z", data);
}

Node *
gdl_gloss_o(Mloc mlp, Tree *ytp, int tok, const char *data, enum gdlpropvals gptype)
{
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/o: %d=%s\n", tok, data);
  (void)gdl_balance_state(mlp, tok, data);
  gdl_push(ytp, "g:glo");
  gdl_gp_type(ytp,gptype);
  return gdl_graph_node(ytp, "g:z", data);
}

Node *
gdl_gloss_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/c: %d=%s\n", tok, data);
  ret =  gdl_graph_node(ytp, "g:z", data);
  if (!gdl_balance_state(mlp, tok, data))
    gdl_pop(ytp, data);
  return ret;
}
Node *
gdl_state_o(Mloc mlp, Tree *ytp, int tok, const char *data, enum gdlpropvals gptype)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/o: %d=%s\n", tok, data);
  (void)gdl_balance_state(mlp, tok, data);
  /*gdl_push(ytp, "g:gp");*/
  ret = gdl_graph_node(ytp, "g:z", data);
  gdl_node_type(ret,gptype);
  return ret;
}

Node *
gdl_state_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", tok, data);
  ret =  gdl_graph_node(ytp, "g:z", data);
#if 1
  (void)gdl_balance_state(mlp, tok, data);
#else
  if (!gdl_balance(mlp, tok, data))
    gdl_pop(ytp, data);
#endif
  return ret;
}

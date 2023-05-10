#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
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

struct gdlstate gst; /* global gdl state */
Node *lgp = NULL;   /* last grapheme node pointer */

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
  gdl_prop(ytp->curr, p, PG_GDL_GROUP);
}

void
gdl_node_type(Node *np, enum gdlpropvals p)
{
  gdl_prop(np, p, PG_GDL_GROUP);
}

static Node *
gdl_graph_node(Tree *ytp, const char *name, const char *data)
{
  Node *np = NULL;
  struct gdlstate *st = NULL;
  np = tree_add(ytp, NS_GDL, name, ytp->curr->depth, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  lgp = np;
  *(prop_state(np)) = gst;
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
  if (mesg_remove_error(m.file, m.line, "must be qualified") && gdltrace)
    mesg_err(&m, "gdl_remove_q_error succeeded");
  else if (mesg_remove_error(m.file, m.line, "unknown value") && gdltrace)
    mesg_err(&m, "gdl_remove_q_error succeeded");
  else if (mesg_remove_error(m.file, m.line, "unknown sign") && gdltrace)
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
      gdl_prop(ret, '$', PG_GDL_FLAGS);
      g_literal_flag = 0;
    }
  else if (g_logoforce_flag)
    {
      gdl_prop(ret, '~', PG_GDL_FLAGS);
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
gdl_break_o(Mloc mlp, Tree *ytp, int tok, const char *data, enum gdlpropvals gptype)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/o: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  ret = gdl_meta_node(ytp, "g:z", data);
  switch (tok)
    {
    case '[':
      gst.lost = SB_OP;
      break;
    case L_uhs:
    case L_lhs:
      gst.damaged = SB_OP;
      break;
    }
  gdl_node_type(ret,gptype);
  return ret;
}

Node *
gdl_break_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "gt: BREAK/c: %d=%s\n", tok, data);
  (void)gdl_balance_break(mlp, tok, data);
  gdl_update_closers(lgp, tok);
  switch (tok)
    {
    case ']':
      gst.lost = SB_NO;
      break;
    case R_uhs:
    case R_lhs:
      gst.damaged = SB_NO;
      break;
    }  
  return gdl_meta_node(ytp, "g:z", data);
}

Node *
gdl_gloss_o(Mloc mlp, Tree *ytp, int tok, const char *data, enum gdlpropvals gptype)
{
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/o: %d=%s\n", tok, data);
  (void)gdl_balance_state(mlp, tok, data);
  gdl_push(ytp, "g:glo");
  gdl_gp_type(ytp,gptype);
  switch (tok)
    {
    case L_cur_par:
      gst.glodoc = SB_OP;
      break;
    case L_dbl_cur:
      gst.glolin = SB_OP;
      break;
    case L_ang_par_s:
      gst.implied = SB_OP; /* something more needed here because this is MIN<(ba)> surro */
      break;
    }  
  return gdl_meta_node(ytp, "g:z", data);
}

Node *
gdl_gloss_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: GLOSS/c: %d=%s\n", tok, data);
  ret =  gdl_meta_node(ytp, "g:z", data);
  if (!gdl_balance_state(mlp, tok, data))
    gdl_pop(ytp, data);
  gdl_update_closers(lgp, tok);
  switch (tok)
    {
    case R_cur_par:
      gst.glodoc = SB_NO;
      break;
    case R_dbl_cur:
      gst.glolin = SB_NO;
      break;
    case R_ang_par_s:
      gst.implied = SB_NO; /* something more needed here because this is MIN<(ba)> surro */
      break;
    }  
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
  ret = gdl_meta_node(ytp, "g:z", data);
  gdl_node_type(ret,gptype);
  switch (tok)
    {
    case '<':
      gst.supplied = SB_OP;
      break;
    case '(':
      gst.maybe = SB_OP;
      break;
    case L_ang_par:
      gst.implied = SB_OP;
      break;
    case L_dbl_ang:
      gst.excised = SB_OP;
      break;
    case L_dbl_par:
      gst.erased = SB_OP;
      break;
    case L_dbl_par_c:
      gst.cancelled = SB_OP;
      break;
    case eras_canc_pivot:
      gst.superposed = SB_OP;
      break;
    }  
  return ret;
}

Node *
gdl_state_c(Mloc mlp, Tree *ytp, int tok, const char *data)
{
  Node *ret = NULL;
  if (gdltrace)
    fprintf(stderr, "gt: STATE/c: %d=%s\n", tok, data);
  ret =  gdl_meta_node(ytp, "g:z", data);
#if 1
  (void)gdl_balance_state(mlp, tok, data);
  gdl_update_closers(lgp, tok);
  switch (tok)
    {
    case R_cur_par:
      gst.glodoc = SB_NO;
      break;
    case R_dbl_cur:
      gst.glolin = SB_NO;
      break;
    case R_ang_par_s:
      gst.implied = SB_NO; /* something more needed here because this is MIN<(ba)> surro */
      break;
    case '>':
      gst.supplied = SB_NO;
      break;
    case ')':
      gst.maybe = SB_NO;
      break;
    case R_ang_par:
      gst.implied = SB_NO;
      break;
    case R_dbl_ang:
      gst.excised = SB_NO;
      break;
    case R_dbl_par:
      gst.erased = SB_NO;
      break;
    case R_dbl_par_c:
      gst.cancelled = SB_NO;
      break;
    }  
#else
  if (!gdl_balance(mlp, tok, data))
    gdl_pop(ytp, data);
#endif
  return ret;
}

void
gdl_update_flags(Node *np, int tok)
{
  switch (tok)
    {
    case '*':
      np->props->u.s.f_star = 1;
      break;
    case '!':
      np->props->u.s.f_bang = 1;
      break;
    case '?':
      np->props->u.s.f_query = 1;
      break;
    case '#':
      np->props->u.s.f_hash = 1;
      break;
    case PLUS_FLAG:
      np->props->u.s.f_plus = 1;
      break;
    case UFLAG1:
      np->props->u.s.f_uf1 = 1;
      break;
    case UFLAG2:
      np->props->u.s.f_uf2 = 1;
      break;
    case UFLAG3:
      np->props->u.s.f_uf3 = 1;
      break;
    case UFLAG4:
      np->props->u.s.f_uf4 = 1;
      break;
    }
}

void
gdl_update_closers(Node *np, int tok)
{
  switch (tok)
    {
    case ']':
      np->props->u.s.lost = SB_CL;
      break;
    case R_uhs:
    case R_lhs:
      np->props->u.s.damaged = SB_CL;
      break;
    case '>':
      np->props->u.s.supplied = SB_CL;
      break;
    case ')':
      np->props->u.s.maybe = SB_CL;
      break;
    case R_ang_par:
      np->props->u.s.implied = SB_CL;
      break;
    case R_dbl_ang:
      np->props->u.s.excised = SB_CL;
      break;
    case R_dbl_par:
      np->props->u.s.erased = SB_CL;
      break;
    case R_dbl_par_c:
      np->props->u.s.cancelled = SB_CL;
      break;
    }
}

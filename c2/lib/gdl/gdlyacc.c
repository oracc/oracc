#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include "gdl.tab.h"
#include "gdl.h"
#include "gvl.h"

extern const char *currgdlfile;
extern int gdltrace, gdllineno;
extern void gdl_wrapup_buffer(void);
extern void gdl_validate(Tree *tp);

Tree *
gdlparse_string(Mloc *m, char *s)
{
  Tree *tp = tree_init();
  (void)tree_root(tp, NS_GDL, "g:gdl", 1, NULL);
  /* gdl_prop(tp->curr, 0, PG_XNS, "xmlns:g", "http://oracc.org/ns/gdl/2.0"); */
  gdl_setup_buffer(s);
  gdl_set_tree(tp);
  gdl_lex_init(m->file, m->line);
  gdlparse();
  gdl_wrapup_buffer();
  /* gdl_validate(tp); */
  return tp;
}

#if 0
void
gdl_validate(Tree *tp)
{
  tree_iterator(tp, NULL, gvl_iterator_pre_fnc, gvl_iterator_post_fnc);
}
#endif

void
gdl_remove_q_error(Mloc m)
{
  if (mesg_remove_error(m.file, m.line, "must be qualified") && gdltrace)
    mesg_err(&m, "gdl_remove_q_error succeeded");
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

Node *
gdl_delim(Tree *ytp, const char *data)
{
  Node *np = NULL;
  if (gdltrace)
    fprintf(stderr, "DELIM: %c\n", '-');
  np = tree_add(ytp, NS_GDL, "g:d", ytp->curr->depth, NULL); 
  np->text = data;
  return np;
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
gdl_graph(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "GRAPH: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:g", data);
}

Node *
gdl_nongraph(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "NONGRAPH: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:x", data);
}

Node *
gdl_listnum(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "LISTNUM: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:l", data);
}

Node *
gdl_number(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "NUMBER: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:n", data);
}

Node *
gdl_punct(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "PUNCT: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:p", data);
}

void
gdl_prop(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->propmem, ynp->props, ptype, gtype, k, v);
  else
    mesg_warning(currgdlfile, gdllineno, "gdl_prop passed NULL ynp");
}

Node *
gdl_state(Tree *ytp, const char *data)
{
  if (gdltrace)
    fprintf(stderr, "STATE: %s\n", gdllval.text);
  return gdl_graph_node(ytp, "g:z", data);
}

Node *
gdl_pop(Tree *ytp, const char *s)
{
  Node *np = tree_pop(ytp);
  if (!strcmp(np->name, s))
    /* error */;
  return np;
}

void
gdl_push(Tree *ytp, const char *s)
{
  tree_add(ytp, NS_GDL, s, ytp->curr->depth, NULL);
  tree_push(ytp);
}


#if 0
/* After refactoring the parser this became redundant but I'm keeping it around in case I ever want it again */
void
gdl_append(Node *ynp, const char *s)
{
  if (ynp)
    {
      if (ynp->data)
	{
	  char *p = (char*)pool_alloc(strlen(ynp->data)+strlen(s)+1, gdlpool);
	  (void)sprintf(p, "%s%s", ynp->data, s);
	  ynp->data = p;
	}
    }
  else
    {
      fprintf(stderr, "gdl_append passed NULL ynp\n");
    }
}
#endif

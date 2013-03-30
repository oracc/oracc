#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <psdtypes.h>
#include <hash.h>
#include "cdf.h"
#include "warning.h"
#include "run.h"
#include "tree.h"
#include "nonx.h"
#include "xmlnames.h"
#include "pool.h"
#include "globals.h"
#include "memblock.h"

/* Process an xtf:lg which contains an optional xtf:l followed by a sequence
   of zero or more xtf:v.

   The xtf:l is the composite line, and is autocreated if empty to serve as
   a column template for the score block.

   The xtf:v nodes are exemplar transliterations.

   The word matrix alternates text-content and non-text columns, the
   latter being used for, e.g., line break notations.

   We first find the line with the most text-content columns to create the
   master column template.  This is used in combination with the xtf:l+xtf:v
   count to create a column/row matrix which is then filled in from the
   g:w, g:x and g:nonw nodes.

   The result matrix is validated for matching numbers of columns in each
   row, and the xtf:lg block is rewritten to include empty nodes for any
   cell which is present in the master matrix but not in a line/exemplar.
 */

enum wm_row_type { WM_COMPOSITE , WM_EXEMPLAR };

/* 
   WM_EMPTY is a cell which is not used by any row
   WM_IMPLIED is the first text cell, when there is a line that begins with WM_INTER
   WM_INTRA is a cell with text content
   WM_INTER is a cell between text content cells 
 */
enum wm_cell_type { WM_EMPTY , WM_IMPLIED , WM_INTRA , WM_INTER };

struct wm_cell
{
  enum wm_cell_type type;
  struct node *word; /* we call all the INTER/INTRA material for a cell 'word' */
  struct wm_cell *next; /* for WM_INTER only, multiple nodes are collapsed to a single cell */
};

struct wm_row
{
  enum wm_row_type type;
  struct node *line;
  struct wm_cell *cells;
};

struct wm_block
{
  enum wm_cell_type *master;
  struct wm_row composite;
  struct wm_row *exemplars;
  int nexemplars;
  int nmasters;
};

#define wm_childCount(row) ((row)->line->children.lastused)
#define wm_child(child_index) ((struct node *)(row->line->children.nodes[child_index]))
#define wm_childEtype(child_index) (wm_child(child_index))->etype

struct mb *mb_wm_cells;

static struct node *
wm_composite_line(struct node *lg, int *composite_index)
{
  int i;
  for (i = 0; i < lg->children.lastused; ++i)
    {
      if (((struct node*)lg->children.nodes[i])->etype == e_l)
	{
	  *composite_index = i;
	  return lg->children.nodes[i];
	}
    }
  return NULL;
}

static const char *
find_nonw_type(struct node *np)
{
  if (np->etype == e_g_nonw)
    np = firstChild(np);
  return (const char *)getAttr(np, "g:type");
}

static void
wm_count_cells(struct wm_row *row, int *n_intra_p, int *n_inter_p)
{
  int i, n_intra, n_inter;
  for (i = n_intra = n_inter = 0; row->line && i < wm_childCount(row); ++i)
    {
      const char *nonw_type;
      switch (wm_childEtype(i))
	{
	case e_n_w:
	case e_g_w:
	case e_g_gloss:
	  ++n_intra;
	  break;
	case e_g_nonw:
	  nonw_type = find_nonw_type(wm_child(i));
	  if (!strcmp(nonw_type, "word-absent")
	      || !strcmp(nonw_type, "word-broken"))
	    ++n_intra;
	  else
	    ++n_inter;
	  break;
	case e_g_x:
	  ++n_inter;
	  break;
	default:
	  vwarning("wm_count_cells: child type %s not handled", wm_child(i)->names->pname);
	  break;
	}
    }
  if (n_intra_p)
    *n_intra_p = n_intra;
  if (n_inter_p)
    *n_inter_p = n_inter;
}

static void
wm_append_cell_node(struct wm_row *row, int cell_index, struct node *node)
{
  if (row->cells[cell_index].word)
    {
      struct wm_cell *tmp;
      for (tmp = &row->cells[cell_index]; tmp->next; tmp = tmp->next)
	;
      tmp->next = mb_new(mb_wm_cells);
      tmp->next->word = node;
    }
  else
    {
      row->cells[cell_index].word = node;
    }
}

static void
wm_set_cells(enum wm_cell_type *master, struct wm_row *row)
{
  int i, intra_nth;
  for (i = intra_nth = 0; row->line && i < wm_childCount(row); ++i)
    {
      int cell_index = 0;
      const char *nonw_type;

      switch (wm_childEtype(i))
	{
	case e_n_w:
	case e_g_w:
	case e_g_gloss:
	  ++intra_nth;
	  master[intra_nth * 2] = WM_INTRA;
	  row->cells[intra_nth * 2].word = wm_child(i);
	  break;
	case e_g_nonw:
	  nonw_type = find_nonw_type(wm_child(i));
	  if (!strcmp(nonw_type, "word-absent")
	      || !strcmp(nonw_type, "word-broken"))
	    {
	      ++intra_nth;
	      master[intra_nth * 2] = WM_INTRA;
	      row->cells[intra_nth * 2].word = wm_child(i);
	    }
	  else
	    {
	      if (intra_nth)
		cell_index = (intra_nth*2) + 1;
	      else
		cell_index = 1;
	      master[cell_index] = WM_INTER;
	      wm_append_cell_node(row, cell_index, wm_child(i));
	    }
	  break;
	case e_g_x:
	  if (intra_nth)
	    cell_index = (intra_nth*2) + 1;
	  else
	    cell_index = 1;
	  master[cell_index] = WM_INTER;
	  wm_append_cell_node(row, cell_index, wm_child(i));
	  break;
	default:
	  vwarning("wm_set_cells: child type %s not handled", wm_child(i)->names->pname);
	  break;
	}
    }  
}

/* Naive implementation.

   Future implementation needs to check the master vector to see if it needs 
   an empty WM_INTER node after any of the WM_INTRA nodes. */   
static void
wm_rewrite_cells(int ncells, enum wm_cell_type *master, struct wm_row *row)
{
  struct nodelist *nl = calloc(1, sizeof(struct nodelist));
  int i;
  for (i = 0; i < ncells; ++i)
    {
      struct node *wrapper = NULL;

      if (WM_EMPTY == master[i])
	continue;

      if (row->cells[i].word)
	{
	  struct wm_cell *tmp;
	  wrapper = elem(e_c, row->cells[i].word->parent, 
			 row->cells[i].word->lnum, CELL);
	  appendChild(wrapper, row->cells[i].word);
	  for (tmp = row->cells[i].next; tmp; tmp = tmp->next)
	    appendChild(wrapper, tmp->word);
	}
      else if (row->line)
	{
	  wrapper = elem(e_c, row->line, row->line->lnum, CELL);
	}
      if (wrapper)
	addToNodeList(nl, wrapper);
    }
  if (row->line)
    row->line->children = *nl;
  free(nl);
}

void
word_matrix_rewrite(struct node *lg)
{
  struct wm_block block;
  int i, composite_index, ncols;
  int max_intra = 0, max_inter = 0;
  char colsbuf[8];
  extern int max_cells;

  memset(&block, '\0', sizeof(struct wm_block));
  mb_wm_cells = mb_init(sizeof(struct wm_cell), 8);

  block.composite.line = wm_composite_line(lg, &composite_index);
  block.exemplars = calloc(lg->children.lastused - 1, sizeof(struct wm_row));
  for (i = composite_index + 1; i < lg->children.lastused; ++i)
    {
      if (((struct node*)lg->children.nodes[i])->etype == e_v)
	block.exemplars[block.nexemplars++].line = lg->children.nodes[i];
    }

  wm_count_cells(&block.composite, &max_intra, &max_inter);
  for (i = 0; i < block.nexemplars; ++i)
    {
      int tmp_intra = 0, tmp_inter = 0;
      wm_count_cells(&block.exemplars[i], &tmp_intra, &tmp_inter);
      if (tmp_intra > max_intra)
	max_intra = tmp_intra;
      if (tmp_inter > max_inter)
	max_inter = tmp_inter;
    }
  /* pre-increment the counter because column 0 is the empty 
     WM_INTRA column which precedes the possible non-empty
     WM_INTER column which can begin a line */
  if (max_intra > max_inter)
    block.nmasters = ++max_intra * 2;
  else
    block.nmasters = ++max_inter * 2;
  block.master = calloc(block.nmasters, sizeof(enum wm_cell_type));

  block.composite.cells = mb_new_array(mb_wm_cells, block.nmasters);
  wm_set_cells(block.master, &block.composite);
  for (i = 0; i < block.nexemplars; ++i)
    {
      block.exemplars[i].cells = mb_new_array(mb_wm_cells, block.nmasters);
      wm_set_cells(block.master, &block.exemplars[i]);
    }

  wm_rewrite_cells(block.nmasters, block.master, &block.composite);
  for (i = 0; i < block.nexemplars; ++i)
    wm_rewrite_cells(block.nmasters, block.master, &block.exemplars[i]);

  for (i = ncols = 0; i < block.nmasters; ++i)
    if (block.master[i])
      ++ncols;

  sprintf(colsbuf,"%d",ncols);
  setAttr(lg, a_cols, (unsigned char *)colsbuf);

  if (ncols > max_cells)
    max_cells = ncols;
  
  free(block.master);
  free(block.exemplars);
}

/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: grapheme.c,v 0.3 1997/09/08 14:50:14 sjt Exp s $
*/
#include <psd_base.h>
#include "index.h"
#include "property.h"
#include "../types.h"

struct grapheme *grapheme_list_base = NULL;
struct grapheme *grapheme_list = NULL;
struct any_type *grapheme_mm;

extern struct location8 l8;
extern Two_bytes curr_properties;
extern int curr_sentence_id, curr_lemma_id;

void
grapheme (const char *text)
{
  if (text == NULL || !strlen((const char*)text) || !curr_node)
    return;

  if (curr_node->used_flag)
    {
      curr_node->right = new_node ();
      curr_node->right->left = curr_node;
      curr_node = curr_node->right;
      curr_node->l.branch_id = curr_node->left->l.branch_id;
    }
  curr_node->used_flag = 1;
#if 1
  memcpy(&curr_node->l,&l8,sizeof(struct location8));
#else
  curr_node->l.unit_id = curr_unit_id;
  curr_node->l.word_id = curr_word_id;
#endif
  curr_node->l.start_column = start_column++;
  curr_node->l.properties = curr_properties;
  if (grapheme_list)
    {
      grapheme_list->next = new (grapheme_mm);
      grapheme_list = grapheme_list->next;
    }
  else
    {
      grapheme_list = grapheme_list_base = new (grapheme_mm);
    }
  grapheme_list->text = (unsigned char *)xstrdup((const char*)text);
  grapheme_list->node = curr_node;
  grapheme_list->next = NULL;
  curr_node->parallel_info = curr_parallel;
}

void
grapheme_inherit_preceding_properties(void)
{
  if (curr_node->left)
    curr_node->l.properties = curr_node->left->l.properties;
}

void
grapheme_decr_start_column()
{
  --start_column;
}
void
grapheme_end_column_logo()
{
  curr_node->l.end_column |= G_LOGO;
}

void
grapheme_boundary (int btype)
{
  if (curr_node)
    {
      curr_node->boundary = btype;
      if (curr_node->end_of_branch_flag)
	curr_node->parallel_info->boundary = btype;
      if (btype == ' ')
	++curr_word_id;
    }
}

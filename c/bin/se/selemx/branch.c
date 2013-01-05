/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: branch.c,v 0.3 1997/09/08 14:50:13 sjt Exp s $
*/
#include <psd_base.h>
#include "index.h"

int branch_id;
extern const char *atf_name;
extern int atf_line;

void
begin_branch ()
{
  if (parallels_depth == 0)
    {
      if (!curr_node)
	{
	  mwarning(NULL, "%s:%d: malformed XTF; NULL curr_node", atf_name, atf_line);
	  return;
	}
      if (curr_node->used_flag)
	{
	  curr_node->right = new_node ();
	  curr_node->right->left = curr_node;
	  curr_node = curr_node->right;
	  if (option_flag && curr_parallel->parent 
	      && curr_node->left->l.branch_id > 0)
	    curr_node->l.branch_id = curr_node->left->l.branch_id * -1;
	  else
	    curr_node->l.branch_id = curr_node->left->l.branch_id;
	}
      else
	{
	  if (option_flag && curr_parallel->parent)
	    curr_node->l.branch_id = branch_id * -1;
	  else
	    curr_node->l.branch_id = branch_id;
	}
      curr_parallel->curr_node = curr_node;
      ++parallels_depth;
    }
  else
    {
      curr_parallel->curr_node->down = new_node ();
      curr_parallel->curr_node->down->up 
	= curr_parallel->curr_node;
      curr_node 
	= curr_parallel->curr_node 
	  = curr_parallel->curr_node->down;
      if (option_flag)
	curr_node->l.branch_id = branch_id * -1;
      else
	{
	  if (interchangeable_flag)
	    curr_node->l.branch_id = branch_id;
	  else
	    curr_node->l.branch_id = ++branch_id;
	}
      ++parallels_depth;
    }
  start_column = curr_parallel->start_column;
  curr_word_id = curr_parallel->word_id;
}

void
end_branch ()
{
  if (!curr_node)
    return;
  if (!curr_node->used_flag)
    {
      /* branch contained no graphemes */
      curr_parallel->empty_branch_flag = 1;
      grapheme ((char*)"#");
    }
  curr_node->end_of_branch_flag = 1;
  if ((start_column-1) > logo_mask(curr_parallel->end_column))
    {
      /* branch is the widest so far in the current
	 group of parallel branches */
      int logo = is_logo(curr_parallel->end_column);
      curr_parallel->end_column = mask_sc(start_column-1);
      if (logo)
	curr_parallel->end_column |= G_LOGO;
    }
  if (curr_node->boundary)
    {
      /* branch ended with a grapheme or word boundary */
      curr_parallel->boundary = curr_node->boundary;
    }
}

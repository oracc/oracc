/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: option.c,v 0.3 1997/09/08 14:50:16 sjt Exp $
*/
#include "index.h"

/* An option is grapheme that may be in the line or not,
   a notion mainly applicable to semantic determinatives.
   In the present implementation, a `#' grapheme is
   generated for the present branch, and all sub-branches,
   including the determinative, have the main branch_id 
   * -1 as their branch_id.  The query program can test
   for adjacency as either a difference of 1 in the end_column/
   start_column, or the same end/start but with differently
   signed but otherwise identical branch_id's */

int option_flag;

void
begin_option ()
{
  ++option_flag;
  begin_parallels ();
  begin_branch ();
  end_branch ();
  begin_branch ();
}
void
end_option ()
{
  end_branch ();
  end_parallels ();
  --option_flag;
}

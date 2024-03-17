/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: intercha.c,v 0.3 1997/09/08 14:50:15 sjt Exp $
*/
#include "index.h"

/* An interchangeable is a series of graphemes that all occupy
   the same space; i.e., they all have the same branch_id,
   start_column and end_column.  This is mainly useful for
   transliteration of ambiguous graphemes, like su/zu or 
   ba/ma. */

int interchangeable_flag;

void
begin_interchangeable ()
{
  ++interchangeable_flag;
  begin_parallels ();
}
void
end_interchangeable ()
{
  end_parallels ();
  --interchangeable_flag;
}

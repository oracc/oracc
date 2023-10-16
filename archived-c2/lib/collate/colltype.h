/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: colltype.h,v 0.3 1997/09/08 14:49:54 sjt Exp s $
*/
#ifndef COLLTYPE_H_
#define COLLTYPE_H_ 1

struct Collate_info
{
  const unsigned char *name;
  char *delims;
  char *digits;
  const unsigned char ** tiles;
  const unsigned char ** tiles_i;
  int digit_offset;
};

struct Collate_info_list_node
{
  const unsigned char *name;
  struct Collate_info *coll;
};

#ifndef Uchar
#include <c2types.h>
#endif

#endif

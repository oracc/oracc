/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: mm_any.h,v 0.4 1997/09/08 14:50:12 sjt Exp $
*/
#ifndef MM_ANY_H_
#define MM_ANY_H_ 1
struct any_type 
{
  struct mem_block *mem_rover;
  struct mem_block *mem_base;
  int last_element_used;
  int elements_per_alloc;
  int element_size;
};

extern struct any_type * init_mm (int element_size, int elements_per_alloc);
extern void * new (struct any_type *any);
extern void reset (struct any_type *any);
#endif

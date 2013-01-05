/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: mm_any.h,v 0.4 1997/09/08 14:50:12 sjt Exp $
*/
#ifndef MEMBLOCK_H_
#define MEMBLOCK_H_ 1
struct mb 
{
  struct mem_block *mem_rover;
  struct mem_block *mem_base;
  int last_element_used;
  int elements_per_alloc;
  int element_size;
};

#define mb_term mb_free /* why I didn't call it mb_term I don't know */

extern struct mb * mb_init (int element_size, int elements_per_alloc);
extern void mb_free(struct mb *any);
extern void *mb_merge(struct mb *any, size_t *bytes);
extern void *mb_new (struct mb *any);
extern void *mb_new_array (struct mb *any, int count);
extern void mb_reset (struct mb *any);
#endif

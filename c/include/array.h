/*
 Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
 1997 under the GNU General Public License (see /psd/COPYING).

 $Id: array.h,v 0.3 1997/09/08 14:49:55 sjt Exp $
*/

#ifndef ARRAY_H_
#define ARRAY_H_ 1
extern const char force_array_inclusion;
extern void *array_pare (void *ap, size_t size, size_t count, int (*test)(const void *));
extern void *array_sort (void *ap, size_t size, size_t count, int (*cmp)(const void *, const void *));
extern void *array_merge (Boolean invert, void *arg_ap1, void *arg_ap2, size_t size, size_t arg_count1, size_t arg_count2, size_t *new_count, int (*cmp)(const void *, const void *));
#endif /* ARRAY_H_ */
